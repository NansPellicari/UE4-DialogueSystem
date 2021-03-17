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

#include "AI/Task/BTTask_WheelResponses.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "NansUE4Utilities/public/Misc/ErrorUtils.h"
#include "Service/BTDialogueDifficultyHandler.h"
#include "UI/DialogueUI.h"
#include "UI/ResponseButtonWidget.h"
#include "UI/WheelButtonWidget.h"

#define LOCTEXT_NAMESPACE "DialogueSystem"

UBTTask_WheelResponses::UBTTask_WheelResponses(const FObjectInitializer& ObjectInitializer) : UBTTask_Responses(
	ObjectInitializer
)
{
	NodeName = "Wheel Responses";
}

FString UBTTask_WheelResponses::GetStaticDescription() const
{
	FString ReturnDesc = "\nResponses Type Tolerance: " + FString::SanitizeFloat(ResponsesTypeTolerance);
	ReturnDesc += Super::GetStaticDescription();

	return ReturnDesc;
}

void UBTTask_WheelResponses::ReceiveOnTick(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (!IsValid(DialogueUI))
	{
		EDITOR_ERROR("DialogueSystem", LOCTEXT("WrongUIType", "The UI set is not valid (UDialogueUI is expected) in "));
		FinishLatentTask(OwnerComp, EBTNodeResult::Aborted);
		return;
	}

	UWheelButtonWidget* WheelButton = DialogueUI->GetWheelButton();

	if (!IsValid(WheelButton))
	{
		EDITOR_ERROR(
			"DialogueSystem",
			LOCTEXT("WrongWheelButtonNameOrInexistant", "Check the wheel button name or if exists in the Dialogue UI in "
			),
			this
		);
		FinishLatentTask(OwnerComp, EBTNodeResult::Aborted);
		return;
	}

	WheelButton->GetWheelDistRatio(WheelRatioClockwise, WheelRatioAntiClockwise);

	const float ClockwiseWheelReach = FMath::Clamp<float>(
		WheelRatioClockwise - WheelRatioAntiClockwise,
		ResponsesTypeTolerance,
		100
	);
	const float CounterClockwiseWheelReach =
		FMath::Clamp<float>(WheelRatioAntiClockwise - WheelRatioClockwise, ResponsesTypeTolerance, 100);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	UBTDialogueDifficultyHandler* DifficultyHandler =
		Cast<UBTDialogueDifficultyHandler>(BlackboardComp->GetValueAsObject(DifficultyHandlerKeyName));

	if (!ensure(DifficultyHandler != nullptr))
	{
		EDITOR_ERROR(
			"DialogueSystem",
			LOCTEXT("DifficultyHandlerMissingOnWheelResponses", "The difficulty handler is missing on WheelResponses"),
			this
		);
		FinishLatentTask(OwnerComp, EBTNodeResult::Aborted);
		return;
	}

	for (const auto& Button : DialogueUI->GetResponsesButttons())
	{
		Button->SetVisibility(ESlateVisibility::Hidden);
		FBTDialogueResponse Response = Button->GetResponse();

		const bool UP = (Button->InfluencedBy == EResponseDirection::UP &&
						 DifficultyHandler->GetDifficultyLevel(Response) <= ClockwiseWheelReach);
		const bool DOWN = (Button->InfluencedBy == EResponseDirection::DOWN &&
						   DifficultyHandler->GetDifficultyLevel(Response) <= CounterClockwiseWheelReach);

		if (UP || DOWN || Button->InfluencedBy == EResponseDirection::NONE)
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
