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

#include "Ability/AbilityTasks/AT_WaitReceivePoints.h"
#include "Ability/NDSAbilitySystemComponent.h"

UAT_WaitReceivePoints::UAT_WaitReceivePoints(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	TriggerOnce = false;
}

UAT_WaitReceivePoints* UAT_WaitReceivePoints::WaitReceivePoints(UGameplayAbility* OwningAbility,
	bool InTriggerOnce)
{
	UAT_WaitReceivePoints* MyObj = NewAbilityTask<UAT_WaitReceivePoints>(OwningAbility);
	MyObj->TriggerOnce = InTriggerOnce;
	return MyObj;
}

void UAT_WaitReceivePoints::Activate()
{
	UNDSAbilitySystemComponent* ASC = Cast<UNDSAbilitySystemComponent>(AbilitySystemComponent);

	if (ASC)
	{
		ASC->ReceivedPoints.AddDynamic(this, &UAT_WaitReceivePoints::OnPointsReceived);
	}
}

void UAT_WaitReceivePoints::OnDestroy(bool AbilityIsEnding)
{
	UNDSAbilitySystemComponent* ASC = Cast<UNDSAbilitySystemComponent>(AbilitySystemComponent);

	if (ASC)
	{
		ASC->ReceivedPoints.RemoveDynamic(this, &UAT_WaitReceivePoints::OnPointsReceived);
	}

	Super::OnDestroy(AbilityIsEnding);
}

void UAT_WaitReceivePoints::OnPointsReceived(UNDSAbilitySystemComponent* SourceASC, float UnmitigatedPoints,
	float MitigatedPoints, FDialogueBlockResult ExtraData)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnPointsEarned.Broadcast(SourceASC, UnmitigatedPoints, MitigatedPoints, ExtraData);
	}

	if (TriggerOnce)
	{
		EndTask();
	}
}
