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

#include "BTDialogueTypes.h"
#include "NansCommon/Public/Service/NansComparatorHelpers.h"

#include "PointSystemHelpers.generated.h"

/**
 * Points Structure
 */
USTRUCT(BlueprintType)
struct NANSDIALOGSYSTEM_API FNPoint
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Point")
	int32 Point = 0;
	UPROPERTY(BlueprintReadWrite, Category = "Point")
	FNResponseCategory Category;
	UPROPERTY(BlueprintReadWrite, Category = "Point")
	float Difficulty = 0.f;
	UPROPERTY(BlueprintReadWrite, Category = "Point")
	TSubclassOf<UGameplayEffect> EffectOnEarned;
};

/**
 * Condition for point
 */
USTRUCT(BlueprintType)
struct NANSDIALOGSYSTEM_API FPointCondition
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Point Condition")
	FNResponseCategory PointType;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Point Condition")
	ENansConditionComparator Operator;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Point Condition")
	int32 CompareTo;
};
