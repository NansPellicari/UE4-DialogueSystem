#pragma once

#include "BTDialogueTypes.h"
#include "NansCommon/Public/Service/NansComparatorHelpers.h"

#include "PointSystemHelpers.generated.h"

/**
 * Points Structure
 */
USTRUCT(BlueprintType)
struct NANSDIALOGSYSTEM_API FPoint
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Point")
	int32 Point = 0;
	UPROPERTY(BlueprintReadWrite, Category = "Point")
	FNResponseCategory Category;
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
