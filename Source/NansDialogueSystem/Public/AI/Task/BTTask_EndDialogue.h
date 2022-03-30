// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_EndDialogue.generated.h"

class UBehaviorTreeComponent;

/**
 * 
 */
UCLASS()
class NANSDIALOGUESYSTEM_API UBTTask_EndDialogue : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_EndDialogue(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

#if WITH_EDITOR
	virtual FName GetNodeIconName() const override;
#endif	  // WITH_EDITOR
};
