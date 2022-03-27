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

#include "Service/DialogueBTHelpers.h"

#include "AbilitySystemComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/HUD.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/ErrorUtils.h"
#include "Setting/DialogueSystemSettings.h"
#include "UI/DialogueUI.h"

#define LOCTEXT_NAMESPACE "DialogueSystem"

NDialogueBTHelpers::NDialogueBTHelpers()
{
}

NDialogueBTHelpers::~NDialogueBTHelpers()
{
}

ACharacter* NDialogueBTHelpers::GetPlayerCharacter(const AAIController& AIController)
{
	ACharacter* PlayerChar = UGameplayStatics::GetPlayerCharacter(&AIController, 0);
	if (!IsValid(PlayerChar))
	{
		return nullptr;
	}
	return PlayerChar;
}

APlayerController* NDialogueBTHelpers::GetPlayerController(UBrainComponent& OwnerComp, const FString TaskName)
{
	check(OwnerComp.GetAIOwner())
	const auto PlayerChar = GetPlayerCharacter(*OwnerComp.GetAIOwner());
	if (!IsValid(PlayerChar))
	{
		return nullptr;
	}
	return Cast<APlayerController>(PlayerChar->GetController());
}

UDialogueUI* NDialogueBTHelpers::GetUIFromBlackboard(UBehaviorTreeComponent& OwnerComp,
                                                     UBlackboardComponent* Blackboard)
{
	const auto Settings = UDialogueSystemSettings::Get()->BehaviorTreeSettings;
	const FName UINameKey = Settings.UINameKey;
	const auto PlayerHUD = GetPlayerHUD(OwnerComp, FString(__FUNCTION__));
	if (!IsValid(PlayerHUD.GetObject()))
	{
		// errors are already manage in GetPlayerHUD()
		return nullptr;
	}
	const FName UIName = Blackboard->GetValueAsName(UINameKey);
	if (!IDialogueHUD::Execute_IsDisplayed(PlayerHUD.GetObject(), UIName))
	{
		EDITOR_ERROR(
			"DialogueSystem",
			FText::Format(LOCTEXT("WrongUIName", "The UI {0} is not currently displayed "), FText::FromName(UIName)),
			&OwnerComp
		);
		return nullptr;
	}

	return Cast<UDialogueUI>(IDialogueHUD::Execute_GetCurrentUIPanel(PlayerHUD.GetObject()));
}

TScriptInterface<IDialogueHUD> NDialogueBTHelpers::GetPlayerHUD(UBehaviorTreeComponent& OwnerComp,
                                                                const FString TaskName)
{
	const APlayerController* PlayerController = GetPlayerController(OwnerComp, TaskName);
	if (!IsValid(PlayerController))
	{
		EDITOR_WARN(
			"DialogueSystem",
			FText::Format(LOCTEXT("NoPlayerController", "no player controller when called {0}"), FText::FromString(
				TaskName)),
			&OwnerComp
		);
		return nullptr;
	}
	const auto PlayerHUD = PlayerController->GetHUD();
	if (!IsValid(PlayerHUD) || !PlayerHUD->Implements<UDialogueHUD>())
	{
		EDITOR_ERROR(
			"DialogueSystem",
			FText::Format(LOCTEXT("WrongHUD", "{0} only works with IDialogueHUD"), FText::FromString(TaskName)),
			&OwnerComp
		);
		return nullptr;
	}
	TScriptInterface<IDialogueHUD> DialogueHUD;
	DialogueHUD.SetInterface(Cast<IDialogueHUD>(PlayerHUD));
	DialogueHUD.SetObject(PlayerHUD);
	return DialogueHUD;
}

