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

#include "AI/Decorator/BTDecorator_CheckEarnedPoint.h"

#include "BTDialogueTypes.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NansUE4Utilities/public/Misc/ErrorUtils.h"
#include "NansUE4Utilities/public/Misc/TextLibrary.h"
#include "Service/BTDialogPointsHandler.h"
#include "Service/NansComparator.h"

#define LOCTEXT_NAMESPACE "DialogSystem"

void FPointCondition::ToDialogueHistorySearch(const TArray<FPointCondition> ResponsePositions,
	TArray<FNansConditionOperator> ConditionsOperators, TArray<FNDialogueHistorySearch>& Searches,
	TArray<FNansConditionOperator>& Operators)
{
	int32 Index = 0;
	for (auto& RespPos : ResponsePositions)
	{
		FNDialogueHistorySearch Search, Search2;

		Search.DialogName.bIsAll = true;
		Search2.DialogName.bIsAll = true;

		Search.PropertyName = ENPropertyValue::InitialPoints;
		Search.Operator = RespPos.Operator;
		Search.FloatValue = RespPos.CompareTo;

		Search2.PropertyName = ENPropertyValue::CategoryName;
		Search2.Operator = ENansConditionComparator::Equals;
		Search2.CategoryValue = RespPos.PointType;
		Searches.Add(Search);
		Searches.Add(Search2);
		FNansConditionOperator Operator;
		Operator.Operand1 = UNansComparator::BuildKeyFromIndex(Index);
		Operator.Operand2 = UNansComparator::BuildKeyFromIndex(Index + 1);
		Operator.Operator = ENansConditionOperator::AND;
		Operator.Inversed = false;
		Operator.OperatorWithPreviousCondition = ENansConditionOperator::Save;
		Operator.GroupName = FString("InternGrp") + FString::FromInt(Index) + FString("&") +
							 FString::FromInt(Index + 1);
		Operators.Add(Operator);
		bool IsFirst = true;
		for (auto& CondOperator : ConditionsOperators)
		{
			FString StepOpTo = UNansComparator::BuildKeyFromIndex(Index / 2);
			CondOperator.Operand1 = CondOperator.Operand1 == StepOpTo ? Operator.GroupName : CondOperator.Operand1;
			CondOperator.Operand2 = CondOperator.Operand2 == StepOpTo ? Operator.GroupName : CondOperator.Operand2;
			if (IsFirst)
			{
				CondOperator.OperatorWithPreviousCondition = ENansConditionOperator::Save;
			}
			IsFirst = false;
		}
		Index += 2;
	}
	Operators.Append(ConditionsOperators);
}

UBTDecorator_CheckEarnedPoint::UBTDecorator_CheckEarnedPoint(const FObjectInitializer& ObjectInitializer) : Super(
	ObjectInitializer
)
{
	NodeName = "Conditions with earned points";
}

bool UBTDecorator_CheckEarnedPoint::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory) const
{
	const UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	UBTDialogPointsHandler* PointsHandler = Cast<UBTDialogPointsHandler>(
		BlackboardComp->GetValueAsObject(PointsHandlerKeyName)
	);

	if (PointsHandler == nullptr)
	{
		EDITOR_ERROR(
			"DialogSystem",
			LOCTEXT("InvalidPointsHandlerKey", "Invalid key for PointsHandler in "),
			reinterpret_cast<UObject*>(OwnerComp.GetCurrentTree())
		);
		return false;
	}

	TArray<FNDialogueHistorySearch> Searches;
	TArray<FNansConditionOperator> Operators;

	FPointCondition::ToDialogueHistorySearch(
		EarnedPointsConditions,
		ConditionsOperators,
		Searches,
		Operators
	);

	return PointsHandler->HasResults(Searches, Operators);
}

FString UBTDecorator_CheckEarnedPoint::GetStaticDescription() const
{
	FString ReturnDesc = Super::GetStaticDescription();

	ReturnDesc += FString("\n\nEarned Points conditions:");
	for (int32 Index = 0; Index != EarnedPointsConditions.Num(); ++Index)
	{
		FPointCondition Condition = EarnedPointsConditions[Index];

		ReturnDesc += FString::Printf(
			TEXT("\n%s %s %s %d"),
			*UNansComparator::BuildKeyFromIndex(Index),
			*Condition.PointType.Name.ToString(),
			*UNansComparator::ComparatorToString(Condition.Operator),
			Condition.CompareTo
		);
	}

	if (ConditionsOperators.Num() <= 0) return ReturnDesc;

	ReturnDesc += FString("\n\nConditions Operators:");
	ReturnDesc += UNansComparator::OperatorsToString(ConditionsOperators);

	return ReturnDesc;
}
#undef LOCTEXT_NAMESPACE
