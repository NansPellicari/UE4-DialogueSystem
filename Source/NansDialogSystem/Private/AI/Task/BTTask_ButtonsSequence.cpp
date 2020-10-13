// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Task/BTTask_ButtonsSequence.h"

#include "AI/Task/BTTask_Countdown.h"
#include "BTDialogueResponseContainer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/Button.h"
#include "Components/CanvasPanelSlot.h"
#include "NansUE4Utilities/public/Misc/ErrorUtils.h"
#include "NansUE4Utilities/public/Misc/TextLibrary.h"
#include "Service/ButtonSequenceMovementManager.h"
#include "Service/NansArrayUtils.h"
#include "UI/ButtonSequenceWidget.h"
#include "UI/DialogHUD.h"
#include "UI/WheelButtonWidget.h"
#include "UI/WheelProgressBarWidget.h"

#define LOCTEXT_NAMESPACE "DialogSystem"

UBTTask_ButtonsSequence::UBTTask_ButtonsSequence(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bNotifyTick = true;
	bNotifyTaskFinished = true;

	BTSequenceManager = MakeShareable(new NButtonSequenceMovementManager());
	CountDownTask = ObjectInitializer.CreateDefaultSubobject<UBTTask_Countdown>(this, TEXT("Countdown"));
	CountDownTask->OnCountdownEnds().AddUObject(this, &UBTTask_ButtonsSequence::OnCountdownEnds);

	SequenceToText.Add(EButtonsSequence::OSBD, LOCTEXT("CNVSequenceOSBD", "OSBD"));
	SequenceToText.Add(EButtonsSequence::IAJA, LOCTEXT("CSVSequenceIAJA", "IAJA"));
	SequenceToText.Add(EButtonsSequence::Neutral, LOCTEXT("CSVSequenceNEUTRAL", "N"));
	SequenceAlignment.Add(EButtonsSequence::OSBD, EAlignment::CNV);
	SequenceAlignment.Add(EButtonsSequence::IAJA, EAlignment::CSV);
	SequenceAlignment.Add(EButtonsSequence::Neutral, EAlignment::Neutral);
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
	DialogHUD = Cast<UDialogHUD>(Blackboard->GetValueAsObject(HUD));

	if (!ensure(DialogHUD != nullptr))
	{
		EDITOR_ERROR("DialogSystem", LOCTEXT("WrongHUDType", "The HUD set is not valid (UDialogHUD is expected) in "));
		return EBTNodeResult::Aborted;
	}

	DialogHUD->OnEndDisplayResponse.AddDynamic(this, &UBTTask_ButtonsSequence::OnEndDisplayResponse);

	ButtonsSlot = Cast<UPanelWidget>(DialogHUD->FindWidget(ButtonsSlotName));

	if (!ensure(ButtonsSlot != nullptr))
	{
		EDITOR_ERROR("DialogSystem", LOCTEXT("WrongButtonsSlotName", "The ButtonsSlotName set doesn't exists in HUD in "));
		return EBTNodeResult::Aborted;
	}

	if (!ensure(ButtonWidget != NULL && !ButtonWidget->GetClass()->GetFullName().IsEmpty()))
	{
		EDITOR_ERROR("DialogSystem", LOCTEXT("WrongButtonWidget", "Need to set a ButtonWidget in "));
		return EBTNodeResult::Aborted;
	}

	// TODO make wheel name configurable?
	UWheelButtonWidget* WheelButton = Cast<UWheelButtonWidget>(DialogHUD->FindWidget(FName("WheelButton")));

	BTSequenceManager->Initialize(ButtonsSlot, DefaultVelocity, WheelButton);
	CreateButtons();

	UWheelProgressBarWidget* TimeWidget = Cast<UWheelProgressBarWidget>(DialogHUD->FindWidget("ProgressBar"));
	if (TimeWidget != nullptr)
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

void UBTTask_ButtonsSequence::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	CountDownTask->OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
	PlayerTries.Empty();
	TriesStatus = EBTNodeResult::InProgress;
	SequenceIndex = -1;
	BTSequenceManager->Reset();
	DialogHUD->OnEndDisplayResponse.RemoveAll(this);
}

void UBTTask_ButtonsSequence::RemoveButtons(UBehaviorTreeComponent& OwnerComp)
{
	while (ButtonsSlot->GetChildrenCount() > 0)
	{
		UUserWidget* Widget = Cast<UUserWidget>(ButtonsSlot->GetChildAt(0));

		// Should be a Weiiiird behavior, but just in case
		if (!ensure(Widget != nullptr))
		{
			EDITOR_ERROR("DialogSystem", LOCTEXT("WrongWidgetInResponsesSlot", "A Wrong object type is in the responses slot in "));
			FinishLatentTask(OwnerComp, EBTNodeResult::Aborted);
			return;
		}
		UButtonSequenceWidget* Button = Cast<UButtonSequenceWidget>(Widget);

		if (Button != nullptr)
		{
			Button->OnBTClicked.Clear();
			DialogHUD->OnRemovedChild.Broadcast(Button);
			Button->MarkPendingKill();
		}

		ButtonsSlot->RemoveChildAt(0);
	}
}

