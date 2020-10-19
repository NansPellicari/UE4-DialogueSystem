// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/BTService.h"
#include "CoreMinimal.h"

#include "BTService_ClearDialog.generated.h"

/**
 *
 */
UCLASS()
class NANSDIALOGSYSTEM_API UBTService_ClearDialog : public UBTService
{
	GENERATED_BODY()

public:
	UPROPERTY(EditInstanceOnly, Category = "Blackboard")
	FName PointsHandlerKeyName = FName("PointsHandler");

	UBTService_ClearDialog(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;

#if WITH_EDITOR
	virtual FName GetNodeIconName() const override;
#endif	  // WITH_EDITOR
};
