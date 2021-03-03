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

#include "AI/Service/BTService_DialogDifficultyHandler.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "NansUE4Utilities/public/Misc/ErrorUtils.h"
#include "NansUE4Utilities/public/Misc/TextLibrary.h"
#include "Service/BTDialogDifficultyHandler.h"

#define LOCTEXT_NAMESPACE "DialogSystem"

UBTService_DialogDifficultyHandler::UBTService_DialogDifficultyHandler(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = "Dialog Difficulty Handler";

	bNotifyTick = false;
	bTickIntervals = true;
	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = false;
}

void UBTService_DialogDifficultyHandler::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	BTDialogDifficultyHandler = Cast<UBTDialogDifficultyHandler>(
		BlackboardComp->GetValueAsObject(DifficultyHandlerKeyName)
	);

	if (BTDialogDifficultyHandler == nullptr)
	{
		BTDialogDifficultyHandler = NewObject<UBTDialogDifficultyHandler>(&OwnerComp);
		BTDialogDifficultyHandler->Initialize(OwnerComp);
		BlackboardComp->SetValueAsObject(DifficultyHandlerKeyName, BTDialogDifficultyHandler);
	}
}

#if WITH_EDITOR
FName UBTService_DialogDifficultyHandler::GetNodeIconName() const
{
	// TODO import my own icon
	return FName("BTEditor.Graph.BTNode.Task.PlaySound.Icon");
}
#endif	  // WITH_EDITOR

FString UBTService_DialogDifficultyHandler::GetStaticDescription() const
{
	FString ReturnDesc;
	ReturnDesc += "Difficulty Handler Key: " + DifficultyHandlerKeyName.ToString();

	return ReturnDesc;
}
#undef LOCTEXT_NAMESPACE
