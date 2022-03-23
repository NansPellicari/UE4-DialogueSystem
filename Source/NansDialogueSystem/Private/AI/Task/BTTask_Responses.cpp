// Copyright 2020-present Nans Pellicari (nans.pellicari@gmail.com).
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "AI/Task/BTTask_Responses.h"

#include "NDialogueSubsystem.h"
#include "NDSFunctionLibrary.h"
#include "PointSystemHelpers.h"
#include "AI/Task/BTTask_Countdown.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NansUE4Utilities/public/Misc/ErrorUtils.h"
#include "NansUE4Utilities/public/Misc/TextLibrary.h"
#include "Runtime/UMG/Public/Components/PanelWidget.h"
#include "Service/BTDialoguePointsHandler.h"
#include "Service/DialogueBTHelpers.h"
#include "Setting/DialogueSystemSettings.h"
#include "UI/DialogueProgressBarWidget.h"
#include "UI/DialogueUI.h"
#include "UI/ResponseButtonWidget.h"

#define LOCTEXT_NAMESPACE "DialogSystem"

UBTTask_Responses::UBTTask_Responses(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	bNotifyTick = true;
	bNotifyTaskFinished = true;

	CountDownTask = objectInitializer.CreateDefaultSubobject<UBTTask_Countdown>(this, TEXT("Countdown"));
	CountDownTask->OnCountdownEnds().AddUObject(this, &UBTTask_Responses::OnCountdownEnds);
	const auto Settings = UDialogueSystemSettings::Get()->BehaviorTreeSettings;

	if (UINameKey == NAME_None)
	{
		UINameKey = Settings.UINameKey;
	}
}

#if WITH_EDITOR
void UBTTask_Responses::PostLoad()
{
	Super::PostLoad();
	CountDownTask->SetTimeToResponse(TimeToResponse);
}

void UBTTask_Responses::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	CountDownTask->SetTimeToResponse(TimeToResponse);
}
#endif	  // WITH_EDITOR

EBTNodeResult::Type UBTTask_Responses::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	OwnerComponent = &OwnerComp;
	Blackboard = OwnerComp.GetBlackboardComponent();
	if (UINameKey.IsNone())
	{
		EDITOR_ERROR("DialogSystem", LOCTEXT("NotSetUIKey", "Set a key value for UI in "));
		return EBTNodeResult::Aborted;
	}

	const auto Settings = UDialogueSystemSettings::Get()->BehaviorTreeSettings;
	const AAIController* AIOwner = OwnerComp.GetAIOwner();
	check(IsValid(AIOwner));
	UNDialogueSubsystem* DialSys = UNDSFunctionLibrary::GetDialogSubsystem();
	check(IsValid(DialSys));
	PointsHandler = DialSys->GetPointsHandler(AIOwner);
	if (!ensure(PointsHandler.IsValid())) return EBTNodeResult::Aborted;

	DialogueUI = NDialogueBTHelpers::GetUIFromBlackboard(OwnerComp, Blackboard);
	if (!IsValid(DialogueUI))
	{
		// Error is already manage in NDialogueBTHelpers::GetUIFromBlackboard()
		return EBTNodeResult::Aborted;
	}

	DialogueUI->OnEndDisplayResponse.AddDynamic(this, &UBTTask_Responses::OnEndDisplayResponse);

	ResponsesSlot = DialogueUI->GetResponsesSlot();

	UDialogueProgressBarWidget* TimeWidget = DialogueUI->GetProgressBar();
	if (IsValid(TimeWidget))
	{
		CountDownTask->Initialize(TimeWidget, TimeToResponse);
		CountDownTask->ExecuteTask(OwnerComp, NodeMemory);
	}

	CreateButtons();

	return EBTNodeResult::InProgress;
}

void UBTTask_Responses::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (ResponseStatus == EBTNodeResult::Succeeded)
	{
		// Wait for DialogueUI ending display response
		return;
	}

	if (ReponsesUP.Num() <= 0 && ReponsesDOWN.Num() <= 0 && MiddleResponse.IsEmpty())
	{
		// I don't know why someone can create this settings, but it shouldn't stop the process
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	if (!ensure(IsValid(DialogueUI)))
	{
		EDITOR_ERROR("DialogSystem", LOCTEXT("WrongUIType", "The UI set is not valid (UDialogueUI is expected) in "));
		FinishLatentTask(OwnerComp, EBTNodeResult::Aborted);
		return;
	}

	CountDownTask->TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	ReceiveOnTick(OwnerComp, NodeMemory, DeltaSeconds);

	FinishLatentTask(OwnerComp, EBTNodeResult::InProgress);
}

