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
#include "BehaviorTree/BTDecorator.h"
#include "Dialogue/DialogueHistorySearch.h"
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

	static void ToDialogueHistorySearch(const TArray<FBTStepCondition> Steps,
		TArray<FNansConditionOperator> ConditionsOperators,
		TArray<FNDialogueHistorySearch>& Searches,
		TArray<FNansConditionOperator>& Operators);
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
};
