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

#include "AI/Decorator/BTDecorator_CheckInStep.h"

#include "Step.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Misc/NansComparator.h"
#include "NansUE4Utilities/public/Misc/ErrorUtils.h"
#include "Service/BTDialogPointsHandler.h"
#include "Setting/DialogSystemSettings.h"

#define LOCTEXT_NAMESPACE "DialogSystem"

void FBTStepCondition::ToDialogueHistorySearch(const TArray<FBTStepCondition> StepConditions,
	TArray<FNansConditionOperator> ConditionsOperators, TArray<FNDialogueHistorySearch>& Searches,
	TArray<FNansConditionOperator>& Operators)
{
	int32 Index = 0;
	for (auto& StepCondition : StepConditions)
	{
		FNDialogueHistorySearch Search, Search2;
		if (StepCondition.isDone)
		{
			FNStep Step(StepCondition.Step, StepCondition.StepLabel);
			Search.DialogName.SetValue(Step.GetLabel().ToString());
			Search2.DialogName.SetValue(Step.GetLabel().ToString());
		}
		else
		{
			// TODO check if enough, maybe should add a bIsNot
			Search.DialogName.bIsAll = true;
			Search2.DialogName.bIsAll = true;
		}
		Search.PropertyName = ENPropertyValue::InitialPoints;
		Search.Operator = StepCondition.Operator;
		Search.FloatValue = StepCondition.CompareTo;

		Search2.PropertyName = ENPropertyValue::CategoryName;
		Search2.Operator = ENansConditionComparator::Equals;
		Search2.CategoryValue = StepCondition.Category;
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

UBTDecorator_CheckInStep::UBTDecorator_CheckInStep(const FObjectInitializer& ObjectInitializer) : Super(
	ObjectInitializer
)
{
	NodeName = "Check: Points in step";
}

bool UBTDecorator_CheckInStep::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	const auto Settings = UDialogSystemSettings::Get()->BehaviorTreeSettings;
	UBTDialogPointsHandler* PointsHandler = Cast<UBTDialogPointsHandler>(
		BlackboardComp->GetValueAsObject(Settings.PointsHandlerKey)
	);

	if (PointsHandler == nullptr)
	{
		EDITOR_ERROR(
			"DialogSystem",
			LOCTEXT("InvalidPointsHandlerKey", "Invalid key for PointsHandler in "),
			(UObject*) OwnerComp.GetCurrentTree()
		);
		return false;
	}

	TArray<FNDialogueHistorySearch> Searches;
	TArray<FNansConditionOperator> Operators;

	FBTStepCondition::ToDialogueHistorySearch(
		StepConditions,
		ConditionsOperators,
		Searches,
		Operators
	);

	return PointsHandler->HasResults(Searches, Operators);
}

FString UBTDecorator_CheckInStep::GetStaticDescription() const
{
	FString ReturnDesc;

	ReturnDesc += FString("\nStep conditions:");
	for (int32 Index = 0; Index != StepConditions.Num(); ++Index)
	{
		FBTStepCondition Condition = StepConditions[Index];

		ReturnDesc += FString::Printf(
			TEXT("\n%s: Step [%s] is Done [%s] - %s %s %d"),
			*UNansComparator::BuildKeyFromIndex(Index),
			*(!Condition.StepLabel.IsNone() ? Condition.StepLabel.ToString() : FString::FromInt(Condition.Step)),
			Condition.isDone ? "x" : " ",
			*Condition.Category.Name.ToString(),
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
