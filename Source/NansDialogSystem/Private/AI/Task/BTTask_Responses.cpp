// Copyright 2019 Nans Pellicari. All Rights Reserved.

#include "AI/Task/BTTask_Responses.h"

#include "AI/Task/BTTask_Countdown.h"
#include "BTDialogueResponseContainer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/Button.h"
#include "NansUE4Utilities/public/Misc/ErrorUtils.h"
#include "NansUE4Utilities/public/Misc/TextLibrary.h"
#include "Runtime/UMG/Public/Components/PanelWidget.h"
#include "UI/DialogHUD.h"
#include "UI/ResponseButtonWidget.h"
#include "UI/WheelProgressBarWidget.h"

#define LOCTEXT_NAMESPACE "DialogSystem"

UBTTask_Responses::UBTTask_Responses(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	bNotifyTick = true;
	bNotifyTaskFinished = true;

	CountDownTask = objectInitializer.CreateDefaultSubobject<UBTTask_Countdown>(this, TEXT("Countdown"));
	CountDownTask->OnCountdownEnds().AddUObject(this, &UBTTask_Responses::OnCountdownEnds);
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
	if (HUD.IsNone())
	{
		EDITOR_ERROR("DialogSystem", LOCTEXT("NotSetHUDKey", "Set a key value for HUD in "));
		return EBTNodeResult::Aborted;
	}
	DialogHUD = GetHUDFromBlackboard(Blackboard);
	DialogHUD->OnEndDisplayResponse.AddDynamic(this, &UBTTask_Responses::OnEndDisplayResponse);

	if (!ensure(DialogHUD != nullptr))
	{
		EDITOR_ERROR("DialogSystem", LOCTEXT("WrongHUDType", "The HUD set is not valid (UDialogHUD is expected) in "));
		return EBTNodeResult::Aborted;
	}

	ResponsesSlot = Cast<UPanelWidget>(DialogHUD->FindWidget(ResponsesSlotName));

	if (!ensure(ResponsesSlot != nullptr))
	{
		EDITOR_ERROR("DialogSystem", LOCTEXT("WrongResponsesSlotName", "The ResponsesSlotName set doesn't exists in HUD in "));
		return EBTNodeResult::Aborted;
	}

	if (!ensure(ResponseButtonWidget != NULL && !ResponseButtonWidget->GetClass()->GetFullName().IsEmpty()))
	{
		EDITOR_ERROR("DialogSystem", LOCTEXT("WrongResponseButtonWidget", "Need to set a ResponseButtonWidget in "));
		return EBTNodeResult::Aborted;
	}

	UWheelProgressBarWidget* TimeWidget = Cast<UWheelProgressBarWidget>(DialogHUD->FindWidget("ProgressBar"));
	if (TimeWidget != nullptr)
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
		// Wait for DialogHUD ending display response
		return;
	}

	if (ReponsesUP.Num() <= 0 && ReponsesDOWN.Num() <= 0 && NeutralResponse.IsEmpty())
	{
		// I don't know why someone can create this settings, but it shouldn't stop the process
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	if (!ensure(DialogHUD != nullptr))
	{
		EDITOR_ERROR("DialogSystem", LOCTEXT("WrongHUDType", "The HUD set is not valid (UDialogHUD is expected) in "));
		FinishLatentTask(OwnerComp, EBTNodeResult::Aborted);
		return;
	}

	if (!ensure(ResponsesSlot != nullptr))
	{
		EDITOR_ERROR("DialogSystem", LOCTEXT("WrongResponsesSlotName", "The ResponsesSlotName set not exists in HUD in "));
		FinishLatentTask(OwnerComp, EBTNodeResult::Aborted);
		return;
	}

	CountDownTask->TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	ReceiveOnTick(OwnerComp, NodeMemory, DeltaSeconds);

	FinishLatentTask(OwnerComp, EBTNodeResult::InProgress);
}

