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

#include "AI/Task/BTTask_CreateUIPanel.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/Character.h"
#include "NansUE4Utilities/public/Misc/ErrorUtils.h"
#include "Service/DialogueBTHelpers.h"
#include "Setting/DialogueSystemSettings.h"

#define LOCTEXT_NAMESPACE "DialogueSystem"

UBTTask_CreateUIPanel::UBTTask_CreateUIPanel(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = "Create UI Panel";
	auto Settings = UDialogueSystemSettings::Get()->BehaviorTreeSettings;
	if (UIName == NAME_None)
	{
		UIName = Settings.DefaultUIName;
	}
	if (UINameKey == NAME_None)
	{
		UINameKey = Settings.UINameKey;
	}
	if (PreviousUINameKey == NAME_None)
	{
		PreviousUINameKey = Settings.PreviousUINameKey;
	}
	if (PreviousUIClassKey == NAME_None)
	{
		PreviousUIClassKey = Settings.PreviousUIClassKey;
	}
}

EBTNodeResult::Type UBTTask_CreateUIPanel::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

	if (UIToCreate == nullptr || UIToCreate->GetDefaultObject()->GetClass()->GetFullName().IsEmpty())
	{
		EDITOR_ERROR(
			"DialogueSystem",
			LOCTEXT("NotSetUIToCreate", "Set a UI class (inherited from UNansUserWidget) in ")
		);
		return EBTNodeResult::Type::Aborted;
	}

	const auto PlayerHUD = NDialogueBTHelpers::GetPlayerHUD(OwnerComp, FString(__FUNCTION__));
	if (!IsValid(PlayerHUD.GetObject()))
	{
		return EBTNodeResult::Type::Aborted;
	}

	// Same widget already exists
	if (IDialogueHUD::Execute_IsDisplayed(PlayerHUD.GetObject(), UIName))
	{
		// TODO check this:
		// OldWidget->StopAllAnimations();
		return EBTNodeResult::Type::Succeeded;
	}

	IDialogueHUD::Execute_DisplayUIPanel(PlayerHUD.GetObject(), UIToCreate, UIName);
	Blackboard->SetValueAsName(UINameKey, UIName);

	return EBTNodeResult::Type::Succeeded;
}

FString UBTTask_CreateUIPanel::GetStaticDescription() const
{
	FString ReturnDesc = Super::GetStaticDescription();

	ReturnDesc += ":\nUI: " + UINameKey.ToString();
	ReturnDesc += "\nUIName: " + UIName.ToString();
	ReturnDesc += "\nUI To Create: ";
	if (UIToCreate != nullptr && !UIToCreate->GetDefaultObject()->GetClass()->GetFullName().IsEmpty())
	{
		ReturnDesc += UIToCreate->GetDefaultObject()->GetClass()->GetName();
	}
	else
	{
		ReturnDesc += "None";
	}

	return ReturnDesc;
}

#if WITH_EDITOR
FName UBTTask_CreateUIPanel::GetNodeIconName() const
{
	// TODO import my own icon
	return FName("BTEditor.Graph.BTNode.Task.Wait.Icon");
}
#endif	  // WITH_EDITOR

#undef LOCTEXT_NAMESPACE
