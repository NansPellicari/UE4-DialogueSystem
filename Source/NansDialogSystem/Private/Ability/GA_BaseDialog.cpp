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

#include "Ability/GA_BaseDialog.h"


#include "GameplayEffectExtension.h"
#include "NansDialogSystemLog.h"
#include "Component/PlayerDialogComponent.h"
#include "Ability/NDSGameplayEffectTypes.h"
#include "Ability/AbilityTasks/AT_WaitReceivePoints.h"
#include "Ability/AttributeSets/DialogAttributeSets.h"
#include "Dialogue/DialogueResult.h"
#include "GameFramework/Character.h"
#include "Misc/ErrorUtils.h"
#include "Setting/DialogSystemSettings.h"

#define LOCTEXT_NAMESPACE "DialogSystem"

UGA_BaseDialog::UGA_BaseDialog()
{
	const FGameplayTagContainer TagsToAdd = UDialogSystemSettings::Get()->TagsForDialogAbility;
	if (TagsToAdd.Num() > 0)
	{
		AbilityTags.AppendTags(TagsToAdd);
	}
	const FGameplayTagContainer TagsActivationOwned = UDialogSystemSettings::Get()->DialogActivationOwnedTags;
	if (TagsActivationOwned.Num() > 0)
	{
		ActivationOwnedTags.AppendTags(TagsActivationOwned);
	}
	const FGameplayTagContainer TagsBlocking = UDialogSystemSettings::Get()->TagsBlockingDialogAbility;
	if (TagsBlocking.Num() > 0)
	{
		ActivationBlockedTags.AppendTags(TagsBlocking);
	}

	FAbilityTriggerData TriggerData = FAbilityTriggerData();
	TriggerData.TriggerTag = UDialogSystemSettings::Get()->TriggerAbilityTag;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);

	bIsDebug = UDialogSystemSettings::Get()->bDebugDialogAbility;
}

void UGA_BaseDialog::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo)) return;

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
	if (bIsDebug)
	{
		UE_LOG(LogDialogSystem, Display, TEXT("%s Activate ability"), ANSI_TO_TCHAR(__FUNCTION__));
	}

	Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());
	UAbilitySystemComponent* ABS = Character->FindComponentByClass<UAbilitySystemComponent>();

	UAT_WaitReceivePoints* Task = UAT_WaitReceivePoints::WaitReceivePoints(this, false);
	Task->OnPointsEarned.AddDynamic(this, &UGA_BaseDialog::OnPointsEarnedAttributeChanged);
	// ReadyForActivation() is how you activate the AbilityTask in C++.
	// Blueprint has magic from K2Node_LatentGameplayTaskCall that will automatically call ReadyForActivation().
	Task->ReadyForActivation();
}

bool UGA_BaseDialog::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	ACharacter* TmpCharacter = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());
	UPlayerDialogComponent* DialogComp = TmpCharacter->FindComponentByClass<UPlayerDialogComponent>();
	if (!IsValid(DialogComp))
	{
		if (bIsDebug)
		{
			UE_LOG(
				LogDialogSystem,
				Warning,
				TEXT("%s no PlayerDialogComponent in the character %s, it can't works with Dialog System"),
				ANSI_TO_TCHAR(__FUNCTION__),
				*TmpCharacter->GetFullName()
			);
		}
		return false;
	}

	return true;
}

void UGA_BaseDialog::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	if (ScopeLockCount > 0)
	{
		WaitingToExecute.Add(
			FPostLockDelegate::CreateUObject(
				this,
				&UGA_BaseDialog::CancelAbility,
				Handle,
				ActorInfo,
				ActivationInfo,
				bReplicateCancelAbility
			)
		);
		return;
	}

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

	if (bIsDebug)
	{
		UE_LOG(LogDialogSystem, Display, TEXT("%s Ability cancelled"), ANSI_TO_TCHAR(__FUNCTION__));
	}
}

void UGA_BaseDialog::OnPointsEarnedAttributeChanged(UNDSAbilitySystemComponent* SourceASC, float UnmitigatedPoints,
	float MitigatedPoints, FDialogueResult ExtraData)
{
	UPlayerDialogComponent* DialogComp = Character->FindComponentByClass<UPlayerDialogComponent>();

	if (bIsDebug)
	{
		UE_LOG(
			LogDialogSystem,
			Display,
			TEXT("%s PointEarned Changed, for: %s, UnmitigatedPoints: %f, MitigatedPoints: %f"),
			ANSI_TO_TCHAR(__FUNCTION__),
			*SourceASC->GetOwnerActor()->GetName(),
			UnmitigatedPoints,
			MitigatedPoints
		);
		UE_LOG(LogDialogSystem, Display, TEXT("%s GetData: %s"), ANSI_TO_TCHAR(__FUNCTION__), *ExtraData.ToString());
	}

	DialogComp->AddResponse(ExtraData);
}

void UGA_BaseDialog::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (bIsDebug)
	{
		UE_LOG(LogDialogSystem, Display, TEXT("%s Ability ends"), ANSI_TO_TCHAR(__FUNCTION__));
	}
}

#undef LOCTEXT_NAMESPACE