void UBTTask_Responses::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
	ResponseStatus = EBTNodeResult::InProgress;
	CountDownTask->OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
	MiddleResponseIndex = -1;

	const bool bHasRemoved = DialogueUI->RemoveResponses();

	if (!bHasRemoved)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Aborted);
		return;
	}

	if (DialogueUI == nullptr) return;

	DialogueUI->OnEndDisplayResponse.RemoveAll(this);
	DialogueUI->Reset();
}

void UBTTask_Responses::CreateButtons()
{
	int8 Index = 0;
	int32 Position = -ReponsesUP.Num() - 1;
	if (ReponsesUP.Num() > 0)
	{
		// reverse loop to set the right display order
		for (int8 i = ReponsesUP.Num() - 1; i >= 0; --i)
		{
			CreateButton(ReponsesUP[i], Index++, ++Position, ReponsesUP.Last().Point);
		}
	}

	Position = 0;
	FBTDialogueResponse MiddleDialogueResponse;
	MiddleDialogueResponse.Category = MiddleResponseCategory;
	MiddleDialogueResponse.Point = MiddleResponsePoint;
	MiddleDialogueResponse.Text = MiddleResponse;
	MiddleResponseIndex = Index++;
	CreateButton(MiddleDialogueResponse, MiddleResponseIndex, Position, 1);

	if (ReponsesDOWN.Num() > 0)
	{
		for (int8 i = 0; i < ReponsesDOWN.Num(); ++i)
		{
			CreateButton(ReponsesDOWN[i], Index++, ++Position, ReponsesDOWN.Last().Point);
		}
	}
}

void UBTTask_Responses::CreateButton(FBTDialogueResponse Response, int8 Index, int32 Position, int32 MaxLevel)
{
	FResponseButtonBuilderData BuilderData;
	BuilderData.Response = Response;
	BuilderData.DisplayOrder = Position;
	BuilderData.MaxPoints = MaxLevel;
	BuilderData.InfluencedBy = Position > 0 ? EResponseDirection::DOWN : EResponseDirection::UP;

	UResponseButtonWidget* ButtonWidget = DialogueUI->AddNewResponse(BuilderData);
	if (!ensure(ButtonWidget != nullptr))
	{
		FinishLatentAbort(*OwnerComponent);
		return;
	}
	// TODO Change this and instead listen DialogueUI->OnResponse (with more params)
	// TODO should be listen by DialogueUI instead, see OnButtonClicked to see what event this should listen
	ButtonWidget->OnBTClicked.AddDynamic(this, &UBTTask_Responses::OnButtonClicked);
}

void UBTTask_Responses::OnButtonClicked(UResponseButtonWidget* ButtonWidget)
{
	if (!ensure(Blackboard != nullptr))
	{
		FinishLatentAbort(*OwnerComponent);
		return;
	}

	const FBTDialogueResponse Response = ButtonWidget->GetResponse();

	PointsHandler->AddPoints(FNPoint(Response), ButtonWidget->DisplayOrder);

	// TODO maybe instead of empty title should output a readable Response.Category
	// TODO Should be call by DialogueUI itself
	DialogueUI->SetPlayerText(Response.Text, FText::GetEmpty());
	ResponseStatus = EBTNodeResult::Succeeded;
}

void UBTTask_Responses::OnCountdownEnds(UBehaviorTreeComponent* OwnerComp)
{
	UResponseButtonWidget* ButtonWidget = Cast<UResponseButtonWidget>(ResponsesSlot->GetChildAt(MiddleResponseIndex));
	FBTDialogueResponse Response;
	int32 DisplayOrder = 0;

	// TODO set a random value depending on the response setting + weighting with player class level
	// eg. More chance to have a random CSV response if player is better in CNV: this to push the frustration up
	if (ButtonWidget != nullptr)
	{
		Response = ButtonWidget->GetResponse();
		DisplayOrder = ButtonWidget->DisplayOrder;
	}
	else
	{
		Response.Category = MiddleResponseCategory;
		Response.Point = 0;
	}

	PointsHandler->AddPoints(FNPoint(Response), DisplayOrder);

	// TODO maybe instead of empty title should output a readable Response.Category
	// TODO Should be call by dialog UI itself
	DialogueUI->SetPlayerText(Response.Text, FText::GetEmpty());
	ResponseStatus = EBTNodeResult::Succeeded;
}

