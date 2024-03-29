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

#include "GameplayTags.h"
#include "BTDialogueTypes.generated.h"

struct FNDialogueDifficultyMagnitudeFactorSettings;
struct FNDialogueDifficultyMagnitudeSettings;
class UGameplayEffect;

UENUM(BlueprintType)
enum class EResponseDirection : uint8
{
	UP,
	DOWN,
	NONE,
};

#ifndef TOFLAG
#define TOFLAG(Enum) (1 << static_cast<uint8>(Enum))
#endif

struct FNDialogueCategorySettings;
struct FNDialogFactorTypeSettings;

USTRUCT(BlueprintType)
struct NANSDIALOGUESYSTEM_API FNDialogueCategory
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DialogueCategory")
	FGameplayTag Name;

	static FLinearColor GetColorFromSettings(const FNDialogueCategory& DialogueCategory);
	static TArray<FNDialogueDifficultyMagnitudeFactorSettings> GetDifficulties(
		const FNDialogueCategory& DialogueCategory);

	FORCEINLINE friend uint32 GetTypeHash(const FNDialogueCategory& Var)
	{
		return GetTypeHash(Var.Name.GetTagName());
	}
};

/**
 * It is a params struct used in BehaviorTree UI to set a button data.
 */
USTRUCT(BlueprintType)
struct NANSDIALOGUESYSTEM_API FBTDialogueResponse
{
	GENERATED_USTRUCT_BODY()

	FBTDialogueResponse() { }

	FBTDialogueResponse(
		FNDialogueCategory InCategory,
		FText InText,
		int32 InPoint
	) :
		Text(InText),
		Point(InPoint),
		Category(InCategory) { }

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Response", meta = (MultiLine = true))
	FText Text;
	// It is used for button sequence only now.
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Response", meta = (MultiLine = false))
	FText AltText;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Response")
	int32 Point = 0;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Response")
	float Difficulty = 0;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Response")
	FNDialogueCategory Category;

	/** If true, tasks will use the Default GameplayEffect set in DialogueSystemSettings. */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Response")
	bool bUsedDefaultEffect = true;

	/**
	* /!\ Note: Be aware that this effect is used to create a GameplayEffectSpec in Tasks to add:
	*
	* - some level magnitude (using SetByCaller)
	* - some extra data to pass to the PlayerDialogueComponent (see FNDSGameplayEffectContext)
	* - asset tags to identify easily the gameplay effect as a dialog effect (see UDialogueSystemSettings::TagToIdentifyDialogEffect)
	*
	* Take this in consideration when you build your GameplayEffect.
	*/
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Response", Meta=(EditCondition="!bUsedDefaultEffect"))
	TSubclassOf<UGameplayEffect> EffectOnEarned;

	TSubclassOf<UGameplayEffect> GetSpawnableEffectOnEarned() const;
};
