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
#include "Abilities/Tasks/AbilityTask.h"
#include "Ability/NDSAbilitySystemComponent.h"
#include "Dialogue/DialogueResult.h"

#include "AT_WaitReceivePoints.generated.h"

/**
 * Waits until the Ability Owner receives damage.
 */
UCLASS()
class NANSDIALOGUESYSTEM_API UAT_WaitReceivePoints : public UAbilityTask
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(BlueprintAssignable)
	FReceivedPointsEarnedDelegate OnPointsEarned;

	virtual void Activate() override;

	UFUNCTION()
	void OnPointsReceived(class UNDSAbilitySystemComponent* SourceASC, float UnmitigatedPoints, float MitigatedPoints,
		FDialogueResult ExtraData);

	// Wait until the ability owner receives points.
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf =
		"OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAT_WaitReceivePoints* WaitReceivePoints(UGameplayAbility* OwningAbility, bool TriggerOnce);

protected:
	bool TriggerOnce;

	virtual void OnDestroy(bool AbilityIsEnding) override;
};
