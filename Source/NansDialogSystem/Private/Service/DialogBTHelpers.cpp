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

#include "Service/DialogBTHelpers.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Misc/ErrorUtils.h"
#include "Service/InteractiveBTHelpers.h"
#include "Setting/InteractiveSettings.h"
#include "UI/DialogHUD.h"
#include "UI/InteractiveHUDPlayer.h"

#define LOCTEXT_NAMESPACE "DialogSystem"

NDialogBTHelpers::NDialogBTHelpers() {}

NDialogBTHelpers::~NDialogBTHelpers() {}

UDialogHUD* NDialogBTHelpers::GetHUDFromBlackboard(UBehaviorTreeComponent& OwnerComp, UBlackboardComponent* Blackboard)
{
	const auto Settings = UInteractiveSettings::Get()->BehaviorTreeSettings;
	const FName UINameKey = Settings.UINameKey;
	const auto PlayerHUD = NInteractiveBTHelpers::GetPlayerHUD(OwnerComp, FString(__FUNCTION__));
	if (!IsValid(PlayerHUD))
	{
		// error are already manage in NInteractiveBTHelpers::GetPlayerHUD()
		return nullptr;
	}
	const FName UIName = Blackboard->GetValueAsName(UINameKey);
	if (!PlayerHUD->IsDisplayed(UIName))
	{
		EDITOR_ERROR(
			"DialogSystem",
			FText::Format(LOCTEXT("WrongHUDName", "The HUD {0} is not currently displayed "), FText::FromName(UIName)),
			&OwnerComp
		);
		return nullptr;
	}

	return Cast<UDialogHUD>(PlayerHUD->GetCurrentUIPanel());
}
#undef LOCTEXT_NAMESPACE
