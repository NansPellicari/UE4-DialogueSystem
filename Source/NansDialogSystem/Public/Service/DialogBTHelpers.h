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

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Component/PlayerDialogComponent.h"
#include "UI/DialogHUD.h"

class UBehaviorTreeComponent;
class UBlackboardComponent;

/**
 * 
 */
class NANSDIALOGSYSTEM_API NDialogBTHelpers
{
public:
	NDialogBTHelpers();
	~NDialogBTHelpers();
	static UDialogHUD* GetHUDFromBlackboard(UBehaviorTreeComponent& OwnerComp, UBlackboardComponent* Blackboard);
	static UAbilitySystemComponent* GetGASC(UBehaviorTreeComponent& OwnerComp);
	static UPlayerDialogComponent* GetDialogComponent(UBehaviorTreeComponent& OwnerComp);
};
