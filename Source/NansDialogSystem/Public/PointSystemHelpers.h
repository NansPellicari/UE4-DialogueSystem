#pragma once

#include "NansCommon/Public/Service/NansComparatorHelpers.h"

#include "PointSystemHelpers.generated.h"

UENUM(BlueprintType)
enum class EAlignment : uint8
{
	None,
	CNV,
	Neutral,
	CSV
};

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
	EAlignment Alignment = EAlignment::Neutral;
};

/**
 * Condition for point
 */
USTRUCT(BlueprintType)
struct NANSDIALOGSYSTEM_API FPointCondition
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Point Condition")
	EAlignment PointType;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Point Condition")
	ENansConditionComparator Operator;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Point Condition")
	int32 CompareTo;
};
