// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIModule\Classes\BehaviorTree\Services\BTService_BlueprintBase.h"
#include "CoreMinimal.h"

#include "BTService_DialogPointsHandler.generated.h"

class UBTDialogPointsHandler;

/**
 *
 */
UCLASS(BlueprintType)
class NANSDIALOGSYSTEM_API UBTService_DialogPointsHandler : public UBTService_BlueprintBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditInstanceOnly, Category = "Blackboard")
	FName PointsHandlerKeyName = FName("PointsHandler");

	UPROPERTY(EditInstanceOnly, Category = "Blackboard")
	FName StepsKeyName = FName("Steps");

	UBTService_DialogPointsHandler(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;

#if WITH_EDITOR
	virtual FName GetNodeIconName() const override;
#endif	  // WITH_EDITOR

protected:
	UPROPERTY(BlueprintReadOnly)
	UBTDialogPointsHandler* BTDialogPointsHandler;
};
