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

#pragma once

#include "CoreMinimal.h"
#include "BTDialogueTypes.h"
#include "Setting/DialogueSystemSettings.h"

#include "BTDialogueDifficultyHandler.generated.h"

class UBehaviorTreeComponent;
// struct FNDialogueFactorSettings;
class UNFactorsFactoryClientAdapter;
class UAbilitySystemComponent;

UCLASS()
class NANSDIALOGUESYSTEM_API UBTDialogueDifficultyHandler : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	bool bDebug = false;

	void Initialize(UBehaviorTreeComponent& OwnerComp);
	void Clear();

	UBTDialogueDifficultyHandler();
	float GetDifficultyLevel(const FBTDialogueResponse& Response);

protected:
	/** Should change depending on the Character the player will choose */
	float GeneralDifficultyLevel = 1.f;
	TArray<FNDialogueFactorSettings> Settings;

	UPROPERTY()
	UAbilitySystemComponent* PlayerGASC;
};
