// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/BTService.h"
#include "CoreMinimal.h"

#include "BTService_AddPointsAndResponses.generated.h"

class UBTDialogPointsHandler;
class UBTDialogueResponseContainer;

/**
 *
 */
UCLASS()
class NANSDIALOGSYSTEM_API UBTService_AddPointsAndResponses : public UBTService
{
	GENERATED_BODY()

	UPROPERTY(EditInstanceOnly, Category = "Blackboard")
	FName ResponseContainerName = FName("ResponseContainer");

	UPROPERTY(EditInstanceOnly, Category = "Blackboard")
	FName PointsHandlerKeyName = FName("PointsHandler");

	UBTService_AddPointsAndResponses(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;

#if WITH_EDITOR
	virtual FName GetNodeIconName() const override;
#endif	  // WITH_EDITOR

private:
	UPROPERTY()
	UBTDialogPointsHandler* PointsHandler;
	UPROPERTY()
	UBTDialogueResponseContainer* ResponseContainer;
};
