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

#include "AI/Decorator/BTDecorator_CheckResponsePosition.h"


#include "Step.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Misc/NansComparator.h"
#include "NansUE4Utilities/public/Misc/ErrorUtils.h"
#include "Service/BTDialogPointsHandler.h"

#define LOCTEXT_NAMESPACE "DialogSystem"

void FResponsePositionCondition::ToDialogueHistorySearch(const TArray<FResponsePositionCondition> ResponsePositions,
	TArray<FNansConditionOperator> ConditionsOperators, TArray<FNDialogueHistorySearch>& Searches,
	TArray<FNansConditionOperator>& Operators)
{
	for (auto& RespPos : ResponsePositions)
	{
		FNDialogueHistorySearch Search;

		FNStep Step(RespPos.Step, RespPos.StepLabel);
		Search.DialogName.SetValue(Step.GetLabel().ToString());
		Search.DialogName.bLastOnly = !RespPos.bInEveryInstances;

		Search.PropertyName = ENPropertyValue::SentencePosition;
		Search.Operator = RespPos.Operator;
		Search.IntValue = RespPos.Position;


		Searches.Add(Search);
	}
	Operators.Append(ConditionsOperators);
}

UBTDecorator_CheckResponsePosition::UBTDecorator_CheckResponsePosition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = "Conditions on response order";
}


bool UBTDecorator_CheckResponsePosition::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
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

	FResponsePositionCondition::ToDialogueHistorySearch(
		ResponsePositionConditions,
		ConditionsOperators,
		Searches,
		Operators
	);

	return PointsHandler->HasResults(Searches, Operators);
}

FString UBTDecorator_CheckResponsePosition::GetStaticDescription() const
{
	FString ReturnDesc = Super::GetStaticDescription();

	ReturnDesc += FString("\n\nResponse position conditions:");
	for (int32 Index = 0; Index != ResponsePositionConditions.Num(); ++Index)
	{
		const FResponsePositionCondition Condition = ResponsePositionConditions[Index];

		ReturnDesc += FString::Printf(
			TEXT("\n%s (%s) Step [%s], response %s %d"),
			*UNansComparator::BuildKeyFromIndex(Index),
			Condition.bInEveryInstances ? TEXT("all") : TEXT("last only"),
			*(!Condition.StepLabel.IsNone() ? Condition.StepLabel.ToString() : FString::FromInt(Condition.Step)),
			*UNansComparator::ComparatorToString(Condition.Operator),
			Condition.Position
		);
	}

	if (ConditionsOperators.Num() <= 0) return ReturnDesc;

	ReturnDesc += FString("\n\nConditions Operators:");
	ReturnDesc += UNansComparator::OperatorsToString(ConditionsOperators);

	return ReturnDesc;
}

#undef LOCTEXT_NAMESPACE
