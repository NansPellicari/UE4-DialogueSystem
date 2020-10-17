// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BTDialogueTypes.h"
#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "PointSystemHelpers.h"

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

	// TODO change this to get a FactorFactory
	UPROPERTY(EditAnywhere, Category = ResponseCategory)
	FName FactorName;
	UPROPERTY(EditAnywhere, Category = ResponseCategory, Meta = (Bitmask, BitmaskEnum = "EFactorType"))
	int32 Type;
};

USTRUCT()
struct NANSDIALOGSYSTEM_API FNDialogResponseCategorySettings
{
	GENERATED_USTRUCT_BODY()

	// TODO change this to get a FactorFactory
	UPROPERTY(EditAnywhere, Category = ResponseCategory)
	TArray<FNDialogFactorTypeSettings> Factors;
	UPROPERTY(EditAnywhere, Category = ResponseCategory)
	FName Name;
	UPROPERTY(EditAnywhere, Category = ResponseCategory)
	FLinearColor Color;

	static FNDialogResponseCategorySettings* CreateNullInstance()
	{
		FNDialogResponseCategorySettings* NullInst = new FNDialogResponseCategorySettings();
		NullInst->Name = NAME_None;
		NullInst->Color = FLinearColor::Black;
		NullInst->Factors.Empty();
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

	void GetPointsEarnerConfigs(TMap<FName, TArray<FNDialogFactorTypeSettings>>& Confs) const
	{
		for (FNDialogResponseCategorySettings Settings : ResponseCategorySettings)
		{
			for (auto& Factor : Settings.Factors)
			{
				if (Factor.Type & (int32) EFactorType::PointsEarner)
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
