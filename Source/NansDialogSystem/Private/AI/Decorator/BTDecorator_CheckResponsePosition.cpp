// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Decorator/BTDecorator_CheckResponsePosition.h"

#include "BehaviorTree/BehaviorTreeTypes.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NansUE4Utilities/public/Misc/ErrorUtils.h"
#include "Service/BTDialogPointsHandler.h"
#include "Service/NansComparator.h"

#define LOCTEXT_NAMESPACE "DialogSystem"

UBTDecorator_CheckResponsePosition::UBTDecorator_CheckResponsePosition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = "Conditions on response order";
	Comparator = ObjectInitializer.CreateDefaultSubobject<UNansComparator>(this, TEXT("Comparator"));
}

bool UBTDecorator_CheckResponsePosition::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
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

bool UBTDecorator_CheckResponsePosition::EvaluateArray(UBTDialogPointsHandler* PointsHandler) const
{
	bool HasConditionsOperator = ConditionsOperators.Num() > 0;
	TMap<FString, BoolStruct*> ConditionsResults;

	for (int32 Index = 0; Index < ResponsePositionConditions.Num(); ++Index)
	{
		FResponsePositionCondition ResponseCondition = ResponsePositionConditions[Index];

		FBTPointInStep PointInStep;
		PointsHandler->getLastResponseFromStep(ResponseCondition.Step, PointInStep);

		if (PointInStep.Step <= 0)
		{
			return false;
		}

		bool Results =
			Comparator->EvaluateComparator<int32>(ResponseCondition.Operator, PointInStep.Position, ResponseCondition.Position);
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

	// DEBUG to remove
	/*for (TPair<FString, BoolStruct*> Result : ConditionsResults) {
		UE_LOG(LogTemp, Warning, TEXT("Result: %s %i"), *Result.Key, Result.Value->value);
	}*/

	return Comparator->EvaluateOperators(ConditionsOperators, ConditionsResults);
}

FString UBTDecorator_CheckResponsePosition::GetStaticDescription() const
{
	FString ReturnDesc = Super::GetStaticDescription();

	ReturnDesc += FString("\n\nResponse position conditions:");
	for (int32 Index = 0; Index != ResponsePositionConditions.Num(); ++Index)
	{
		FResponsePositionCondition Condition = ResponsePositionConditions[Index];

		ReturnDesc += FString::Printf(TEXT("\nC%d Step %d, response %s %d"),
			Index,
			Condition.Step,
			*UNansComparator::ComparatorToString(Condition.Operator),
			Condition.Position);
	}

	if (ConditionsOperators.Num() <= 0) return ReturnDesc;

	ReturnDesc += FString("\n\nConditions Operators:");
	ReturnDesc += Comparator->OperatorsToString(ConditionsOperators);

	return ReturnDesc;
}

#undef LOCTEXT_NAMESPACE
