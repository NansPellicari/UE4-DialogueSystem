// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Decorator/BTDecorator_CheckEarnedPoint.h"

#include "BTStepsWithPoints.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NansUE4Utilities/public/Misc/ErrorUtils.h"
#include "NansUE4Utilities/public/Misc/TextLibrary.h"
#include "Service/NansComparator.h"

#define LOCTEXT_NAMESPACE "DialogSystem"

UBTDecorator_CheckEarnedPoint::UBTDecorator_CheckEarnedPoint(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = "Conditions with earned points";
	Comparator = ObjectInitializer.CreateDefaultSubobject<UNansComparator>(this, TEXT("Comparator"));
}

bool UBTDecorator_CheckEarnedPoint::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	UBTStepsWithPoints* BTSteps = Cast<UBTStepsWithPoints>(BlackboardComp->GetValueAsObject(StepsKeyName));

	if (BTSteps == nullptr)
	{
		EDITOR_ERROR("DialogSystem", LOCTEXT("InvalidStepsKey", "Invalid key for Steps in "), (UObject*) OwnerComp.GetCurrentTree());
		return false;
	}

	return EvaluateArray(BTSteps);
}

bool UBTDecorator_CheckEarnedPoint::EvaluateArray(UBTStepsWithPoints* StepsContext) const
{
	bool HasConditionsOperator = ConditionsOperators.Num() > 0;
	TMap<FString, BoolStruct*> ConditionsResults;

	for (int32 Index = 0; Index < EarnedPointsConditions.Num(); ++Index)
	{
		FPointCondition PointCondition = EarnedPointsConditions[Index];
		if (PointCondition.PointType == EAlignment::None)
		{
			ConditionsResults.Add(Comparator->BuildKeyFromIndex(Index), new BoolStruct(false));
			continue;
		}

		int32 Points = StepsContext->GetPoints(PointCondition.PointType);
		bool Results = Comparator->EvaluateComparator<int32>(PointCondition.Operator, Points, PointCondition.CompareTo);
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

		ReturnDesc += FString::Printf(TEXT("\nC%d %s %s %d"),
			Index,
			*EnumToString(EAlignment, Condition.PointType),
			*UNansComparator::ComparatorToString(Condition.Operator),
			Condition.CompareTo);
	}

	if (ConditionsOperators.Num() <= 0) return ReturnDesc;

	ReturnDesc += FString("\n\nConditions Operators:");
	ReturnDesc += Comparator->OperatorsToString(ConditionsOperators);

	return ReturnDesc;
}
#undef LOCTEXT_NAMESPACE
