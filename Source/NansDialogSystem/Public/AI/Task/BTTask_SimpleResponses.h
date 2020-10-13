// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AI/Task/BTTask_Responses.h"
#include "CoreMinimal.h"

#include "BTTask_SimpleResponses.generated.h"

/**
 *
 */
UCLASS()
class NANSDIALOGSYSTEM_API UBTTask_SimpleResponses : public UBTTask_Responses
{
	GENERATED_BODY()

	UBTTask_SimpleResponses(const FObjectInitializer& objectInitializer);

public:
#if WITH_EDITOR
	virtual FName GetNodeIconName() const override;
#endif	  // WITH_EDITOR

private:
	virtual void ReceiveOnTick(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
