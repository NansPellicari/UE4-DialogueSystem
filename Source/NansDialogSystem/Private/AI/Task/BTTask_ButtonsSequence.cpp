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

#include "AI/Task/BTTask_ButtonsSequence.h"

#include "PointSystemHelpers.h"
#include "AI/Task/BTTask_Countdown.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NansUE4Utilities/public/Misc/ErrorUtils.h"
#include "NansUE4Utilities/public/Misc/TextLibrary.h"
#include "Service/BTDialogPointsHandler.h"
#include "Service/ButtonSequenceMovementManager.h"
#include "Service/DialogBTHelpers.h"
#include "Service/InteractiveBTHelpers.h"
#include "Misc/NansArrayUtils.h"
#include "Setting/DialogSystemSettings.h"
#include "Setting/InteractiveSettings.h"
#include "UI/ButtonSequenceWidget.h"
#include "UI/DialogHUD.h"
#include "UI/InteractiveHUDPlayer.h"
#include "UI/ResponseButtonWidget.h"
#include "UI/WheelButtonWidget.h"
#include "UI/WheelProgressBarWidget.h"

#define LOCTEXT_NAMESPACE "DialogSystem"

TSubclassOf<UGameplayEffect> FBTButtonSequenceResponse::GetSpawnableEffectOnEarned() const
{
	auto GEffect = EffectOnEarned;
	if (!IsValid(GEffect))
	{
		for (const auto& CatSet : UDialogSystemSettings::Get()->ResponseCategorySettings)
		{
			if (CatSet.Name == Category.Name)
			{
				GEffect = CatSet.DefaultEffect;
				break;
			}
		}
		if (!IsValid(GEffect))
		{
			return nullptr;
		}
	}
	return GEffect;
}

UBTTask_ButtonsSequence::UBTTask_ButtonsSequence(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bNotifyTick = true;
	bNotifyTaskFinished = true;

	BTSequenceManager = MakeShareable(new NButtonSequenceMovementManager());
	CountDownTask = ObjectInitializer.CreateDefaultSubobject<UBTTask_Countdown>(this, TEXT("Countdown"));
	CountDownTask->OnCountdownEnds().AddUObject(this, &UBTTask_ButtonsSequence::OnCountdownEnds);
	auto Settings = UInteractiveSettings::Get()->BehaviorTreeSettings;

	if (UINameKey == NAME_None)
	{
		UINameKey = Settings.UINameKey;
	}
}

void UBTTask_ButtonsSequence::BeginDestroy()
{
	Super::BeginDestroy();
	BTSequenceManager.Reset();
}

#if WITH_EDITOR
void UBTTask_ButtonsSequence::PostLoad()
{
	Super::PostLoad();
	CountDownTask->SetTimeToResponse(TimeToResponse);
}

void UBTTask_ButtonsSequence::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	CountDownTask->SetTimeToResponse(TimeToResponse);
}
#endif	  // WITH_EDITOR

EBTNodeResult::Type UBTTask_ButtonsSequence::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	OwnerComponent = &OwnerComp;
	Blackboard = OwnerComp.GetBlackboardComponent();
	if (UINameKey.IsNone())
	{
		EDITOR_ERROR("DialogSystem", LOCTEXT("NotSetHUDKey", "Set a key value for HUD in "));
		return EBTNodeResult::Aborted;
	}

	PointsHandler = Cast<UBTDialogPointsHandler>(
		Blackboard->GetValueAsObject(PointsHandlerKeyName)
	);

	if (!IsValid(PointsHandler))
	{
		EDITOR_ERROR("DialogSystem", LOCTEXT("NotPointsHandler", "Set a UBTDialogPointsHandler in "));
		return EBTNodeResult::Aborted;
	}

	DialogHUD = NDialogBTHelpers::GetHUDFromBlackboard(OwnerComp, Blackboard);
	if (!IsValid(DialogHUD))
	{
		// Error is already manage in NDialogBTHelpers::GetHUDFromBlackboard()
		return EBTNodeResult::Aborted;
	}

	DialogHUD->OnEndDisplayResponse.AddDynamic(this, &UBTTask_ButtonsSequence::OnEndDisplayResponse);

	ButtonsSlot = DialogHUD->GetResponsesSlot();

	UWheelButtonWidget* WheelButton = DialogHUD->GetWheelButton();

	BTSequenceManager->Initialize(ButtonsSlot, DefaultVelocity, WheelButton);
	CreateButtons();

	UWheelProgressBarWidget* TimeWidget = DialogHUD->GetProgressBar();
	if (IsValid(TimeWidget))
	{
		CountDownTask->Initialize(TimeWidget, TimeToResponse);
		CountDownTask->ExecuteTask(OwnerComp, NodeMemory);
	}

	return EBTNodeResult::InProgress;
}

