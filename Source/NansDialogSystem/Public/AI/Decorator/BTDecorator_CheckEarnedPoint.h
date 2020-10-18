// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/BTDecorator.h"
#include "CoreMinimal.h"
#include "PointSystemHelpers.h"

#include "BTDecorator_CheckEarnedPoint.generated.h"

class UNansComparator;
class UBTDialogPointsHandler;

/**
 *
 */
UCLASS()
class NANSDIALOGSYSTEM_API UBTDecorator_CheckEarnedPoint : public UBTDecorator
{
	GENERATED_UCLASS_BODY()

	// Basically conditions are check "first at last" with AND operator, use "ConditionsOperators" if you want more
	// This allow to check values already earned in this Behavior Tree
	UPROPERTY(EditAnywhere, Category = "Condition")
	TArray<FPointCondition> EarnedPointsConditions;

	// Be worried! Use this only if you don't want an \"first at last\" AND operator on StepConditions
	UPROPERTY(EditAnywhere, Category = "Condition")
	TArray<FNansConditionOperator> ConditionsOperators;

	UPROPERTY(EditInstanceOnly, Category = "Blackboard")
	FName PointsHandlerKeyName = FName("PointsHandler");

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	bool EvaluateArray(UBTDialogPointsHandler* PointsHandler) const;
	virtual FString GetStaticDescription() const override;

private:
	UNansComparator* Comparator;
};
