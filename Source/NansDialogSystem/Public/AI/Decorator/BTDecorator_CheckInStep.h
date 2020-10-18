// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BTDialogueTypes.h"
#include "BehaviorTree/BTDecorator.h"
#include "CoreMinimal.h"
#include "Service/NansComparatorHelpers.h"

#include "BTDecorator_CheckInStep.generated.h"

class UNansComparator;
class UBTDialogPointsHandler;
class UBlackboardComponent;

/**
 * This is a structure to create a condition against a step
 */
USTRUCT(BlueprintType)
struct NANSDIALOGSYSTEM_API FBTStepCondition
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Step Condition")
	bool isDone = true;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Step Condition")
	int32 Step;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Step Condition")
	FNResponseCategory CategoryPoint;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Step Condition")
	ENansConditionComparator Operator;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Step Condition")
	int32 CompareTo;
};

/**
 *
 */
UCLASS()
class NANSDIALOGSYSTEM_API UBTDecorator_CheckInStep : public UBTDecorator
{
	GENERATED_UCLASS_BODY()

	// Basically conditions are check "first at last" with AND operator, use "ConditionsOperators" if you want more
	UPROPERTY(EditAnywhere, Category = "Condition")
	TArray<FBTStepCondition> StepConditions;

	// Be worried! Use this only if you don't want an \"first at last\" AND operator on StepConditions
	UPROPERTY(EditAnywhere, Category = "Condition")
	TArray<FNansConditionOperator> ConditionsOperators;

	UPROPERTY(EditInstanceOnly, Category = "Blackboard")
	FName PointsHandlerKeyName = FName("PointsHandler");

	virtual FString GetStaticDescription() const override;

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

private:
	UNansComparator* Comparator;
	bool EvaluateArray(UBTDialogPointsHandler* PointsHandler) const;
	bool EvaluateCondition(UBTDialogPointsHandler* PointsHandler, FBTStepCondition Condition) const;
};
