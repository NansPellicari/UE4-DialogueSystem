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

#include "Service/DialogBTHelpers.h"

#include "AbilitySystemComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/HUD.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/ErrorUtils.h"
#include "Setting/DialogSystemSettings.h"
#include "UI/DialogueUI.h"

#define LOCTEXT_NAMESPACE "DialogSystem"

NDialogBTHelpers::NDialogBTHelpers() {}

NDialogBTHelpers::~NDialogBTHelpers() {}

ACharacter* NDialogBTHelpers::GetPlayerCharacter(UBrainComponent& OwnerComp, const FString TaskName)
{
	ACharacter* PlayerChar = UGameplayStatics::GetPlayerCharacter(&OwnerComp, 0);
	check(IsValid(PlayerChar));
	return PlayerChar;
}

APlayerController* NDialogBTHelpers::GetPlayerController(UBrainComponent& OwnerComp, const FString TaskName)
{
	const auto PlayerChar = GetPlayerCharacter(OwnerComp, TaskName);
	return Cast<APlayerController>(PlayerChar->GetController());
}

UDialogueUI* NDialogBTHelpers::GetUIFromBlackboard(UBehaviorTreeComponent& OwnerComp, UBlackboardComponent* Blackboard)
{
	const auto Settings = UDialogSystemSettings::Get()->BehaviorTreeSettings;
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
			"DialogSystem",
			FText::Format(LOCTEXT("WrongUIName", "The UI {0} is not currently displayed "), FText::FromName(UIName)),
			&OwnerComp
		);
		return nullptr;
	}

	return Cast<UDialogueUI>(IDialogueHUD::Execute_GetCurrentUIPanel(PlayerHUD.GetObject()));
}

TScriptInterface<IDialogueHUD> NDialogBTHelpers::GetPlayerHUD(UBehaviorTreeComponent& OwnerComp, const FString TaskName)
{
	APlayerController* PlayerController = GetPlayerController(OwnerComp, TaskName);
	const auto PlayerHUD = PlayerController->GetHUD();
	if (!IsValid(PlayerHUD) || !PlayerHUD->Implements<UDialogueHUD>())
	{
		EDITOR_ERROR(
			"DialogSystem",
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

void NDialogBTHelpers::
RemoveUIFromBlackboard(UBehaviorTreeComponent& OwnerComp, UBlackboardComponent* Blackboard)
{
	const auto Settings = UDialogSystemSettings::Get()->BehaviorTreeSettings;
	const FName UINameKey = Settings.UINameKey;
	const FName PreviousUINameKey = Settings.PreviousUINameKey;
	const FName PreviousUIClassKey = Settings.PreviousUIClassKey;

	const FName WidgetName = Blackboard->GetValueAsName(UINameKey);
	auto PlayerHUD = GetPlayerHUD(OwnerComp, FString(__FUNCTION__));
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

UAbilitySystemComponent* NDialogBTHelpers::GetGASC(UBehaviorTreeComponent& OwnerComp)
{
	auto Char = GetPlayerCharacter(OwnerComp, __FUNCTION__);
	if (!IsValid(Char))
	{
		// errors are already manage in GetPlayerCharacter()
		return nullptr;
	}

	auto Comp = Char->FindComponentByClass<UAbilitySystemComponent>();
	if (!IsValid(Comp))
	{
		EDITOR_ERROR(
			"DialogSystem",
			FText::Format(LOCTEXT("ABSNotFound", "The UAbilitySystemComponent has not be found in {0} "), FText::
				FromString(OwnerComp.GetFullName())),
			&OwnerComp
		);
		return nullptr;
	}
	return Comp;
}

UPlayerDialogComponent* NDialogBTHelpers::GetDialogComponent(UBehaviorTreeComponent& OwnerComp)
{
	auto Char = GetPlayerCharacter(OwnerComp, __FUNCTION__);
	if (!IsValid(Char))
	{
		// errors are already manage in GetPlayerCharacter()
		return nullptr;
	}

	auto Comp = Char->FindComponentByClass<UPlayerDialogComponent>();
	if (!IsValid(Comp))
	{
		EDITOR_ERROR(
			"DialogSystem",
			FText::Format(LOCTEXT("PlayerDialogCompNotFound", "The UPlayerDialogComponent has not be found in {0} "),
				FText::
				FromString(OwnerComp.GetFullName())),
			&OwnerComp
		);
		return nullptr;
	}
	return Comp;
}
#undef LOCTEXT_NAMESPACE
