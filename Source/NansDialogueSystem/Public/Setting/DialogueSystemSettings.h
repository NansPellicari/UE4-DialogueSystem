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
#include "AttributeSet.h"
#include "BTDialogueTypes.h"
#include "GameplayEffect.h"
#include "Ability/GA_BaseDialogue.h"
#include "Ability/AttributeSets/DialogueAttributeSets.h"
#include "Engine/DeveloperSettings.h"

#include "DialogueSystemSettings.generated.h"

USTRUCT()
struct FDialogueBehaviorTreeSettings
{
	GENERATED_BODY()

	UPROPERTY(config, EditAnywhere, Category= "Task|CreateUI" )
	FName DefaultUIName = FName("UIForAI");
	UPROPERTY(config, EditAnywhere, Category= "Task|AccessUI" )
	FName UINameKey = FName("CurrentUIName");
	UPROPERTY(config, EditAnywhere, Category= "Task|AccessUI" )
	FName PreviousUINameKey = FName("PreviousUIName");
	UPROPERTY(config, EditAnywhere, Category= "Task|AccessUI" )
	FName PreviousUIClassKey = FName("PreviousUIClass");
	UPROPERTY(config, EditAnywhere, Category= "Task|Points" )
	FName PointsHandlerKey = FName("PointsHandler");
	UPROPERTY(config, EditAnywhere, Category= "Task|Points" )
	FName DifficultyHandlerKey = FName("DifficultyHandler");
};

USTRUCT()
struct NANSDIALOGUESYSTEM_API FNDialogueFactorSettings
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = Difficulty)
	FNDialogueCategory Category;
	UPROPERTY(EditAnywhere, Category = Difficulty)
	float Multiplier;
	UPROPERTY(EditAnywhere, Category = Difficulty, Meta = (ClampMin = "0", ClampMax = "1"))
	float RangeFrom;
	UPROPERTY(EditAnywhere, Category = Difficulty, Meta = (ClampMin = "0", ClampMax = "1"))
	float RangeTo;
};

USTRUCT()
struct NANSDIALOGUESYSTEM_API FNDialogueDifficultyMagnitudeFactorSettings
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = Difficulty)
	FGameplayAttribute AttributeValue;

	UPROPERTY(EditAnywhere, Category = ResponseCategory)
	FGameplayAttribute MaxAtttributeValue;
};

USTRUCT()
struct NANSDIALOGUESYSTEM_API FNDialogueDifficultyMagnitudeSettings
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	FNDialogueCategory Category;

	UPROPERTY(EditAnywhere)
	TArray<FNDialogueDifficultyMagnitudeFactorSettings> Attributes;

	static FNDialogueDifficultyMagnitudeSettings* CreateNullInstance()
	{
		FNDialogueDifficultyMagnitudeSettings* NullInst = new FNDialogueDifficultyMagnitudeSettings();
		NullInst->Category.Name = FGameplayTag::EmptyTag;
		return NullInst;
	}
};

USTRUCT()
struct NANSDIALOGUESYSTEM_API FNDialogueCategorySettings
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = ResponseCategory)
	FGameplayTag Name;

	UPROPERTY(EditAnywhere, Category = ResponseCategory)
	FLinearColor Color;

	/**
	 * /!\ Note: Be aware that this default effect is used to create a GameplayEffectSpec in Tasks to add:
	 *
	 * - some level magnitude (using SetByCaller)
	 * - some extra data to pass to the PlayerDialogueComponent (see FNDSGameplayEffectContext)
	 * - asset tags to identify easily the gameplay effect as a dialog effect (see UDialogueSystemSettings::TagToIdentifyDialogEffect)
	 *
	 * Take this in consideration when you build your GameplayEffect.
	 */
	UPROPERTY(EditDefaultsOnly, Category=GameplayEffect)
	TSubclassOf<UGameplayEffect> DefaultEffect;
};

/**
 *
 */
