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

#include "GameplayTags.h"
#include "BTDialogueTypes.generated.h"

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

UENUM(BlueprintType, Meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EFactorType : uint8
{
	None = 0 UMETA(Hidden),
	Difficulty = 0x00000001,
	PointsMultiplier = 0x00000002,
};

ENUM_CLASS_FLAGS(EFactorType)

struct FNDialogResponseCategorySettings;
struct FNDialogFactorTypeSettings;

USTRUCT(BlueprintType)
struct NANSDIALOGSYSTEM_API FNResponseCategory
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ResponseCategory")
	FGameplayTag Name;

	FLinearColor GetColor() const;
	TArray<FNDialogFactorTypeSettings> GetFactors(const int32 Type = 0) const;
	const FNDialogResponseCategorySettings& GetConfig() const;
};

/**
 * Dialogue Reponse Struct
 */
USTRUCT(BlueprintType)
struct NANSDIALOGSYSTEM_API FBTDialogueResponse
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Response", meta = (MultiLine = true))
	FText Text;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Response")
	int32 Level = 0;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Response")
	float DifficultyLevel = 0;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Response")
	FNResponseCategory Category;

	static FBTDialogueResponse CreateNullObject()
	{
		FBTDialogueResponse Response = FBTDialogueResponse();
		Response.Text = NSLOCTEXT("DialogSystem", "DefaultDialogueResponseText", "Euh...");
		return Response;
	}
};