void UBTTask_Responses::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
	ResponseStatus = EBTNodeResult::InProgress;
	CountDownTask->OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
	NeutralResponseIndex = -1;

	if (ResponsesSlot == nullptr)
	{
		EDITOR_ERROR("DialogSystem", LOCTEXT("NoResponseSlot", "No responses slot in "));
		FinishLatentTask(OwnerComp, EBTNodeResult::Aborted);
		return;
	}

	while (ResponsesSlot->GetChildrenCount() > 0)
	{
		UUserWidget* Widget = Cast<UUserWidget>(ResponsesSlot->GetChildAt(0));

		if (Widget == nullptr)
		{
			EDITOR_ERROR("DialogSystem", LOCTEXT("WrongWidgetInResponsesSlot", "A Wrong object type is in the responses slot in "));
			FinishLatentTask(OwnerComp, EBTNodeResult::Aborted);
			return;
		}
		UResponseButtonWidget* Button = Cast<UResponseButtonWidget>(Widget);

		if (Button != nullptr)
		{
			Button->OnBTClicked.Clear();
			DialogHUD->OnRemovedChild.Broadcast(Button);
			Button->MarkPendingKill();
		}

		ResponsesSlot->RemoveChildAt(0);
	}

	ListButtonIndexes.Empty();

	if (DialogHUD == nullptr) return;

	DialogHUD->OnEndDisplayResponse.RemoveAll(this);
	DialogHUD->Reset();
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
			CreateButton(ReponsesUP[i], Index++, ++Position, ReponsesUP.Last().Level);
		}
	}

	Position = 0;
	FBTDialogueResponse NeutralDialogueResponse;
	NeutralDialogueResponse.Alignment = EAlignment::Neutral;
	NeutralDialogueResponse.Level = NeutralResponsePoint;
	NeutralDialogueResponse.Text = NeutralResponse;
	NeutralResponseIndex = Index++;
	CreateButton(NeutralDialogueResponse, NeutralResponseIndex, Position, 0);

	if (ReponsesDOWN.Num() > 0)
	{
		for (int8 i = 0; i < ReponsesDOWN.Num(); ++i)
		{
			CreateButton(ReponsesDOWN[i], Index++, ++Position, ReponsesDOWN.Last().Level);
		}
	}
}

void UBTTask_Responses::CreateButton(FBTDialogueResponse Response, int8 Index, int32 Position, int32 MaxLevel)
{
	FString UniqName = UNTextLibrary::UniqueString(Response.Text);

	UResponseButtonWidget* ButtonWidget = CreateWidget<UResponseButtonWidget>(GetWorld(), ResponseButtonWidget);

	if (!ensure(ButtonWidget != nullptr))
	{
		FinishLatentAbort(*OwnerComponent);
		return;
	}

	UBTDialogueResponseContainer* ResponseContainer =
		NewObject<UBTDialogueResponseContainer>((UObject*) OwnerComponent, UBTDialogueResponseContainer::StaticClass());
	ResponseContainer->SetResponse(Response);
	ResponseContainer->DisplayOrder = Position;
	ResponseContainer->InfluencedBy = Position > 0 ? EResponseDirection::DOWN : EResponseDirection::UP;
	ButtonWidget->SetResponse(ResponseContainer);

	ButtonWidget->OnBTClicked.AddDynamic(this, &UBTTask_Responses::OnButtonClicked);
	ButtonWidget->ComputeColor(MaxLevel);
	ListButtonIndexes.Add(UniqName, Index);
	ResponsesSlot->AddChild(ButtonWidget);
	DialogHUD->OnAddedChild.Broadcast(ButtonWidget);
}

void UBTTask_Responses::OnButtonClicked(UResponseButtonWidget* ButtonWidget)
{
	if (!ensure(Blackboard != nullptr))
	{
		FinishLatentAbort(*OwnerComponent);
		return;
	}

	UBTDialogueResponseContainer* ResponseContainer = ButtonWidget->GetResponse();
	Blackboard->SetValueAsObject(ResponseContainerName, ResponseContainer);

	DialogHUD->OnResponse.Broadcast(ButtonWidget->GetResponse()->GetResponse().Text);
	ResponseStatus = EBTNodeResult::Succeeded;
}

