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

#include "AI/Decorator/BTDecorator_CheckEarnedPoint.h"

#include "BTDialogueTypes.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Misc/NansComparator.h"
#include "NansUE4Utilities/public/Misc/ErrorUtils.h"
#include "Service/BTDialoguePointsHandler.h"
#include "Setting/DialogueSystemSettings.h"

#define LOCTEXT_NAMESPACE "DialogueSystem"

void FPointCondition::ToDialogueHistorySearch(const TArray<FPointCondition> ResponsePositions,
	TArray<FNansConditionOperator> ConditionsOperators, TArray<FNDialogueHistorySearch>& Searches,
	TArray<FNansConditionOperator>& Operators)
{
	int32 Index = 0;
	TMap<int32, int32> IndexesMatches;
	for (auto& RespPos : ResponsePositions)
	{
		FNDialogueHistorySearch Search, Search2;

		Search.DialogueName.bIsAll = true;
		Search2.DialogueName.bIsAll = true;

		Search.PropertyName = ENPropertyValue::CategoryName;
		Search.Operator = ENansConditionComparator::Equals;
		Search.CategoryValue = RespPos.PointType;

		Search2.PropertyName = ENPropertyValue::InitialPoints;
		Search2.Operator = RespPos.Operator;
		Search2.FloatValue = RespPos.CompareTo;
		Searches.Add(Search);
		Searches.Add(Search2);

		ENansConditionOperator OpForInterGrp = ENansConditionOperator::AND;
		TArray<FNansConditionOperator> NewOperators;
		int32 OpIdx = 0;

		int32 RealIdx = !IndexesMatches.Contains(Index) ? 0 : IndexesMatches[Index];
		int32 RealNewIdx = RealIdx + 1;

		FString GroupName = FString("InternGrp") + FString::FromInt(RealIdx) + FString("&") +
							FString::FromInt(RealIdx + 1);
		NewOperators.Add(FNansConditionOperator());
		NewOperators[OpIdx].Operand1 = UNansComparator::BuildKeyFromIndex(RealIdx);
		NewOperators[OpIdx].Operand2 = UNansComparator::BuildKeyFromIndex(RealIdx + 1);
		NewOperators[OpIdx].Operator = ENansConditionOperator::AND;
		NewOperators[OpIdx].Inversed = false;
		NewOperators[OpIdx].OperatorWithPreviousCondition = ENansConditionOperator::Save;
		NewOperators[OpIdx].GroupName = GroupName;

		if (RespPos.Operator == ENansConditionComparator::Inferior
			|| RespPos.Operator == ENansConditionComparator::InferiorOrEquals)
		{
			FNDialogueHistorySearch Search3 = Search;
			Search3.bInversed = true;
			Searches.Add(Search3);
			NewOperators.Add(FNansConditionOperator());
			OpIdx++;
			NewOperators[OpIdx].Operand1 = GroupName;
			NewOperators[OpIdx].Operand2 = UNansComparator::BuildKeyFromIndex(RealIdx + 2);
			RealNewIdx++;
			NewOperators[OpIdx].Operator = ENansConditionOperator::OR;
			NewOperators[OpIdx].OperatorWithPreviousCondition = ENansConditionOperator::Save;
			NewOperators[OpIdx].GroupName = GroupName + FString("OrInverse");
		}

		Operators.Append(NewOperators);

		bool IsFirst = true;
		for (auto& CondOperator : ConditionsOperators)
		{
			FString StepOpTo = UNansComparator::BuildKeyFromIndex(Index);
			CondOperator.Operand1 = CondOperator.Operand1 == StepOpTo
										? NewOperators[OpIdx].GroupName // always get the last NewOperators
										: CondOperator.Operand1;
			CondOperator.Operand2 = CondOperator.Operand2 == StepOpTo
										? NewOperators[OpIdx].GroupName // always get the last NewOperators
										: CondOperator.Operand2;
			if (IsFirst)
			{
				CondOperator.OperatorWithPreviousCondition = ENansConditionOperator::Save;
			}
			IsFirst = false;
		}
		IndexesMatches.Add(++Index, ++RealNewIdx);
	}
	Operators.Append(ConditionsOperators);
}

UBTDecorator_CheckEarnedPoint::UBTDecorator_CheckEarnedPoint(const FObjectInitializer& ObjectInitializer) : Super(
	ObjectInitializer
)
{
	NodeName = "Check: Earned points";
}

bool UBTDecorator_CheckEarnedPoint::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory) const
{
	const UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	const auto Settings = UDialogueSystemSettings::Get()->BehaviorTreeSettings;

	UBTDialoguePointsHandler* PointsHandler = Cast<UBTDialoguePointsHandler>(
		BlackboardComp->GetValueAsObject(Settings.PointsHandlerKey)
	);

	if (PointsHandler == nullptr)
	{
		EDITOR_ERROR(
			"DialogueSystem",
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
	FString ReturnDesc;

	ReturnDesc += FString("\nEarned Points conditions:");
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
