// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/BTDecorator.h"
#include "CoreMinimal.h"
#include "PointSystemHelpers.h"
#include "Service/NansComparatorHelpers.h"

#include "BTDecorator_CheckResponsePosition.generated.h"

class UNansComparator;
class UBTStepsWithPoints;

/**
 * This is a structure to create a condition to check what was the response position
 */
USTRUCT(BlueprintType)
struct NANSDIALOGSYSTEM_API FResponsePositionCondition
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Response Position Condition")
	int32 Step;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Response Position Condition")
	ENansConditionComparator Operator;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Response Position Condition")
	int32 Position;
};

/**
 *
 */
UCLASS()
class NANSDIALOGSYSTEM_API UBTDecorator_CheckResponsePosition : public UBTDecorator
{
	GENERATED_UCLASS_BODY()

	// Basically conditions are check "first at last" with AND operator, use "ConditionsOperators" if you want more
	// This allow to check values already earned in this Behavior Tree
	UPROPERTY(EditAnywhere, Category = "Condition")
	TArray<FResponsePositionCondition> ResponsePositionConditions;

	// Be worried! Use this only if you don't want an \"first at last\" AND operator on StepConditions
	UPROPERTY(EditAnywhere, Category = "Condition")
	TArray<FNansConditionOperator> ConditionsOperators;

	UPROPERTY(EditInstanceOnly, Category = "Blackboard")
	FName StepsKeyName = FName("Steps");

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	bool EvaluateArray(UBTStepsWithPoints* StepsContext) const;
	virtual FString GetStaticDescription() const override;

private:
	UNansComparator* Comparator;
};
