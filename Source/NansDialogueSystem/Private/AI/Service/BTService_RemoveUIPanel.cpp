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

#include "AI/Service/BTService_RemoveUIPanel.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NansUE4Utilities/public/Misc/ErrorUtils.h"
#include "Service/DialogueBTHelpers.h"

#define LOCTEXT_NAMESPACE "DialogSystem"

UBTService_RemoveUIPanel::UBTService_RemoveUIPanel(const FObjectInitializer& ObjectInitializer) : Super(
	ObjectInitializer
)
{
	NodeName = "Remove UI Panel";

	bNotifyTick = false;
	bTickIntervals = false;
	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = false;
}

void UBTService_RemoveUIPanel::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	NDialogueBTHelpers::RemoveUIFromBlackboard(OwnerComp, Blackboard);
}

#undef LOCTEXT_NAMESPACE