void UBTTask_ButtonsSequence::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (TriesStatus == EBTNodeResult::Succeeded)
	{
		// Wait for DIalogHUD
		FinishLatentTask(OwnerComp, EBTNodeResult::InProgress);
		return;
	}

	BTSequenceManager->MoveButtons(DeltaSeconds);
	CountDownTask->TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	FinishLatentTask(OwnerComp, EBTNodeResult::InProgress);
}

void UBTTask_ButtonsSequence::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTNodeResult::Type TaskResult)
{
	CountDownTask->OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
	PlayerTries.Empty();
	TriesStatus = EBTNodeResult::InProgress;
	SequenceIndex = -1;
	BTSequenceManager->Reset();
	DialogHUD->OnEndDisplayResponse.RemoveAll(this);
	DialogHUD->Reset();
}

void UBTTask_ButtonsSequence::RemoveButtons(UBehaviorTreeComponent& OwnerComp)
{
	const bool bHasRemoved = DialogHUD->RemoveResponses();

	if (!bHasRemoved)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Aborted);
	}
}

void UBTTask_ButtonsSequence::OnCountdownEnds(UBehaviorTreeComponent* OwnerComp)
{
	PointsHandler->AddPoints(FNPoint(FBTDialogueResponse()), SequenceIndex);
	RemoveButtons(*OwnerComp);
	FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
}

void UBTTask_ButtonsSequence::OnEndDisplayResponse()
{
	if (OwnerComponent == nullptr) return;
	FinishLatentTask(*OwnerComponent, EBTNodeResult::Succeeded);
}

void UBTTask_ButtonsSequence::CreateButtons()
{
	if (Sequences.Num() == 0)
	{
		return;
	}

	int32 Position = 0;
	for (FBTButtonSequence Sequence : Sequences)
	{
		FString StringSequence = Sequence.ButtonSequence.ToString();
		FBTDialogueResponse Response = Sequence.ToDialogueResponse();
		for (TCHAR Char : StringSequence)
		{
			Response.AltText = FText::FromString(FString("") + Char);
			FResponseButtonBuilderData BuilderData;
			BuilderData.Response = Response;
			BuilderData.DisplayOrder = Position;
			BuilderData.MaxPoints = Sequence.LevelCoefficient;
			BuilderData.InfluencedBy = Sequence.Direction;

			UResponseButtonWidget* ButtonWidget = DialogHUD->AddNewResponse(BuilderData);

			ButtonWidget->OnBTClicked.AddDynamic(this, &UBTTask_ButtonsSequence::OnButtonClick);

			Position++;
		}
	}

	BTSequenceManager->PlaceButtons();
}

int32 UBTTask_ButtonsSequence::GetEarnedPoint()
{
	if (SequenceIndex < 0 || SequenceIndex >= Sequences.Num())
	{
		EDITOR_ERROR(
			"DialogSystem",
			LOCTEXT("NoSequenceHasBeenChosen", "A sequence is required to get the earned point in ")
		);
		return 0;
	}
	FBTButtonSequence Sequence = Sequences[SequenceIndex];
	return PointsComparedToSequence(PlayerTries, Sequence);

	// Permutation distances
	/// This allow to divid the permutations distance with this coefficient to compute earned point.
	/// So when there is no distance the max value earnable is this number.
	/// eg. if we set levelCoefficient of 3 for the sequence OSBD:
	/// if the player gives BDOS so a total of 8 distance (respective distances are 2, 2, -2, -2)
	/// the point earned is Round(3/8) = 0, For OBSD (2 tot. dist.) Round(3/2) = 2
	// TMap<FString, int32> Results;
	// UNTextLibrary::ComparePermutations(StringSequence, PlayerTries, Results);
	// for (TPair<FString, int32> Pair : Results) {
	//    Total += FMath::Abs<int32>(Pair.Value);
	//}
	// Total = Total == 0 ? 1 : Total;
	// return FMath::RoundToInt((float)Sequence.LevelCoefficient / Total);
}

