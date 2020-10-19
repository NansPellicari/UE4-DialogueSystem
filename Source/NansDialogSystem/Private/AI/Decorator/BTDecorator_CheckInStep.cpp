// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Decorator/BTDecorator_CheckInStep.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Factor/DialogFactorUnit.h"
#include "Logging/MessageLog.h"
#include "NansUE4Utilities/public/Misc/ErrorUtils.h"
#include "NansUE4Utilities/public/Misc/TextLibrary.h"
#include "Runtime/CoreUObject/Public/Misc/UObjectToken.h"
#include "Service/BTDialogPointsHandler.h"
#include "Service/NansComparator.h"

#define LOCTEXT_NAMESPACE "DialogSystem"

UBTDecorator_CheckInStep::UBTDecorator_CheckInStep(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = "Conditions with points in step";
	Comparator = ObjectInitializer.CreateDefaultSubobject<UNansComparator>(this, TEXT("Comparator"));
}

FString UBTDecorator_CheckInStep::GetStaticDescription() const
{
	FString ReturnDesc = Super::GetStaticDescription();

	ReturnDesc += FString("\n\nStep conditions:");
	for (int32 Index = 0; Index != StepConditions.Num(); ++Index)
	{
		FBTStepCondition Condition = StepConditions[Index];

		ReturnDesc += FString::Printf(TEXT("\nC%d: Step %d is Done [%s] - %s %s %d"),
			Index,
			Condition.Step,
			Condition.isDone ? "x" : " ",
			*Condition.CategoryPoint.Name.ToString(),
			*UNansComparator::ComparatorToString(Condition.Operator),
			Condition.CompareTo);
	}

	if (ConditionsOperators.Num() <= 0) return ReturnDesc;

	ReturnDesc += FString("\n\nConditions Operators:");
	ReturnDesc += Comparator->OperatorsToString(ConditionsOperators);

	return ReturnDesc;
}

bool UBTDecorator_CheckInStep::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	UBTDialogPointsHandler* PointsHandler = Cast<UBTDialogPointsHandler>(BlackboardComp->GetValueAsObject(PointsHandlerKeyName));

	if (PointsHandler == nullptr)
	{
		EDITOR_ERROR("DialogSystem",
			LOCTEXT("InvalidPointsHandlerKey", "Invalid key for PointsHandler in "),
			(UObject*) OwnerComp.GetCurrentTree());
		return false;
	}

	return EvaluateArray(PointsHandler);
}

bool UBTDecorator_CheckInStep::EvaluateArray(UBTDialogPointsHandler* PointsHandler) const
{
	bool HasConditionsOperator = ConditionsOperators.Num() > 0;
	TMap<FString, BoolStruct*> ConditionsResults;

	for (int32 Index = 0; Index != StepConditions.Num(); ++Index)
	{
		FBTStepCondition Condition = StepConditions[Index];
		FString SIndex = Comparator->BuildKeyFromIndex(Index);
		ConditionsResults.Add(SIndex, new BoolStruct(EvaluateCondition(PointsHandler, Condition)));

		if (ConditionsResults.FindRef(SIndex)->value == false)
		{
			if (HasConditionsOperator)
			{
				continue;
			}
			else
			{
				break;
			};
		}
	}

	if (HasConditionsOperator == false)
	{
		FString Key = Comparator->BuildKeyFromIndex(ConditionsResults.Num() - 1);
		return ConditionsResults.FindRef(Key)->value;
	}
	return Comparator->EvaluateOperators(ConditionsOperators, ConditionsResults);
}

bool UBTDecorator_CheckInStep::EvaluateCondition(UBTDialogPointsHandler* PointsHandler, FBTStepCondition Condition) const
{
	if (Condition.Step == 0)
	{
		EDITOR_ERROR(
			"DialogSystem", LOCTEXT("InvalidStepNumberInStepsCondition", "You need to set a step numnber to allow comparison in "));
		return false;
	}

	UNDialogFactorUnit* FactorUnit = PointsHandler->GetLastResponseFromStep(Condition.Step);

	if (FactorUnit->Step <= 0)
	{
		return Condition.isDone ? false : true;
	}

	if (FactorUnit->CategoryName != Condition.CategoryPoint.Name)
	{
		return false;
	}

	return Comparator->EvaluateComparator<int32>(Condition.Operator, FactorUnit->InitialPoint, Condition.CompareTo);
}

#undef LOCTEXT_NAMESPACE
