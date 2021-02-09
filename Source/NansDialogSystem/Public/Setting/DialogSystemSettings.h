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
#include "BTDialogueTypes.h"
#include "Attribute/FactorAttribute.h"
#include "Engine/DeveloperSettings.h"
#include "NansCoreHelpers/Public/Math/MathTypes.h"

#include "DialogSystemSettings.generated.h"

USTRUCT()
struct NANSDIALOGSYSTEM_API FNDialogFactorSettings
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = Difficulty)
	FNResponseCategory Category;
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
struct NANSDIALOGSYSTEM_API FNDialogResponseCategorySettings
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = ResponseCategory)
	FGameplayTag Name;

	UPROPERTY(EditAnywhere, Category = ResponseCategory)
	FLinearColor Color;

	UPROPERTY(EditAnywhere, Category = ResponseCategory)
	TArray<FNDialogFactorTypeSettings> Factors;

	static FNDialogResponseCategorySettings* CreateNullInstance()
	{
		FNDialogResponseCategorySettings* NullInst = new FNDialogResponseCategorySettings();
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
	UPROPERTY(GlobalConfig, EditAnywhere)
	FFactorAttribute PointsCollector;
	UPROPERTY(GlobalConfig, EditAnywhere)
	TArray<FNDialogResponseCategorySettings> ResponseCategorySettings;
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

	void GetPointsMultipliersConfigs(TMap<FGameplayTag, TArray<FNDialogFactorTypeSettings>>& Confs) const
	{
		for (FNDialogResponseCategorySettings Settings : ResponseCategorySettings)
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

	void GetResponseCategoryConfigs(TArray<FNDialogResponseCategorySettings>& ShareableNames) const
	{
		for (FNDialogResponseCategorySettings Settings : ResponseCategorySettings)
		{
			ShareableNames.Add(Settings);
		}
	}
};