const int32 UBTTask_ButtonsSequence::PointsComparedToSequence(const FString& Tries,
	const FBTButtonSequence& Sequence) const
{
	FString StringSequence = Sequence.ButtonSequence.ToString();

	if (StringSequence == Tries)
	{
		// Ok good it's exactly the same
		return Sequence.LevelCoefficient;
	}

	float Total = 0;
	float found = 0;

	// Letters in order
	for (int32 i = Tries.Len(); i > 0; --i)
	{
		int32 IndexesPossibles = Tries.Len() - i;
		for (int32 j = 0; j <= IndexesPossibles; j++)
		{
			FString Chunck = Tries.Mid(j, i);
			if (StringSequence.Find(Chunck) >= 0)
			{
				found = i;
				break;
			}
		}
		if (found > 0)
		{
			break;
		}
	}
	found = found == 1 ? 0 : found;
	return FMath::FloorToInt((found / static_cast<float>(Tries.Len())) * Sequence.LevelCoefficient);
}

void UBTTask_ButtonsSequence::OnButtonClick(UResponseButtonWidget* Button)
{
	PlayerTries += Button->GetText();
	FText SequenceTxt;

	for (int32 Index = 0; Index != Sequences.Num(); ++Index)
	{
		FBTButtonSequence Seq = Sequences[Index];
		FString StringSequence = Seq.ButtonSequence.ToString();
		if (StringSequence.Len() == PlayerTries.Len() && UNTextLibrary::ArePermutations(StringSequence, PlayerTries))
		{
			Button->SetVisibility(ESlateVisibility::Collapsed);
			TriesStatus = EBTNodeResult::Succeeded;
			SequenceIndex = Index;
			SequenceTxt = Seq.ButtonSequence;
			break;
		}

		if (UNTextLibrary::LettersAreInString(StringSequence, PlayerTries))
		{
			Button->SetVisibility(ESlateVisibility::Collapsed);
			// Ok, go on player !!
			return;
		}
	}

	if (TriesStatus == EBTNodeResult::Succeeded)
	{
		if (SequenceIndex < 0 || SequenceIndex >= Sequences.Num())
		{
			EDITOR_ERROR(
				"DialogSystem",
				LOCTEXT("NoSequenceHasBeenChosen", "A sequence is required to get the earned point in ")
			);
			FinishLatentTask(*OwnerComponent, EBTNodeResult::Aborted);
			return;
		}
		auto Response = Button->GetResponse();
		int32 TotalPoint = GetEarnedPoint();
		FBTButtonSequence Sequence = Sequences[SequenceIndex];
		auto Resp = Sequence.GetResponseForPoints(TotalPoint);
		FNPoint Point;
		Point.Category = Response.Category;
		Point.Point = TotalPoint;
		Point.Response = SequenceTxt;
		Point.EffectOnEarned = Resp.GetSpawnableEffectOnEarned();
		Point.Difficulty = static_cast<float>(Response.Difficulty);

		PointsHandler->AddPoints(Point, SequenceIndex);

		DialogHUD->SetPlayerText(Resp.Response, FText::GetEmpty());
		RemoveButtons(*OwnerComponent);
		return;
	}

	// here because the last letter disable the tried sequence
	// So restart the tries
	PlayerTries.Empty();
	SequenceIndex = -1;
	DialogHUD->ChangeButtonsVisibility(ESlateVisibility::Visible);
}

