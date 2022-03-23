// Copyright 2020-present Nans Pellicari (nans.pellicari@gmail.com).
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
#include "BehaviorTree/BTDecorator.h"
#include "Dialogue/DialogueHistorySearch.h"
#include "Misc/NansComparatorHelpers.h"

#include "BTDecorator_CheckResponsePosition.generated.h"

class UNansComparator;
class UBTDialoguePointsHandler;

/**
 * This is a structure to create a condition to check what was the response position
 */
USTRUCT(BlueprintType)
struct NANSDIALOGUESYSTEM_API FResponsePositionCondition
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Response Position Condition")
	int32 Step = 0;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Response Position Condition")
	FName StepLabel = NAME_None;

	/**
	 * If checked will compare on each responses given in the desired step.
	 * it can be usefull if you plan to redo several times a step.
	 * Otherwise it will checked only the last results.
	 */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Response Position Condition")
	bool bInEveryInstances = false;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Response Position Condition")
	ENConditionComparator Operator = ENConditionComparator::Equals;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Response Position Condition")
	int32 Position = 0;

	static void ToDialogueHistorySearch(const TArray<FResponsePositionCondition> ResponsePositions,
		TArray<FNansConditionOperator> ConditionsOperators,
		TArray<FNDialogueHistorySearch>& Searches,
		TArray<FNansConditionOperator>& Operators);
};

/**
 *
 */
UCLASS()
class NANSDIALOGUESYSTEM_API UBTDecorator_CheckResponsePosition : public UBTDecorator
{
	GENERATED_UCLASS_BODY()
	// Basically conditions are check "first at last" with AND operator, use "ConditionsOperators" if you want more
	// This allow to check values already earned in this Behavior Tree
	UPROPERTY(EditAnywhere, Category = "Condition")
	TArray<FResponsePositionCondition> ResponsePositionConditions;

	// Be worried! Use this only if you don't want an \"first at last\" AND operator on StepConditions
	UPROPERTY(EditAnywhere, Category = "Condition")
	TArray<FNansConditionOperator> ConditionsOperators;

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	bool EvaluateArray(UBTDialoguePointsHandler* PointsHandler) const;
	virtual FString GetStaticDescription() const override;
};
