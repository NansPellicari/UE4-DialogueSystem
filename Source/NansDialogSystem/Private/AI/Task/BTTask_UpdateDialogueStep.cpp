//  Copyright 2020-present Nans Pellicari (nans.pellicari@gmail.com).
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

#include "AI/Task/BTTask_UpdateDialogueStep.h"

#include "BTDialogueResponseContainer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NansBehaviorSteps/Public/BTStepsHandler.h"
#include "NansUE4Utilities/public/Misc/ErrorUtils.h"
#include "Service/BTDialogPointsHandler.h"
#include "UI/ResponseButtonWidget.h"

#define LOCTEXT_NAMESPACE "DialogSystem"

EBTNodeResult::Type UBTTask_UpdateDialogueStep::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	UObject* BTSteps = BlackboardComp->GetValueAsObject(StepsKeyName);
	UBTDialogPointsHandler* PointsHandler = Cast<UBTDialogPointsHandler>(
		BlackboardComp->GetValueAsObject(PointsHandlerKeyName)
	);

	if (BTSteps == nullptr)
	{
		EDITOR_ERROR("DialogSystem", LOCTEXT("InvalidStepsKey", "Invalid key for Steps in "));
		return EBTNodeResult::Aborted;
	}

	if (!BTSteps->Implements<UBTStepsHandler>())
	{
		EDITOR_ERROR(
			"DialogSystem",
			LOCTEXT("InvalidStepsHandlerClass", "Invalid class for Steps, should implements IBTStepsHandler")
		);
		return EBTNodeResult::Aborted;
	}

	ResponseContainer = Cast<UBTDialogueResponseContainer>(BlackboardComp->GetValueAsObject(ResponseContainerName));

	FNPoint Point;
	int32 Position = 0;
	Point.Point = 0;

	if (ResponseContainer != nullptr)
	{
		FBTDialogueResponse Response = ResponseContainer->GetResponse();
		Point.EffectOnEarned = Response.GetSpawnableEffectOnEarned();
		Point.Difficulty = Response.DifficultyLevel;
		Point.Point = Response.Level;
		Point.Category = Response.Category;
		Position = ResponseContainer->DisplayOrder;
		ResponseContainer->MarkPendingKill();
	}

	PointsHandler->AddPoints(Point, Position);
	IBTStepsHandler::Execute_FinishedCurrentStep(BTSteps);

	BlackboardComp->ClearValue(ResponseContainerName);

	return EBTNodeResult::Succeeded;
}

#if WITH_EDITOR
FName UBTTask_UpdateDialogueStep::GetNodeIconName() const
{
	// TODO import my own icon
	return FName("BTEditor.Graph.BTNode.Task.PlaySound.Icon");
}
#endif	  // WITH_EDITOR

FString UBTTask_UpdateDialogueStep::GetStaticDescription() const
{
	FString ReturnDesc;
	ReturnDesc += "PointsHandler key: " + PointsHandlerKeyName.ToString();
	ReturnDesc += "\nSteps key: " + StepsKeyName.ToString();
	ReturnDesc += "\nResponseContainer Key: " + ResponseContainerName.ToString();

	return ReturnDesc;
}
#undef LOCTEXT_NAMESPACE
