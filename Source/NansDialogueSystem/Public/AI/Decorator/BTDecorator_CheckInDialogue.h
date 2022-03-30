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
#include "BTDialogueTypes.h"
#include "BehaviorTree/BTDecorator.h"
#include "Dialogue/DialogueHistorySearch.h"
#include "Misc/NansComparatorHelpers.h"

#include "BTDecorator_CheckInDialogue.generated.h"

class UNansComparator;
class UBTDialoguePointsHandler;
class UBlackboardComponent;

/**
 * This is a structure to create a condition against a step
 */
USTRUCT(BlueprintType)
struct NANSDIALOGUESYSTEM_API FBTDialogueCondition
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Dialogue Condition")
	bool isDone = true;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Dialogue Condition")
	FName DialogueLabel = NAME_None;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Dialogue Condition")
	FNDialogueCategory Category;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Dialogue Condition")
	ENConditionComparator Operator = ENConditionComparator::Equals;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Dialogue Condition")
	int32 CompareTo = 0.f;

	static void ToDialogueHistorySearch(const TArray<FBTDialogueCondition> DialogueConditions,
		TArray<FNansConditionOperator> ConditionsOperators, TArray<FNDialogueHistorySearch>& Searches,
		TArray<FNansConditionOperator>& Operators);
};

/**
 *
 */
UCLASS()
class NANSDIALOGUESYSTEM_API UBTDecorator_CheckInDialogue : public UBTDecorator
{
	GENERATED_UCLASS_BODY()
	// Basically conditions are check "first at last" with AND operator, use "ConditionsOperators" if you want more
	UPROPERTY(EditAnywhere, Category = "Condition")
	TArray<FBTDialogueCondition> DialogueConditions;

	// Be worried! Use this only if you don't want an \"first at last\" AND operator on DialogueConditions
	UPROPERTY(EditAnywhere, Category = "Condition")
	TArray<FNansConditionOperator> ConditionsOperators;

	virtual FString GetStaticDescription() const override;

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};