void UBTTask_Responses::OnCountdownEnds(UBehaviorTreeComponent* OwnerComp)
{
	UResponseButtonWidget* ButtonWidget = Cast<UResponseButtonWidget>(ResponsesSlot->GetChildAt(NeutralResponseIndex));
	UBTDialogueResponseContainer* DialogueResponse = UBTDialogueResponseContainer::CreateNullObject(OwnerComp);

	// TODO set a random value depending on the response setting + weighting with player class level
	// eg. More chance to have a random CSV response if player is better in CNV: this to push the frustration up
	if (ButtonWidget != nullptr)
	{
		DialogueResponse = ButtonWidget->GetResponse();
	}
	else
	{
		FBTDialogueResponse Response = DialogueResponse->GetResponse();
		Response.Alignment = EAlignment::Neutral;
		Response.Level = 0;
		DialogueResponse->SetResponse(Response);
	}

	DialogHUD->OnResponse.Broadcast(DialogueResponse->GetResponse().Text);
	Blackboard->SetValueAsObject(ResponseContainerName, DialogueResponse);
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
	int32 Position = 0;
	ReturnDesc +=
		DisplayStaticResponses(ReponsesUP, Position, LOCTEXT("CNVNodeResponsesTitle", "[ UP Responses ]\n").ToString(), true);
	if (NeutralResponse.IsEmpty() == false)
	{
		Position = 0;
		ReturnDesc += "\n----------------------\n";
		ReturnDesc += LOCTEXT("CNVNodeResponsesTitle", "[ Neutral Response ]\n").ToString();
		FFormatNamedArguments Arguments;
		Arguments.Add(TEXT("response"), NeutralResponse);
		Arguments.Add(TEXT("level"), NeutralResponsePoint);
		Arguments.Add(TEXT("position"), Position++);
		ReturnDesc +=
			FText::Format(LOCTEXT("CNVNodeNeutralResponse", "\t- \"{response}\"\n\tPosition: {position}, lvl: {level}"), Arguments)
				.ToString();
		ReturnDesc += "\n----------------------\n";
	}
	ReturnDesc +=
		DisplayStaticResponses(ReponsesDOWN, Position, LOCTEXT("CNVNodeResponsesTitle", "[ DOWN Responses ]\n").ToString(), false);

	ReturnDesc += "\nHUD: " + HUD.ToString();
	ReturnDesc += "\nResponse Container: " + ResponseContainerName.ToString();
	ReturnDesc += "\nResponse Button Widget: ";
	if (ResponseButtonWidget != NULL && !ResponseButtonWidget->GetDefaultObject()->GetClass()->GetFullName().IsEmpty())
	{
		ReturnDesc += ResponseButtonWidget->GetDefaultObject()->GetClass()->GetName();
	}
	else
	{
		ReturnDesc += "None";
	}

	ReturnDesc += "\nResponses Slot Name: " + ResponsesSlotName.ToString();

	ReturnDesc += "\n----------------------\n";
	ReturnDesc += CountDownTask->GetStaticDescription();
	ReturnDesc += "\n----------------------\n";

	return ReturnDesc;
}

FString UBTTask_Responses::DisplayStaticResponses(
	const TArray<FBTDialogueResponse>& Responses, int32& Position, FString Title, bool Reverse) const
{
	FString ReturnDesc;
	if (Responses.Num() > 0)
	{
		ReturnDesc += "\n----------------------\n";
		ReturnDesc += Title;
		for (int8 i = 0; i < Responses.Num(); ++i)
		{
			FBTDialogueResponse Response = Responses[i];
			FFormatNamedArguments Arguments;
			FString Text = UNTextLibrary::StringToLines("- \"" + Response.Text.ToString() + "\"", 60, "\t");
			Arguments.Add(TEXT("response"), FText::FromString(Text));
			Arguments.Add(TEXT("level"), Response.Level);
			Arguments.Add(TEXT("difficulty"), Response.DifficultyLevel);
			Arguments.Add(TEXT("alignment"), FText::FromString(EnumToString(EAlignment, Response.Alignment)));
			Arguments.Add(TEXT("position"), Reverse ? --Position : Position++);
			ReturnDesc += FText::Format(
				LOCTEXT("NodeResponsesDetails",
					"{response}\n\tPosition: {position}, difficulty: {difficulty}, lvl: {level}, alignment: {alignment}"),
				Arguments)
							  .ToString();
			ReturnDesc += i < Responses.Num() - 1 ? "\n\n" : "\n";
		}
		ReturnDesc += "----------------------\n";
	}
	return ReturnDesc;
}

UDialogHUD* UBTTask_Responses::GetHUDFromBlackboard(UBlackboardComponent* _Blackboard)
{
	return Cast<UDialogHUD>(_Blackboard->GetValueAsObject(HUD));
}

void UBTTask_Responses::ReceiveOnTick(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) {}
#undef LOCTEXT_NAMESPACE