void NDialogueBTHelpers::RemoveUIFromBlackboard(UBehaviorTreeComponent& OwnerComp, UBlackboardComponent* Blackboard)
{
	const auto Settings = UDialogueSystemSettings::Get()->BehaviorTreeSettings;
	const FName UINameKey = Settings.UINameKey;
	const FName PreviousUINameKey = Settings.PreviousUINameKey;
	const FName PreviousUIClassKey = Settings.PreviousUIClassKey;

	const FName WidgetName = Blackboard->GetValueAsName(UINameKey);
	const auto PlayerHUD = GetPlayerHUD(OwnerComp, FString(__FUNCTION__));
	if (!IsValid(PlayerHUD.GetObject()))
	{
		// Error are already manage in GetPlayerHUD()
		return;
	}

	// Widget not exists or already remove
	if (!IDialogueHUD::Execute_IsDisplayed(PlayerHUD.GetObject(), WidgetName))
	{
		return;
	}

	const FName OldUIName = Blackboard->GetValueAsName(PreviousUINameKey);
	if (OldUIName != NAME_None)
	{
		const TSubclassOf<UUserWidget> OldUIClass = Blackboard->GetValueAsClass(PreviousUIClassKey);
		IDialogueHUD::Execute_DisplayUIPanel(PlayerHUD.GetObject(), OldUIClass, OldUIName);
		Blackboard->SetValueAsName(PreviousUINameKey, NAME_None);
		Blackboard->SetValueAsName(PreviousUINameKey, NAME_None);
	}
	else
	{
		IDialogueHUD::Execute_HideUIPanel(PlayerHUD.GetObject(), WidgetName);
	}

	Blackboard->SetValueAsName(UINameKey, NAME_None);
}

UAbilitySystemComponent* NDialogueBTHelpers::GetGASC(UBehaviorTreeComponent& OwnerComp)
{
	check(OwnerComp.GetAIOwner())
	const auto Char = GetPlayerCharacter(*OwnerComp.GetAIOwner());
	if (!IsValid(Char))
	{
		// errors are already manage in GetPlayerCharacter()
		return nullptr;
	}

	auto Comp = Char->FindComponentByClass<UAbilitySystemComponent>();
	if (!IsValid(Comp))
	{
		EDITOR_ERROR(
			"DialogueSystem",
			FText::Format(LOCTEXT("ABSNotFound", "The UAbilitySystemComponent has not be found in {0} "), FText::
				FromString(OwnerComp.GetFullName())),
			&OwnerComp
		);
		return nullptr;
	}
	return Comp;
}

UPlayerDialogueComponent* NDialogueBTHelpers::GetDialogueComponent(const ACharacter* PlayerChar)
{
	check(IsValid(PlayerChar));

	UPlayerDialogueComponent* Comp = PlayerChar->FindComponentByClass<UPlayerDialogueComponent>();
	if (!IsValid(Comp))
	{
		EDITOR_ERROR(
			"DialogueSystem",
			FText::Format(LOCTEXT("PlayerDialogueCompNotFound",
					"The UPlayerDialogueComponent was not found for player {0} ")
				, FText::FromString(PlayerChar->GetFullName())),
			PlayerChar
		);
		return nullptr;
	}
	return Comp;
}

void NDialogueBTHelpers::ChangeHUDForDialogue(UBehaviorTreeComponent& OwnerComp,
                                              UBlackboardComponent* BlackboardComp)
{
	const FDialogueBehaviorTreeSettings Settings = UDialogueSystemSettings::Get()->BehaviorTreeSettings;
	const auto PlayerHUD = GetPlayerHUD(OwnerComp, FString(__FUNCTION__));
	if (!IsValid(PlayerHUD.GetObject()))
	{
		return;
	}

	const FName PreviousUINameKey = Settings.PreviousUINameKey;
	const FName PreviousUIClassKey = Settings.PreviousUIClassKey;

	// Save previous UIPanel to reload it next time. 
	const FName PreviousName = BlackboardComp->GetValueAsName(PreviousUINameKey);
	if (PreviousName == NAME_None)
	{
		FName OldUIName;
		TSubclassOf<UUserWidget> OldUIClass;
		IDialogueHUD::Execute_GetFullCurrentUIPanel(PlayerHUD.GetObject(), OldUIClass, OldUIName);
		BlackboardComp->SetValueAsName(PreviousUINameKey, OldUIName);
		BlackboardComp->SetValueAsClass(PreviousUIClassKey, OldUIClass);
	}

	RemoveUIFromBlackboard(OwnerComp, BlackboardComp);
}
#undef LOCTEXT_NAMESPACE
