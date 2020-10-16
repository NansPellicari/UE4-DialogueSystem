// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Attribute/ResponseCategory.h"
#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "PointSystemHelpers.h"

#include "DialogSystemSettings.generated.h"

USTRUCT(BlueprintType)
struct FNDialogDifficultySettings
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Difficulty)
	FNResponseCategory Category;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Difficulty)
	float Multiplier;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Difficulty)
	float RangeFrom;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Difficulty)
	float RangeTo;
};

USTRUCT(BlueprintType)
struct FNDialogResponseCategorySettings
{
	GENERATED_USTRUCT_BODY()

	// TODO change this to get a FactorFactory
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ResponseCategory)
	FName FactorName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ResponseCategory)
	FName Name;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ResponseCategory)
	FLinearColor Color;
};

/**
 *
 */
UCLASS(config = Game, defaultconfig, meta = (DisplayName = "Dialog System"))
class NANSDIALOGSYSTEM_API UDialogSystemSettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	UPROPERTY(config, EditAnywhere)
	TArray<FNDialogResponseCategorySettings> ResponseCategorySettings;
	UPROPERTY(config, EditAnywhere)
	TArray<FNDialogDifficultySettings> DifficultySettings;

	static void GetDifficultyConfigs(TArray<FNDialogDifficultySettings>& ShareableNames)
	{
		static const UDialogSystemSettings* StaticObject = GetDefault<UDialogSystemSettings>();
		for (FNDialogDifficultySettings Settings : StaticObject->DifficultySettings)
		{
			ShareableNames.Add(Settings);
		}
	};
	static void GetResponseCategoryConfigs(TArray<FNDialogResponseCategorySettings>& ShareableNames)
	{
		static const UDialogSystemSettings* StaticObject = GetDefault<UDialogSystemSettings>();
		for (auto& Settings : StaticObject->ResponseCategorySettings)
		{
			ShareableNames.Add(Settings);
		}
	};
};
