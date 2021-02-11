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

#include "AI/Service/BTService_DialogPointsHandler.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "NansBehaviorSteps/Public/BTStepsHandler.h"
#include "NansUE4Utilities/public/Misc/ErrorUtils.h"
#include "NansUE4Utilities/public/Misc/TextLibrary.h"
#include "Service/BTDialogPointsHandler.h"
#include "Service/DialogBTHelpers.h"
#include "Service/InteractiveBTHelpers.h"

#define LOCTEXT_NAMESPACE "DialogSystem"

UBTService_DialogPointsHandler::UBTService_DialogPointsHandler(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = "Dialog Points Handler";

	bNotifyTick = false;
	bTickIntervals = true;
	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = false;
}

void UBTService_DialogPointsHandler::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	BTDialogPointsHandler = Cast<UBTDialogPointsHandler>(BlackboardComp->GetValueAsObject(PointsHandlerKeyName));

	if (BTDialogPointsHandler == nullptr)
	{
		BTDialogPointsHandler = NewObject<UBTDialogPointsHandler>(&OwnerComp);
		BlackboardComp->SetValueAsObject(PointsHandlerKeyName, BTDialogPointsHandler);
	}

	UObject* BTSteps = BlackboardComp->GetValueAsObject(StepsKeyName);
	if (BTSteps != nullptr && BTDialogPointsHandler != nullptr)
	{
		if (!BTSteps->Implements<UBTStepsHandler>())
		{
			EDITOR_ERROR(
				"DialogSystem",
				LOCTEXT("InvalidStepsHandlerClass", "Invalid class for Steps, should implements IBTStepsHandler")
			);
			return;
		}
		TScriptInterface<IBTStepsHandler> BTStepsHandler;
		BTStepsHandler.SetObject(BTSteps);
		BTStepsHandler.SetInterface(Cast<IBTStepsHandler>(BTSteps));
		BTDialogPointsHandler->Initialize(
			BTStepsHandler,
			OwnerComp,
			ActorOwner->GetPathName(),
			NDialogBTHelpers::GetABS(OwnerComp, BlackboardComp)
		);

		// TODO create a decorator for this
		// if (!bCanDialogue)
		// {
		// 	OwnerComp.StopLogic(FString("Player can't speak"));
		// }
	}
}

#if WITH_EDITOR
FName UBTService_DialogPointsHandler::GetNodeIconName() const
{
	// TODO import my own icon
	return FName("BTEditor.Graph.BTNode.Task.PlaySound.Icon");
}
#endif	  // WITH_EDITOR

FString UBTService_DialogPointsHandler::GetStaticDescription() const
{
	FString ReturnDesc;
	ReturnDesc += "Points Handler Key: " + PointsHandlerKeyName.ToString();

	return ReturnDesc;
}
#undef LOCTEXT_NAMESPACE
