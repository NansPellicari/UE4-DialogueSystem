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

#include "AI/Decorator/BTDecorator_CheckInDialogue.h"

#include "AIController.h"
#include "NDialogueSubsystem.h"
#include "NDSFunctionLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Misc/NansComparator.h"
#include "NansUE4Utilities/public/Misc/ErrorUtils.h"
#include "Service/DialoguePointsHandler.h"

#define LOCTEXT_NAMESPACE "DialogueSystem"

void FBTDialogueCondition::ToDialogueHistorySearch(const TArray<FBTDialogueCondition> DialogueConditions,
	TArray<FNansConditionOperator> ConditionsOperators, TArray<FNDialogueHistorySearch>& Searches,
	TArray<FNansConditionOperator>& Operators)
{
	int32 Index = 0;
	for (auto& DialogueCondition : DialogueConditions)
	{
		FNDialogueHistorySearch Search, Search2;
		if (DialogueCondition.isDone)
		{
			Search.DialogueName.SetValue(DialogueCondition.DialogueLabel.ToString());
			Search2.DialogueName.SetValue(DialogueCondition.DialogueLabel.ToString());
		}
		else
		{
			// TODO check if enough, maybe should add a bIsNot
			Search.DialogueName.bIsAll = true;
			Search2.DialogueName.bIsAll = true;
		}
		Search.PropertyName = ENPropertyValue::InitialPoints;
		Search.Operator = DialogueCondition.Operator;
		Search.FloatValue = DialogueCondition.CompareTo;

		Search2.PropertyName = ENPropertyValue::CategoryName;
		Search2.Operator = ENConditionComparator::Equals;
		Search2.CategoryValue = DialogueCondition.Category;
		Searches.Add(Search);
		Searches.Add(Search2);
		FNansConditionOperator Operator;
		Operator.Operand1 = UNansComparator::BuildKeyFromIndex(Index);
		Operator.Operand2 = UNansComparator::BuildKeyFromIndex(Index + 1);
		Operator.Operator = ENConditionOperator::AND;
		Operator.Inversed = false;
		Operator.OperatorWithPreviousCondition = ENConditionOperator::Save;
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
				CondOperator.OperatorWithPreviousCondition = ENConditionOperator::Save;
			}
			IsFirst = false;
		}
		Index += 2;
	}
	Operators.Append(ConditionsOperators);
}

UBTDecorator_CheckInDialogue::UBTDecorator_CheckInDialogue(const FObjectInitializer& ObjectInitializer) : Super(
	ObjectInitializer
)
{
	NodeName = "Points in dialogue";
}

bool UBTDecorator_CheckInDialogue::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory) const
{
	const AAIController* AIOwner = OwnerComp.GetAIOwner();
	check(IsValid(AIOwner));
	UNDialogueSubsystem* DialSys = UNDSFunctionLibrary::GetDialogSubsystem();
	check(IsValid(DialSys));
	const TSharedPtr<NDialoguePointsHandler>& PointsHandler = DialSys->GetPointsHandler(AIOwner);
	check(PointsHandler);

	TArray<FNDialogueHistorySearch> Searches;
	TArray<FNansConditionOperator> Operators;

	FBTDialogueCondition::ToDialogueHistorySearch(
		DialogueConditions,
		ConditionsOperators,
		Searches,
		Operators
	);

	return PointsHandler->HasResults(Searches, Operators);
}

FString UBTDecorator_CheckInDialogue::GetStaticDescription() const
{
	FString ReturnDesc;

	ReturnDesc += FString("\nDialogue conditions:");
	for (int32 Index = 0; Index != DialogueConditions.Num(); ++Index)
	{
		FBTDialogueCondition Condition = DialogueConditions[Index];

		ReturnDesc += FString::Printf(
			TEXT("\n%s: Dialogue [%s] is Done [%s] - %s %s %d"),
			*UNansComparator::BuildKeyFromIndex(Index),
			*Condition.DialogueLabel.ToString(),
			Condition.isDone ? "x" : " ",
			*Condition.Category.Name.ToString(),
			*UNansComparator::ComparatorToString(Condition.Operator),
			Condition.CompareTo
		);
	}

	if (ConditionsOperators.Num() <= 0)
	{
		return ReturnDesc;
	}

	ReturnDesc += FString("\n\nConditions Operators:");
	ReturnDesc += UNansComparator::OperatorsToString(ConditionsOperators);

	return ReturnDesc;
}

#undef LOCTEXT_NAMESPACE