void UBTTask_ButtonsSequence::OnCountdownEnds(UBehaviorTreeComponent* OwnerComp)
{
	Blackboard->SetValueAsObject(ResponseContainerKey, UBTDialogueResponseContainer::CreateNullObject(OwnerComp));
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

	for (FBTButtonSequence Sequence : Sequences)
	{
		FString StringSequence = SequenceToText.FindRef(Sequence.ButtonSequence).ToString();
		EAlignment Alignment = SequenceAlignment.FindRef(Sequence.ButtonSequence);
		bool ColorIsComputed = false;
		FLinearColor FinalColor;
		for (TCHAR Char : StringSequence)
		{
			UButtonSequenceWidget* Button =
				CreateWidget<UButtonSequenceWidget>(GetWorld(), ButtonWidget->GetDefaultObject()->GetClass());
			Button->SetText(FString("") + Char);
			Button->OnBTClicked.AddDynamic(this, &UBTTask_ButtonsSequence::OnButtonClick);
			Button->Alignment = Alignment;

			if (ColorIsComputed == false)
			{
				if (Alignment == EAlignment::CNV)
				{
					FinalColor = Button->ColorCNV;
				}
				else if (Alignment == EAlignment::CSV)
				{
					FinalColor = Button->ColorCSV;
				}
				else
				{
					FinalColor = Button->ColorNeutral;
				}
				ColorIsComputed = true;
			}

			Button->FinalColor = FinalColor;
			ButtonsSlot->AddChild(Button);
			DialogHUD->OnAddedChild.Broadcast(Button);
			UCanvasPanelSlot* Slot = Cast<UCanvasPanelSlot>(Button->Slot);
			if (Slot == nullptr) continue;
			Slot->SetSize(Button->ButtonDisplaySize);
		}
	}

	BTSequenceManager->PlaceButtons();
}

