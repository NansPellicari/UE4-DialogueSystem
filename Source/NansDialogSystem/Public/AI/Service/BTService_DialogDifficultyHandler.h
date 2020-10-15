// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIModule\Classes\BehaviorTree\Services\BTService_BlueprintBase.h"
#include "CoreMinimal.h"

#include "BTService_DialogDifficultyHandler.generated.h"

class UBTDialogDifficultyHandler;

/**
 *
 */
UCLASS(Blueprintable)
class NANSDIALOGSYSTEM_API UBTService_DialogDifficultyHandler : public UBTService_BlueprintBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditInstanceOnly, Category = "Blackboard")
	FName DifficultyHandlerKeyName = FName("DifficultyHandler");

	UBTService_DialogDifficultyHandler(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;

#if WITH_EDITOR
	virtual FName GetNodeIconName() const override;
#endif	  // WITH_EDITOR

protected:
	UPROPERTY(BlueprintReadOnly)
	UBTDialogDifficultyHandler* BTDialogDifficultyHandler;
};
