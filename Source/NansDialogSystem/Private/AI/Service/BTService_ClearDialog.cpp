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

#include "AI/Service/BTService_ClearDialog.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Service/BTDialogPointsHandler.h"

#define LOCTEXT_NAMESPACE "DialogSystem"

UBTService_ClearDialog::UBTService_ClearDialog(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = "Clear Dialog";

	bNotifyTick = false;
	bTickIntervals = false;
	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = false;
}

void UBTService_ClearDialog::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	UBTDialogPointsHandler* BTDialogPointsHandler =
		Cast<UBTDialogPointsHandler>(BlackboardComp->GetValueAsObject(PointsHandlerKeyName));

	if (BTDialogPointsHandler == nullptr)
	{
		return;
	}

	BTDialogPointsHandler->Clear();
}

#if WITH_EDITOR
FName UBTService_ClearDialog::GetNodeIconName() const
{
	// TODO import my own icon
	return FName("BTEditor.Graph.BTNode.Task.PlaySound.Icon");
}
#endif	  // WITH_EDITOR

FString UBTService_ClearDialog::GetStaticDescription() const
{
	FString ReturnDesc;
	ReturnDesc += "Points Handler Key: " + PointsHandlerKeyName.ToString();

	return ReturnDesc;
}
#undef LOCTEXT_NAMESPACE
