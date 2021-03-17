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

#include "NDSFunctionLibrary.h"


#include "Ability/NDSAbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/ErrorUtils.h"
#include "Setting/DialogueSystemSettings.h"

#define LOCTEXT_NAMESPACE "DialogueSystem"

void UNDSFunctionLibrary::EffectContextAddPointsData(FGameplayEffectContextHandle EffectContextHandle,
	FDialogueResult Data)
{
	FNDSGameplayEffectContext* EffectContext = static_cast<FNDSGameplayEffectContext*>(EffectContextHandle.Get());
	if (EffectContext)
	{
		EffectContext->AddPointsData(Data);
	}
}

void UNDSFunctionLibrary::EffectContextAddExtraData(FGameplayEffectContextHandle EffectContextHandle, FGameplayTag Tag,
	const FString& Data)
{
	FNDSGameplayEffectContext* EffectContext = static_cast<FNDSGameplayEffectContext*>(EffectContextHandle.Get());
	if (EffectContext)
	{
		EffectContext->AddExtraData(Tag, Data);
	}
}

void UNDSFunctionLibrary::EffectContextAddExtraDataAsMap(FGameplayEffectContextHandle EffectContextHandle,
	TMap<FGameplayTag, FString> NewData)
{
	FNDSGameplayEffectContext* EffectContext = static_cast<FNDSGameplayEffectContext*>(EffectContextHandle.Get());
	if (EffectContext)
	{
		EffectContext->AddExtraData(NewData);
	}
}

TMap<FGameplayTag, FString> UNDSFunctionLibrary::EffectContextGetExtraData(
	const FGameplayEffectContextHandle& EffectContextHandle)
{
	const FNDSGameplayEffectContext* EffectContext = static_cast<const FNDSGameplayEffectContext*>(EffectContextHandle.
		Get());
	TMap<FGameplayTag, FString> Data;
	if (EffectContext)
	{
		Data = EffectContext->GetExtraData();
	}
	return Data;
}

bool UNDSFunctionLibrary::IsPlayerCanDialogue(UObject* WorldContextObject, int32 PlayerIndex)
{
	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(WorldContextObject->GetWorld(), PlayerIndex);
	UNDSAbilitySystemComponent* ABS = Player->FindComponentByClass<UNDSAbilitySystemComponent>();
	if (!IsValid(ABS)) return false;
	TArray<FGameplayAbilitySpec*> MatchingGameplayAbilities;
	ABS->GetActivatableGameplayAbilitySpecsByAllMatchingTags(
		UDialogueSystemSettings::Get()->TagsForDialogueAbility,
		MatchingGameplayAbilities
	);
	if (MatchingGameplayAbilities.Num() <= 0)
	{
		EDITOR_ERROR(
			"DialogueSystem",
			LOCTEXT("CanNotAddPointsForACategory", "Player should be able to speak!"),
			WorldContextObject
		);
		return false;
	}
	return true;
}
#undef LOCTEXT_NAMESPACE
