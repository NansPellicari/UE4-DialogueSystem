// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AI/BehaviorTree/Tasks/BTTask_Base.h"
#include "CoreMinimal.h"

#include "BTTask_UpdateDialogueStep.generated.h"

class UBTStepsWithPoints;
class UBTDialogueResponseContainer;

/**
 *
 */
UCLASS()
class NANSDIALOGSYSTEM_API UBTTask_UpdateDialogueStep : public UBTTask_Base
{
	GENERATED_BODY()

	UPROPERTY(EditInstanceOnly, Category = "Blackboard")
	FName ResponseContainerName = FName("ResponseContainer");

	UPROPERTY(EditInstanceOnly, Category = "Blackboard")
	FName StepsKeyName = FName("Steps");

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;

#if WITH_EDITOR
	virtual FName GetNodeIconName() const override;
#endif	  // WITH_EDITOR

private:
	UPROPERTY()
	UBTStepsWithPoints* BTSteps;
	UPROPERTY()
	UBTDialogueResponseContainer* ResponseContainer;
};
