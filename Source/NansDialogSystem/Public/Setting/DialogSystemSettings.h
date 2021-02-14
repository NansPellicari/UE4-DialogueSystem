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
#include "AttributeSet.h"
#include "BTDialogueTypes.h"
#include "GameplayEffect.h"
#include "Ability/GA_BaseDialog.h"
#include "Ability/AttributeSets/DialogAttributeSets.h"
#include "Attribute/FactorAttribute.h"
#include "Engine/DeveloperSettings.h"
#include "NansCoreHelpers/Public/Math/MathTypes.h"

#include "DialogSystemSettings.generated.h"

USTRUCT()
struct NANSDIALOGSYSTEM_API FNDialogFactorSettings
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
struct NANSDIALOGSYSTEM_API FNDialogFactorTypeSettings
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = FactorType)
	FFactorAttribute Factor;

	UPROPERTY(EditAnywhere, Category = FactorType, Meta = (Bitmask, BitmaskEnum = "EFactorType"))
	int32 Type;

	/**
	 * /!\ A range is mandatory for a difficulty factor.
	 */
	UPROPERTY(EditAnywhere, Category = FactorType)
	bool bHasRange = false;

	UPROPERTY(EditAnywhere, Category = FactorType)
	FRange Range = FRange(0, 100);
};

USTRUCT()
struct NANSDIALOGSYSTEM_API FNDialogueCategorySettings
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
	 * - some extra data to pass to the PlayerDialogComponent (see FNDSGameplayEffectContext)
	 * - asset tags to identify easily the gameplay effect as a dialog effect (see UDialogSystemSettings::TagToIdentifyDialogEffect)
	 *
	 * Take this in consideration when you build your GameplayEffect.
	 */
	UPROPERTY(EditDefaultsOnly, Category=GameplayEffect)
	TSubclassOf<UGameplayEffect> DefaultEffect;

	UPROPERTY(EditAnywhere, Category = ResponseCategory)
	TArray<FNDialogFactorTypeSettings> Factors;

	static FNDialogueCategorySettings* CreateNullInstance()
	{
		FNDialogueCategorySettings* NullInst = new FNDialogueCategorySettings();
		NullInst->Name = FGameplayTag::EmptyTag;
		NullInst->Color = FLinearColor::Black;
		return NullInst;
	}
};

/**
 *
 */
UCLASS(config = Game, defaultconfig, meta = (DisplayName = "Dialog System"))
class NANSDIALOGSYSTEM_API UDialogSystemSettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	/**
	* /!\ This tag will be add to each GameplayEffectSpec create by the Dialog system.
	* It helps to identify clearly where the effect comes from.
	*/
	UPROPERTY(GlobalConfig, EditAnywhere)
	FGameplayTag TagToIdentifyDialogEffect;

	UPROPERTY(GlobalConfig, EditAnywhere)
	FGameplayTag TriggerAbilityTag;

	UPROPERTY(GlobalConfig, EditAnywhere)
	bool bUseDefaultDialogAbility = true;

	/**
	* Before going in any dialog, the Dialog System checks if player has ability to talk.
	*/
	UPROPERTY(GlobalConfig, EditAnywhere, Meta=(EditCondition="!bUseDefaultDialogAbility"))
	TSubclassOf<UGameplayAbility> RequiredAbility = UGA_BaseDialog::StaticClass();

	/**
	* /!\ This is the Meta attribute you should create in your AttributeSet.
	* This meta attribute should be dedicated to collect dialog points.
	*/
	UPROPERTY(GlobalConfig, EditAnywhere, Meta=(EditCondition="!bUseDefaultDialogAbility"))
	FGameplayAttribute MetaAttributeForPointsEarning;

	/**
	 * Set if GA_BaseDialog ability can debug.
	 */
	UPROPERTY(GlobalConfig, EditAnywhere, Meta=(EditCondition="bUseDefaultDialogAbility"))
	bool bDebugDialogAbility = false;

	/**
	 * These tags are added to the player when he starts a dialog.
	 * The dialog ability set them in Activation Owned Tags
	 */
	UPROPERTY(GlobalConfig, EditAnywhere, Meta=(EditCondition="bUseDefaultDialogAbility"))
	FGameplayTagContainer DialogActivationOwnedTags;

	/**
	 * Tags to add in the dialog ability 
	 */
	UPROPERTY(GlobalConfig, EditAnywhere, Meta=(EditCondition="bUseDefaultDialogAbility"))
	FGameplayTagContainer TagsForDialogAbility;

	/**
	 * Tags that ability's owner has which can block Dialog ability activation.
	 */
	UPROPERTY(GlobalConfig, EditAnywhere, Meta=(EditCondition="bUseDefaultDialogAbility"))
	FGameplayTagContainer TagsBlockingDialogAbility;

	/**
	 * This tag is used to send point magnitude on the GameplayEffect,
	 * it should influence earned points amount.
	 */
	UPROPERTY(GlobalConfig, EditAnywhere)
	FGameplayTag PointMagnitudeTag;

	UPROPERTY(GlobalConfig, EditAnywhere)
	FFactorAttribute PointsCollector;
	UPROPERTY(GlobalConfig, EditAnywhere)
	TArray<FNDialogueCategorySettings> ResponseCategorySettings;
	UPROPERTY(GlobalConfig, EditAnywhere)
	TArray<FNDialogFactorSettings> DifficultySettings;

	/** Accessor and initializer **/
	static const UDialogSystemSettings* Get()
	{
		const UDialogSystemSettings* StaticObject = GetDefault<UDialogSystemSettings>();
		return StaticObject;
	}

	void GetDifficultyConfigs(TArray<FNDialogFactorSettings>& ShareableNames) const
	{
		for (FNDialogFactorSettings Settings : DifficultySettings)
		{
			ShareableNames.Add(Settings);
		}
	}

	FGameplayAttribute GetMetaAttributeForPointsEarning() const
	{
		if (bUseDefaultDialogAbility)
		{
			return UDialogAttributeSets::GetPointsEarnedAttribute();
		}
		return MetaAttributeForPointsEarning;
	}

	void GetPointsMultipliersConfigs(TMap<FGameplayTag, TArray<FNDialogFactorTypeSettings>>& Confs) const
	{
		for (FNDialogueCategorySettings Settings : ResponseCategorySettings)
		{
			for (auto& Factor : Settings.Factors)
			{
				if (Factor.Type & static_cast<int32>(EFactorType::PointsMultiplier))
				{
					if (!Confs.Contains(Settings.Name))
					{
						Confs.Add(Settings.Name, {});
					}
					Confs.Find(Settings.Name)->Add(Factor);
				}
			}
		}
	}

	void GetDialogueCategoryConfigs(TArray<FNDialogueCategorySettings>& ShareableNames) const
	{
		for (FNDialogueCategorySettings Settings : ResponseCategorySettings)
		{
			ShareableNames.Add(Settings);
		}
	}
};
