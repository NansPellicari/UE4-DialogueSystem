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

#include "AI/Service/BTService_AddPointsAndResponses.h"

#include "BTDialogueResponseContainer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NansUE4Utilities/public/Misc/ErrorUtils.h"
#include "NansUE4Utilities/public/Misc/TextLibrary.h"
#include "Service/BTDialogPointsHandler.h"
#include "UI/ResponseButtonWidget.h"

#define LOCTEXT_NAMESPACE "DialogSystem"

UBTService_AddPointsAndResponses::UBTService_AddPointsAndResponses(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = "Add Points and Responses";

	bNotifyTick = false;
	bTickIntervals = false;
	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = false;
}

void UBTService_AddPointsAndResponses::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	PointsHandler = Cast<UBTDialogPointsHandler>(BlackboardComp->GetValueAsObject(PointsHandlerKeyName));

	if (PointsHandler == nullptr)
	{
		EDITOR_ERROR("DialogSystem", LOCTEXT("InvalidPointsHandlerKey", "Invalid key for points handler in "));
		return;
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

	BlackboardComp->ClearValue(ResponseContainerName);
}

#if WITH_EDITOR
FName UBTService_AddPointsAndResponses::GetNodeIconName() const
{
	// TODO import my own icon
	return FName("BTEditor.Graph.BTNode.Task.PlaySound.Icon");
}
#endif	  // WITH_EDITOR

FString UBTService_AddPointsAndResponses::GetStaticDescription() const
{
	FString ReturnDesc;
	ReturnDesc += "PointsHandler key: " + PointsHandlerKeyName.ToString();
	ReturnDesc += "\nResponseContainer ey: " + ResponseContainerName.ToString();

	return ReturnDesc;
}
#undef LOCTEXT_NAMESPACE
