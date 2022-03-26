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

#include "AI/Task/BTTask_RemoveUIPanel.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Misc/ErrorUtils.h"
#include "Service/DialogueBTHelpers.h"
#include "Setting/DialogueSystemSettings.h"

#define LOCTEXT_NAMESPACE "DialogueSystem"

UBTTask_RemoveUIPanel::UBTTask_RemoveUIPanel(const FObjectInitializer& ObjectInitializer)
	: UBTTaskNode(ObjectInitializer)
{
	NodeName = "Remove UI Panel";
}

EBTNodeResult::Type UBTTask_RemoveUIPanel::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	NDialogueBTHelpers::RemoveUIFromBlackboard(OwnerComp, Blackboard);
	return EBTNodeResult::Type::Succeeded;
}

FString UBTTask_RemoveUIPanel::GetStaticDescription() const
{
	FString ReturnDesc = Super::GetStaticDescription();
	const auto Settings = UDialogueSystemSettings::Get()->BehaviorTreeSettings;
	const FName UINameKey = Settings.UINameKey;
	const FName PreviousUINameKey = Settings.PreviousUINameKey;
	const FName PreviousUIClassKey = Settings.PreviousUIClassKey;

	ReturnDesc += ":\nUINameKey: " + UINameKey.ToString();
	ReturnDesc += "\nPreviousUINameKey: " + PreviousUINameKey.ToString();
	ReturnDesc += "\nPreviousUIClassKey: " + PreviousUIClassKey.ToString();

	return ReturnDesc;
}

#if WITH_EDITOR
FName UBTTask_RemoveUIPanel::GetNodeIconName() const
{
	// TODO import my own icon
	return FName("BTEditor.Graph.BTNode.Task.Wait.Icon");
}
#endif	  // WITH_EDITOR

#undef LOCTEXT_NAMESPACE