void UBTTask_Responses::OnEndDisplayResponse()
{
	if (OwnerComponent == nullptr) return;
	FinishLatentTask(*OwnerComponent, EBTNodeResult::Succeeded);
}

FString UBTTask_Responses::GetStaticDescription() const
{
	FString ReturnDesc;
	ReturnDesc +=
		DisplayStaticResponses(
			ReponsesUP,
			LOCTEXT("UpResponsesTitle", "[ UP Responses ]\n").ToString(),
			true
		);
	if (MiddleResponse.IsEmpty() == false)
	{
		ReturnDesc +=
			DisplayStaticResponses(
				TArray<FBTDialogueResponse>(
					{FBTDialogueResponse(MiddleResponseCategory, MiddleResponse, MiddleResponsePoint)}
				),
				LOCTEXT("MiddleResponsesTitle", "[ Middle Responses ]\n").ToString(),
				false,
				0
			);
	}
	ReturnDesc +=
		DisplayStaticResponses(
			ReponsesDOWN,
			LOCTEXT("DownResponsesTitle", "[ DOWN Responses ]\n").ToString(),
			false
		);

	ReturnDesc += "\n----------------------\n";
	ReturnDesc += CountDownTask->GetStaticDescription();
	ReturnDesc += "\n----------------------\n";

	return ReturnDesc;
}

FString UBTTask_Responses::DisplayStaticResponse(const FBTDialogueResponse& Response, int32 Position) const
{
	FString ReturnDesc;
	const FString Str = FString::Printf(TEXT("[%i]"), Position);
	const FString Text = UNTextLibrary::StringToLines(Str + " \"" + Response.Text.ToString() + "\"", 60, "\t");
	ReturnDesc += FString::Printf(TEXT("%s"), *Text);
	if (bShowDialogueDetails)
	{
		FFormatNamedArguments Arguments;
		Arguments.Add(TEXT("point"), Response.Point);
		Arguments.Add(TEXT("difficulty"), Response.Difficulty);
		Arguments.Add(TEXT("category"), FText::FromString(Response.Category.Name.ToString()));
		const FText EffectName = IsValid(Response.GetSpawnableEffectOnEarned())
									 ? FText::FromString(Response.GetSpawnableEffectOnEarned()->GetName())
									 : FText::FromString(TEXT("No effect"));
		Arguments.Add(TEXT("effect"), EffectName);
		ReturnDesc += FText::Format(
				LOCTEXT(
					"NodeResponsesDetails",
					"\n\tdifficulty: {difficulty}, point: {point}, category: {category}\n\tEffect: {effect}"
				),
				Arguments
			)
			.ToString();
	}
	return ReturnDesc;
}

FString UBTTask_Responses::DisplayStaticResponses(
	const TArray<FBTDialogueResponse>& Responses, FString Title, bool Reverse, int32 ForcePosition) const
{
	FString ReturnDesc;
	if (Responses.Num() > 0)
	{
		ReturnDesc += "\n----------------------\n";
		ReturnDesc += Title;
		int32 i = Reverse ? -Responses.Num() : 0;
		const int32 Total = Reverse ? 0 : Responses.Num();
		while (i != Total)
		{
			const int32 Idx = Reverse ? -i - 1 : i;
			FBTDialogueResponse Response = Responses[Idx];
			int32 Position = Reverse ? i : i + 1;
			Position = ForcePosition != -1000 ? ForcePosition : Position;
			ReturnDesc += DisplayStaticResponse(Response, Position);
			ReturnDesc += i < Responses.Num() - 1 ? "\n\n" : "\n";
			++i;
		}
		ReturnDesc += "----------------------\n";
	}
	return ReturnDesc;
}

void UBTTask_Responses::ReceiveOnTick(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) {}

#if WITH_EDITOR
FName UBTTask_Responses::GetNodeIconName() const
{
	// TODO import my own icon
	return FName("BTEditor.Graph.BTNode.Task.PlaySound.Icon");
}
#endif	  // WITH_EDITOR
#undef LOCTEXT_NAMESPACE
