// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AI/BehaviorTree/Tasks/BTTask_Base.h"
#include "CoreMinimal.h"

#include "BTTask_CreateHUD.generated.h"

class UNansUserWidget;

/**
 *
 */
UCLASS()
class NANSDIALOGSYSTEM_API UBTTask_CreateHUD : public UBTTask_Base
{
	GENERATED_BODY()

	UPROPERTY(EditInstanceOnly, Category = "HUD")
	FName HUD = FName("HUD");

	UPROPERTY(EditInstanceOnly, Category = "HUD")
	TSubclassOf<UNansUserWidget> HUDToCreate;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;

#if WITH_EDITOR
	virtual FName GetNodeIconName() const override;
#endif	  // WITH_EDITOR
};