FString UBTTask_ButtonsSequence::GetStaticDescription() const
{
	FString Desc = "\nDefault velocity: " + FString::FromInt(DefaultVelocity);

	int32 i = 0;
	for (FBTButtonSequence Sequence : Sequences)
	{
		FString CategoryStr = Sequence.Category.Name.ToString();
		FFormatNamedArguments Arguments;
		Arguments.Add(TEXT("sequence"), Sequence.ButtonSequence);
		Arguments.Add(TEXT("category"), FText::FromString(CategoryStr));
		Desc += "\n\n----------------------\n";
		Desc += FText::Format(
				LOCTEXT("TaskButtonSequenceSequenceTitle", "[ Sequence {sequence} :: {category} ]"),
				Arguments
			)
			.ToString();
		Desc += "\n";
		Desc += FText::Format(
				LOCTEXT("TaskButtonSequenceSequenceDetails", "lvl: {0}, Position: {1}, Direction: {2}"),
				Sequence.LevelCoefficient,
				i,
				FText::FromString(ENUM_TO_STRING(EResponseDirection, Sequence.Direction))
			)
			.ToString();
		Desc += "\n\n";
		Desc += LOCTEXT("TaskButtonSequenceSequenceResponseTitle", "Responses for points:").ToString();

		Arguments.Empty();
		FString Text;

		for (FBTButtonSequenceResponse Response : Sequence.GetResponsesForPoint())
		{
			Response.Category = Sequence.Category;
			FFormatNamedArguments RespArguments;
			RespArguments.Add(TEXT("response"), Response.Response);
			RespArguments.Add(TEXT("point"), Response.ForPoint);
			Text =
				FText::Format(
					LOCTEXT("TaskButtonSequenceSequenceResponse", "[{point}] \"{response}\""),
					RespArguments
				).ToString();
			Desc += "\n" + UNTextLibrary::StringToLines(Text, 60, "\t");

			FText EffectName = IsValid(Response.GetSpawnableEffectOnEarned())
								   ? FText::FromString(Response.GetSpawnableEffectOnEarned()->GetName())
								   : FText::FromString(TEXT("No effect"));
			RespArguments.Reset();
			RespArguments.Add(TEXT("effect"), EffectName);
			Text =
				FText::Format(
					LOCTEXT("TaskButtonSequenceSequenceResponse", "Effect: {effect}\n"),
					RespArguments
				).ToString();
			Desc += "\n" + UNTextLibrary::StringToLines(Text, 60, "\t");
		}

		Desc += "\n\nPossibilities:";
		Desc += ShowPermutationsPoints(Sequence);
		Desc += "\n----------------------";
		i++;
	}

	Desc += "\n----------------------\n";
	Desc += CountDownTask->GetStaticDescription();
	Desc += "\n----------------------\n";

	return Desc;
}

FString UBTTask_ButtonsSequence::ShowPermutationsPoints(const FBTButtonSequence& Sequence) const
{
	FString StaticDesc = StaticButtonSequenceDescriptions::GetDescription(Sequence);
	if (!StaticDesc.IsEmpty()) return StaticDesc;

	const FString SequenceStr = Sequence.ButtonSequence.ToString();
	TArray<FPermutationValue<TCHAR>> CharArr = UNansArrayUtils::StringToPermutationArray(SequenceStr);
	TMap<int32, FPermutationChances> Chances;

	do
	{
		FString str;
		for (FPermutationValue<TCHAR> val : CharArr)
		{
			str.AppendChar(val.Value);
		}
		int32 Points = PointsComparedToSequence(str, Sequence);
		FPermutationChances& Chance = Chances.FindOrAdd(Points);
		Chance.TotalChances++;
		Chance.Possibility = str;
	}
	while (UNansArrayUtils::NextPermutation(CharArr));

	Chances.KeySort(
		[](int32 A, int32 B)
		{
			return A < B; // sort keys in reverse
		}
	);
	FString Desc = "";
	for (TPair<int32, FPermutationChances> Chance : Chances)
	{
		FFormatNamedArguments RespArguments;
		RespArguments.Add(TEXT("possibility"), FText::FromString(Chance.Value.Possibility));
		RespArguments.Add(TEXT("totalChances"), Chance.Value.TotalChances);
		RespArguments.Add(TEXT("point"), Chance.Key);
		Desc += "\n\t* " + FText::Format(
				LOCTEXT(
					"TaskButtonSequenceSequencePermutPossibilities",
					"{point}: TotalChances = {totalChances} eg. \"{possibility}\""
				),
				RespArguments
			)
			.ToString();
	}
	StaticButtonSequenceDescriptions::SetDescription(Sequence, Desc);
	return Desc;
}

#undef LOCTEXT_NAMESPACE