int32 UBTTask_ButtonsSequence::GetEarnedPoint()
{
	if (SequenceIndex < 0 || SequenceIndex >= Sequences.Num())
	{
		EDITOR_ERROR("DialogSystem", LOCTEXT("NoSequenceHasBeenChosen", "A sequence is required to get the earned point in "));
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

const int32 UBTTask_ButtonsSequence::PointsComparedToSequence(const FString& Tries, const FBTButtonSequence& Sequence) const
{
	FString StringSequence = SequenceToText.FindRef(Sequence.ButtonSequence).ToString();

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
	return FMath::FloorToInt((found / (float) Tries.Len()) * Sequence.LevelCoefficient);
}

FString UBTTask_ButtonsSequence::GetStaticDescription() const
{
	FString Desc = Super::GetStaticDescription();
	int32 i = 0;
	for (FBTButtonSequence Sequence : Sequences)
	{
		Desc += "\n\n----------------------\n";
		Desc += FText::Format(
			LOCTEXT("TaskButtonSequenceSequenceTitle", "[ Sequence {0} ]"), SequenceToText.FindRef(Sequence.ButtonSequence))
					.ToString();
		Desc += "\n";
		Desc +=
			FText::Format(LOCTEXT("TaskButtonSequenceSequenceDetails", "lvl: {0}, Position: {1}\n"), Sequence.LevelCoefficient, i)
				.ToString();
		Desc += LOCTEXT("TaskButtonSequenceSequenceResponseTitle", "\nResponses for points:").ToString();
		FFormatNamedArguments Arguments;
		Arguments.Add(TEXT("default"), Sequence.DefaultResponse);
		FString Text =
			FText::Format(LOCTEXT("TaskButtonSequenceSequenceDefaultResponse", "[-1] \"{default}\""), Arguments).ToString();
		Desc += "\n" + UNTextLibrary::StringToLines(Text, 60, "\t");

		for (FBTButtonSequenceResponse Response : Sequence.ResponsesForPoint)
		{
			FFormatNamedArguments RespArguments;
			RespArguments.Add(TEXT("response"), Response.Response);
			RespArguments.Add(TEXT("point"), Response.ForPoint);
			Text =
				FText::Format(LOCTEXT("TaskButtonSequenceSequenceResponse", "[{point}] \"{response}\""), RespArguments).ToString();
			Desc += "\n" + UNTextLibrary::StringToLines(Text, 60, "\t");
		}

		// TODO show possible permutations to test points.
		Desc += "\n\nPossibilities:";
		Desc += ShowPermutationsPoints(Sequence);
		Desc += "\n----------------------";
		i++;
	}

	Desc += "\nHUD: " + HUD.ToString();
	Desc += "\nResponse Container: " + ResponseContainerKey.ToString();

	Desc += "\nResponse Button Widget: ";
	if (ButtonWidget != NULL && !ButtonWidget->GetDefaultObject()->GetClass()->GetFullName().IsEmpty())
	{
		Desc += ButtonWidget->GetDefaultObject()->GetClass()->GetName();
	}
	else
	{
		Desc += "None";
	}

	Desc += "\nResponses Slot Name: " + ButtonsSlotName.ToString();
	Desc += "\nDefault velocity: " + FString::FromInt(DefaultVelocity);

	Desc += "\n----------------------\n";
	Desc += CountDownTask->GetStaticDescription();
	Desc += "\n----------------------\n";

	return Desc;
}

FString UBTTask_ButtonsSequence::ShowPermutationsPoints(const FBTButtonSequence& Sequence) const
{
	FString StaticDesc = StaticButtonSequenceDescriptions::GetDescription(Sequence);
	if (!StaticDesc.IsEmpty()) return StaticDesc;

	const FString SequenceStr = SequenceToText.FindRef(Sequence.ButtonSequence).ToString();
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
	} while (UNansArrayUtils::NextPermutation(CharArr));

	Chances.KeySort([](int32 A, int32 B) {
		return A < B;	 // sort keys in reverse
	});
	FString Desc = "";
	for (TPair<int32, FPermutationChances> Chance : Chances)
	{
		FFormatNamedArguments RespArguments;
		RespArguments.Add(TEXT("possibility"), FText::FromString(Chance.Value.Possibility));
		RespArguments.Add(TEXT("totalChances"), Chance.Value.TotalChances);
		RespArguments.Add(TEXT("point"), Chance.Key);
		Desc += "\n\t* " + FText::Format(LOCTEXT("TaskButtonSequenceSequencePermutPossibilities",
											 "{point}: TotalChances = {totalChances} eg. \"{possibility}\""),
							   RespArguments)
							   .ToString();
	}
	StaticButtonSequenceDescriptions::SetDescription(Sequence, Desc);
	return Desc;
}

void UBTTask_ButtonsSequence::OnButtonClick(UButtonSequenceWidget* Button)
{
	PlayerTries += Button->GetText();

	for (int32 Index = 0; Index != Sequences.Num(); ++Index)
	{
		FBTButtonSequence Seq = Sequences[Index];
		FString StringSequence = SequenceToText.FindRef(Seq.ButtonSequence).ToString();
		if (StringSequence.Len() == PlayerTries.Len() && UNTextLibrary::ArePermutations(StringSequence, PlayerTries))
		{
			Button->SetVisibility(ESlateVisibility::Collapsed);
			TriesStatus = EBTNodeResult::Succeeded;
			SequenceIndex = Index;
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
			EDITOR_ERROR("DialogSystem", LOCTEXT("NoSequenceHasBeenChosen", "A sequence is required to get the earned point in "));
			FinishLatentTask(*OwnerComponent, EBTNodeResult::Aborted);
			return;
		}
		int32 TotalPoint = GetEarnedPoint();
		FBTButtonSequence Sequence = Sequences[SequenceIndex];
		FBTDialogueResponse Response;
		Response.Alignment = SequenceAlignment.FindRef(Sequence.ButtonSequence);
		Response.DisplayOrder = SequenceIndex;
		Response.Level = TotalPoint;
		Response.Text = Sequence.GetResponseForPoints(TotalPoint);

		UBTDialogueResponseContainer* ResponseContainer =
			NewObject<UBTDialogueResponseContainer>(OwnerComponent, UBTDialogueResponseContainer::StaticClass());
		ResponseContainer->SetResponse(Response);
		Blackboard->SetValueAsObject(ResponseContainerKey, ResponseContainer);

		DialogHUD->OnResponse.Broadcast(Response.Text);
		RemoveButtons(*OwnerComponent);
		return;
	}

	// here because the last letter disable the tried sequence
	// So restart the tries
	PlayerTries.Empty();
	SequenceIndex = -1;
	for (int32 i = 0; i < ButtonsSlot->GetChildrenCount(); ++i)
	{
		UButtonSequenceWidget* ButtonFromList = Cast<UButtonSequenceWidget>(ButtonsSlot->GetChildAt(i));
		if (ButtonFromList == nullptr) continue;	// only concerned about button
		ButtonFromList->SetVisibility(ESlateVisibility::Visible);
	}
}

#undef LOCTEXT_NAMESPACE
