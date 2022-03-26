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

#include "Ability/GA_BaseDialogue.h"

#include "GameplayEffectExtension.h"
#include "NansDialogueSystemLog.h"
#include "Component/PlayerDialogueComponent.h"
#include "Ability/NDSGameplayEffectTypes.h"
#include "Ability/AbilityTasks/AT_WaitReceivePoints.h"
#include "Ability/AttributeSets/DialogueAttributeSets.h"
#include "Dialogue/DialogueResult.h"
#include "GameFramework/Character.h"
#include "Misc/ErrorUtils.h"
#include "Setting/DialogueSystemSettings.h"

#define LOCTEXT_NAMESPACE "DialogSystem"

UGA_BaseDialogue::UGA_BaseDialogue()
{
	const FGameplayTagContainer TagsToAdd = UDialogueSystemSettings::Get()->TagsForDialogueAbility;
	if (TagsToAdd.Num() > 0)
	{
		AbilityTags.AppendTags(TagsToAdd);
	}
	const FGameplayTagContainer TagsActivationOwned = UDialogueSystemSettings::Get()->DialogueActivationOwnedTags;
	if (TagsActivationOwned.Num() > 0)
	{
		ActivationOwnedTags.AppendTags(TagsActivationOwned);
	}
	const FGameplayTagContainer TagsBlocking = UDialogueSystemSettings::Get()->TagsBlockingDialogueAbility;
	if (TagsBlocking.Num() > 0)
	{
		ActivationBlockedTags.AppendTags(TagsBlocking);
	}

	FAbilityTriggerData TriggerData = FAbilityTriggerData();
	TriggerData.TriggerTag = UDialogueSystemSettings::Get()->TriggerAbilityTag;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);

	bIsDebug = UDialogueSystemSettings::Get()->bDebugDialogueAbility;
}

void UGA_BaseDialogue::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
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
		UE_LOG(LogDialogueSystem, Display, TEXT("%s Activate ability"), ANSI_TO_TCHAR(__FUNCTION__));
	}

	Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());
	UAbilitySystemComponent* ABS = Character->FindComponentByClass<UAbilitySystemComponent>();

	UAT_WaitReceivePoints* Task = UAT_WaitReceivePoints::WaitReceivePoints(this, false);
	Task->OnPointsEarned.AddDynamic(this, &UGA_BaseDialogue::OnPointsEarnedAttributeChanged);
	// ReadyForActivation() is how you activate the AbilityTask in C++.
	// Blueprint has magic from K2Node_LatentGameplayTaskCall that will automatically call ReadyForActivation().
	Task->ReadyForActivation();
}

bool UGA_BaseDialogue::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	ACharacter* TmpCharacter = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());
	UPlayerDialogueComponent* DialogComp = TmpCharacter->FindComponentByClass<UPlayerDialogueComponent>();
	if (!IsValid(DialogComp))
	{
		if (bIsDebug)
		{
			UE_LOG(
				LogDialogueSystem,
				Warning,
				TEXT("%s no PlayerDialogueComponent in the character %s, it can't works with Dialog System"),
				ANSI_TO_TCHAR(__FUNCTION__),
				*TmpCharacter->GetFullName()
			);
		}
		return false;
	}

	return true;
}

void UGA_BaseDialogue::CancelAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility)
{
	if (ScopeLockCount > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s here 0"), ANSI_TO_TCHAR(__FUNCTION__));
		WaitingToExecute.Add(
			FPostLockDelegate::CreateUObject(
				this,
				&UGA_BaseDialogue::CancelAbility,
				Handle,
				ActorInfo,
				ActivationInfo,
				bReplicateCancelAbility
			)
		);
		UE_LOG(LogTemp, Warning, TEXT("%s here 1"), ANSI_TO_TCHAR(__FUNCTION__));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("%s here 2"), ANSI_TO_TCHAR(__FUNCTION__));
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
	UE_LOG(LogTemp, Warning, TEXT("%s here 3"), ANSI_TO_TCHAR(__FUNCTION__));

	if (bIsDebug)
	{
		UE_LOG(LogDialogueSystem, Display, TEXT("%s Ability cancelled"), ANSI_TO_TCHAR(__FUNCTION__));
	}
}

void UGA_BaseDialogue::OnPointsEarnedAttributeChanged(UNDSAbilitySystemComponent* SourceASC, float UnmitigatedPoints,
	float MitigatedPoints, FDialogueResult ExtraData)
{
	UPlayerDialogueComponent* DialogComp = Character->FindComponentByClass<UPlayerDialogueComponent>();

	if (bIsDebug)
	{
		UE_LOG(
			LogDialogueSystem,
			Display,
			TEXT("%s PointEarned Changed, for: %s, UnmitigatedPoints: %f, MitigatedPoints: %f"),
			ANSI_TO_TCHAR(__FUNCTION__),
			*SourceASC->GetOwnerActor()->GetName(),
			UnmitigatedPoints,
			MitigatedPoints
		);
		UE_LOG(LogDialogueSystem, Display, TEXT("%s GetData: %s"), ANSI_TO_TCHAR(__FUNCTION__), *ExtraData.ToString());
	}

	DialogComp->AddResponse(ExtraData);
}

void UGA_BaseDialogue::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (bIsDebug)
	{
		UE_LOG(LogDialogueSystem, Display, TEXT("%s Ability ends"), ANSI_TO_TCHAR(__FUNCTION__));
	}
}

#undef LOCTEXT_NAMESPACE
