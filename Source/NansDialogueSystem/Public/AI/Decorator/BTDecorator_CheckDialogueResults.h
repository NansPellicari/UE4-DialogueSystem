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
#include "BTDecorator_CheckDialogueResults.generated.h"

class UNansComparator;
class UBTDialoguePointsHandler;

/**
 *
 */
UCLASS()
class NANSDIALOGUESYSTEM_API UBTDecorator_CheckDialogueResults : public UBTDecorator
{
	GENERATED_UCLASS_BODY()
	UPROPERTY(EditAnywhere, Category = "Condition")
	TArray<FNDialogueHistorySearch> DialogueHistorySearches;

	// Be worried! Use this only if you don't want an \"first at last\" AND operator on Conditions
	UPROPERTY(EditAnywhere, Category = "Condition")
	TArray<FNansConditionOperator> ConditionsOperators;

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual FString GetStaticDescription() const override;
};
