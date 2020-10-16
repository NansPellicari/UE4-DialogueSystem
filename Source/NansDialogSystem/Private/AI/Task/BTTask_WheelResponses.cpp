// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Task/BTTask_WheelResponses.h"

#include "BTDialogueResponseContainer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NansExtension/Public/UMG/Blueprint/NansUserWidget.h"
#include "NansUE4Utilities/public/Misc/ErrorUtils.h"
#include "NansUE4Utilities/public/Misc/TextLibrary.h"
#include "Runtime/UMG/Public/Components/PanelWidget.h"
#include "Service/BTDialogDifficultyHandler.h"
#include "UI/DialogHUD.h"
#include "UI/ResponseButtonWidget.h"
#include "UI/WheelButtonWidget.h"

#define LOCTEXT_NAMESPACE "DialogSystem"

FString UBTTask_WheelResponses::GetStaticDescription() const
{
	FString ReturnDesc = Super::GetStaticDescription();
	ReturnDesc += "\nResponses Type Tolerance: " + FString::SanitizeFloat(ResponsesTypeTolerance);

	return ReturnDesc;
}

void UBTTask_WheelResponses::ReceiveOnTick(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (!ensure(DialogHUD != nullptr))
	{
		EDITOR_ERROR("DialogSystem", LOCTEXT("WrongHUDType", "The HUD set is not valid (UDialogHUD is expected) in "));
		FinishLatentTask(OwnerComp, EBTNodeResult::Aborted);
		return;
	}

	// TODO make wheel name configurable?
	UWheelButtonWidget* WheelButton = Cast<UWheelButtonWidget>(DialogHUD->FindWidget(FName("WheelButton")));

	if (!ensure(WheelButton != nullptr))
	{
		EDITOR_ERROR("DialogSystem",
			LOCTEXT("WrongWheelButtonNameOrInexistant", "Check the wheel button name or if exists in the Dialog HUD in "),
			this);
		FinishLatentTask(OwnerComp, EBTNodeResult::Aborted);
		return;
	}

	WheelButton->GetWheelDistRatio(WheelRatioClockwise, WheelRatioAntiClockwise);

	float ClockwiseWheelReach = FMath::Clamp<float>(WheelRatioClockwise - WheelRatioAntiClockwise, ResponsesTypeTolerance, 100);
	float CounterClockwiseWheelReach =
		FMath::Clamp<float>(WheelRatioAntiClockwise - WheelRatioClockwise, ResponsesTypeTolerance, 100);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	UBTDialogDifficultyHandler* DifficultyHandler =
		Cast<UBTDialogDifficultyHandler>(BlackboardComp->GetValueAsObject(DifficultyHandlerKeyName));

	if (!ensure(DifficultyHandler != nullptr))
	{
		EDITOR_ERROR("DialogSystem",
			LOCTEXT("DifficultyHandlerMissingOnWheelResponses", "The difficulty handler is missing on WheelResponses"),
			this);
		FinishLatentTask(OwnerComp, EBTNodeResult::Aborted);
		return;
	}

	for (TPair<FString, int32> ResponseIndex : ListButtonIndexes)
	{
		UResponseButtonWidget* Button = Cast<UResponseButtonWidget>(ResponsesSlot->GetChildAt(ResponseIndex.Value));
		if (!ensure(Button != nullptr))
		{
			EDITOR_ERROR("DialogSystem", LOCTEXT("WrongWidgetInResponsesSlot", "A Wrong object type is in the responses slot in "));
			FinishLatentTask(OwnerComp, EBTNodeResult::Aborted);
			return;
		}
		Button->SetVisibility(ESlateVisibility::Hidden);
		UBTDialogueResponseContainer* ResponseContainer = Button->GetResponse();
		FBTDialogueResponse Response = ResponseContainer->GetResponse();

		bool UP = (ResponseContainer->InfluencedBy == EResponseDirection::UP &&
				   DifficultyHandler->GetDifficultyLevel(Response) <= ClockwiseWheelReach);
		bool DOWN = (ResponseContainer->InfluencedBy == EResponseDirection::DOWN &&
					 DifficultyHandler->GetDifficultyLevel(Response) <= CounterClockwiseWheelReach);

		if (UP || DOWN || ResponseContainer->InfluencedBy == EResponseDirection::NONE)
		{
			Button->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

#if WITH_EDITOR
FName UBTTask_WheelResponses::GetNodeIconName() const
{
	// TODO import my own icon
	return FName("BTEditor.Graph.BTNode.Task.PlaySound.Icon");
}
#endif	  // WITH_EDITOR

#undef LOCTEXT_NAMESPACE
