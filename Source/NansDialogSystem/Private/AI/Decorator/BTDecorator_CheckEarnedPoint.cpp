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

UBTDecorator_CheckEarnedPoint::UBTDecorator_CheckEarnedPoint(const FObjectInitializer& ObjectInitializer) : Super(
	ObjectInitializer
)
{
	NodeName = "Conditions with earned points";
	Comparator = ObjectInitializer.CreateDefaultSubobject<UNansComparator>(this, TEXT("Comparator"));
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

	return EvaluateArray(PointsHandler);
}

bool UBTDecorator_CheckEarnedPoint::EvaluateArray(UBTDialogPointsHandler* PointsHandler) const
{
	const bool HasConditionsOperator = ConditionsOperators.Num() > 0;
	TMap<FString, BoolStruct*> ConditionsResults;

	for (int32 Index = 0; Index < EarnedPointsConditions.Num(); ++Index)
	{
		FPointCondition PointCondition = EarnedPointsConditions[Index];
		if (!PointCondition.PointType.Name.IsValid())
		{
			ConditionsResults.Add(Comparator->BuildKeyFromIndex(Index), new BoolStruct(false));
			continue;
		}

		const int32 Points = PointsHandler->GetDialogPoints(PointCondition.PointType);
		const bool Results = Comparator->EvaluateComparator<int32>(
			PointCondition.Operator,
			Points,
			PointCondition.CompareTo
		);
		ConditionsResults.Add(Comparator->BuildKeyFromIndex(Index), new BoolStruct(Results));

		if (HasConditionsOperator == false && Results == false)
		{
			break;
		}
	}

	if (HasConditionsOperator == false)
	{
		FString Key = Comparator->BuildKeyFromIndex(ConditionsResults.Num() - 1);
		return ConditionsResults.FindRef(Key)->value;
	}

	return Comparator->EvaluateOperators(ConditionsOperators, ConditionsResults);
}

FString UBTDecorator_CheckEarnedPoint::GetStaticDescription() const
{
	FString ReturnDesc = Super::GetStaticDescription();

	ReturnDesc += FString("\n\nEarned Points conditions:");
	for (int32 Index = 0; Index != EarnedPointsConditions.Num(); ++Index)
	{
		FPointCondition Condition = EarnedPointsConditions[Index];

		ReturnDesc += FString::Printf(
			TEXT("\nC%d %s %s %d"),
			Index,
			*Condition.PointType.Name.ToString(),
			*UNansComparator::ComparatorToString(Condition.Operator),
			Condition.CompareTo
		);
	}

	if (ConditionsOperators.Num() <= 0) return ReturnDesc;

	ReturnDesc += FString("\n\nConditions Operators:");
	ReturnDesc += Comparator->OperatorsToString(ConditionsOperators);

	return ReturnDesc;
}
#undef LOCTEXT_NAMESPACE