UCLASS(config = Game, defaultconfig, meta = (DisplayName = "Dialogue System"))
class NANSDIALOGUESYSTEM_API UDialogueSystemSettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	/**
	* /!\ This tag will be add to each GameplayEffectSpec create by the Dialogue system.
	* It helps to identify clearly where the effect comes from.
	*/
	UPROPERTY(GlobalConfig, EditAnywhere)
	FGameplayTag TagToIdentifyDialogueEffect;

	UPROPERTY(GlobalConfig, EditAnywhere)
	FGameplayTag TriggerAbilityTag;

	UPROPERTY(GlobalConfig, EditAnywhere)
	bool bUseDefaultDialogueAbility = true;

	/**
	* Before going in any dialog, the Dialogue System checks if player has ability to talk.
	*/
	UPROPERTY(GlobalConfig, EditAnywhere, Meta=(EditCondition="!bUseDefaultDialogueAbility"))
	TSubclassOf<UGameplayAbility> RequiredAbility = UGA_BaseDialogue::StaticClass();

	/**
	* /!\ This is the Meta attribute you should create in your AttributeSet.
	* This meta attribute should be dedicated to collect dialog points.
	*/
	UPROPERTY(GlobalConfig, EditAnywhere, Meta=(EditCondition="!bUseDefaultDialogueAbility"))
	FGameplayAttribute MetaAttributeForPointsEarning;

	/**
	 * Set if GA_BaseDialog ability can debug.
	 */
	UPROPERTY(GlobalConfig, EditAnywhere, Meta=(EditCondition="bUseDefaultDialogueAbility"))
	bool bDebugDialogueAbility = false;

	/**
	 * These tags are added to the player when he starts a dialog.
	 * The dialog ability set them in Activation Owned Tags
	 */
	UPROPERTY(GlobalConfig, EditAnywhere, Meta=(EditCondition="bUseDefaultDialogueAbility"))
	FGameplayTagContainer DialogueActivationOwnedTags;

	/**
	 * Tags to add in the dialog ability 
	 */
	UPROPERTY(GlobalConfig, EditAnywhere, Meta=(EditCondition="bUseDefaultDialogueAbility"))
	FGameplayTagContainer TagsForDialogueAbility;

	/**
	 * Tags that ability's owner has which can block Dialog ability activation.
	 */
	UPROPERTY(GlobalConfig, EditAnywhere, Meta=(EditCondition="bUseDefaultDialogueAbility"))
	FGameplayTagContainer TagsBlockingDialogueAbility;

	/**
	 * This tag is used to send point magnitude on the GameplayEffect,
	 * it should influence earned points amount.
	 */
	UPROPERTY(GlobalConfig, EditAnywhere)
	FGameplayTag PointMagnitudeTag;

	UPROPERTY(GlobalConfig, EditAnywhere, Category="Response")
	bool bFlushColorCache = false;
	UPROPERTY(GlobalConfig, EditAnywhere, Category="Response")
	TArray<FNDialogueCategorySettings> ResponseCategorySettings;

	UPROPERTY(GlobalConfig, EditAnywhere, Category="Difficulty")
	TArray<FNDialogueDifficultyMagnitudeSettings> DifficultyMagnitudesSettings;
	UPROPERTY(GlobalConfig, EditAnywhere, Category="Difficulty")
	TArray<FNDialogueFactorSettings> DifficultySettings;

	/**
	* This is used by BTTask_CreateHUD & BTTask_RemoveHUD,
	* it will set a default name to the UI created for the DialogueUI container. 
	*/
	UPROPERTY(config, EditAnywhere, Category= "AI" )
	FDialogueBehaviorTreeSettings BehaviorTreeSettings;

	/** Accessor and initializer **/
	static const UDialogueSystemSettings* Get()
	{
		const UDialogueSystemSettings* StaticObject = GetDefault<UDialogueSystemSettings>();
		return StaticObject;
	}

	void GetDifficultyConfigs(TArray<FNDialogueFactorSettings>& ShareableNames) const
	{
		for (FNDialogueFactorSettings Settings : DifficultySettings)
		{
			ShareableNames.Add(Settings);
		}
	}

	FGameplayAttribute GetMetaAttributeForPointsEarning() const
	{
		if (bUseDefaultDialogueAbility)
		{
			return UDialogueAttributeSets::GetPointsEarnedAttribute();
		}
		return MetaAttributeForPointsEarning;
	}

	void GetDialogueCategoryConfigs(TArray<FNDialogueCategorySettings>& ShareableNames) const
	{
		for (FNDialogueCategorySettings Settings : ResponseCategorySettings)
		{
			ShareableNames.Add(Settings);
		}
	}

	void GetDialogueDifficultyMagnitudeConfigs(TArray<FNDialogueDifficultyMagnitudeSettings>& ShareableNames) const
	{
		for (FNDialogueDifficultyMagnitudeSettings Settings : DifficultyMagnitudesSettings)
		{
			ShareableNames.Add(Settings);
		}
	}
};
