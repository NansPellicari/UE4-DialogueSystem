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

#include "AI/Decorator/BTDecorator_CheckUIDisplayed.h"

#include "Service/DialogueBTHelpers.h"
#include "Setting/DialogueSystemSettings.h"

UBTDecorator_CheckUIDisplayed::UBTDecorator_CheckUIDisplayed(const FObjectInitializer& ObjectInitializer)
{
	NodeName = "Check: UI Displayed";
	const auto Settings = UDialogueSystemSettings::Get()->BehaviorTreeSettings;
	if (UIName == NAME_None)
	{
		UIName = Settings.DefaultUIName;
	}
}

bool UBTDecorator_CheckUIDisplayed::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory) const
{
	const auto PlayerHUD = NDialogueBTHelpers::GetPlayerHUD(OwnerComp, FString(__FUNCTION__));
	bool bDisplayed = false;
	if (PlayerHUD != nullptr)
	{
		bDisplayed = IDialogueHUD::Execute_IsDisplayed(PlayerHUD.GetObject(), UIName);
	}
	return bDisplayed;
}
