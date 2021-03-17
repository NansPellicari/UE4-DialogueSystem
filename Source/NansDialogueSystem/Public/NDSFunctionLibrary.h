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


#include "Ability/NDSGameplayEffectTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NDSFunctionLibrary.generated.h"

/**
 * This blueprint lib is mainly useful to call FNDSGameplayEffectContext methods.
 */
UCLASS()
class NANSDIALOGUESYSTEM_API UNDSFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category="Ability|EffectContext")
	static void EffectContextAddPointsData(FGameplayEffectContextHandle EffectContextHandle, FDialogueResult Data);

	UFUNCTION(BlueprintCallable, Category="Ability|EffectContext")
	static void EffectContextAddExtraData(FGameplayEffectContextHandle EffectContextHandle, FGameplayTag Tag,
		const FString& Data);

	UFUNCTION(BlueprintCallable, Category="Ability|EffectContext")
	static void EffectContextAddExtraDataAsMap(FGameplayEffectContextHandle EffectContextHandle,
		TMap<FGameplayTag, FString> NewData);

	UFUNCTION(BlueprintCallable, Category="Ability|EffectContext")
	static TMap<FGameplayTag, FString> EffectContextGetExtraData(
		const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintCallable, Category="Ability", Meta = (WorldContext = "WorldContextObject"))
	static bool IsPlayerCanDialogue(UObject* WorldContextObject, int32 PlayerIndex = 0);
};
