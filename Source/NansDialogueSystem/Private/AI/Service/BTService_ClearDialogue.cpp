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

#include "AI/Service/BTService_ClearDialogue.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Service/BTDialogueDifficultyHandler.h"
#include "Service/BTDialoguePointsHandler.h"
#include "Service/DialogueBTHelpers.h"
#include "Setting/DialogueSystemSettings.h"

#define LOCTEXT_NAMESPACE "DialogueSystem"

UBTService_ClearDialogue::UBTService_ClearDialogue(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = "Clear Dialog";

	bNotifyTick = false;
	bTickIntervals = false;
	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = false;
}

void UBTService_ClearDialogue::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	const auto Settings = UDialogueSystemSettings::Get()->BehaviorTreeSettings;

	UBTDialoguePointsHandler* BTDialoguePointsHandler =
		Cast<UBTDialoguePointsHandler>(BlackboardComp->GetValueAsObject(Settings.PointsHandlerKey));
	UBTDialogueDifficultyHandler* BTDialogDifficultyHandler =
		Cast<UBTDialogueDifficultyHandler>(BlackboardComp->GetValueAsObject(Settings.DifficultyHandlerKey));

	if (IsValid(BTDialoguePointsHandler))
	{
		BTDialoguePointsHandler->Clear();
	}
	if (IsValid(BTDialogDifficultyHandler))
	{
		BTDialogDifficultyHandler->Clear();
	}
	NDialogueBTHelpers::RemoveUIFromBlackboard(OwnerComp, BlackboardComp);
}

#if WITH_EDITOR
FName UBTService_ClearDialogue::GetNodeIconName() const
{
	// TODO import my own icon
	return FName("BTEditor.Graph.BTNode.Task.PlaySound.Icon");
}
#endif	  // WITH_EDITOR

FString UBTService_ClearDialogue::GetStaticDescription() const
{
	FString ReturnDesc;

	return ReturnDesc;
}
#undef LOCTEXT_NAMESPACE
