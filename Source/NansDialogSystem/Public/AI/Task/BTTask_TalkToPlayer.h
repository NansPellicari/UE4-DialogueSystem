// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/BTTaskNode.h"
#include "CoreMinimal.h"

#include "BTTask_TalkToPlayer.generated.h"

class UDialogHUD;
class UButton;

/**
 *
 */
UCLASS()
class NANSDIALOGSYSTEM_API UBTTask_TalkToPlayer : public UBTTaskNode
{
	GENERATED_BODY()

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
	UFUNCTION()
	void OnQuestionEnd();

	virtual FString GetStaticDescription() const override;

#if WITH_EDITOR
	virtual FName GetNodeIconName() const override;
#endif	  // WITH_EDITOR

protected:
	UPROPERTY(EditInstanceOnly, Category = "HUD")
	FName HUDName = FName("HUD");

	UPROPERTY(EditInstanceOnly, Category = "Message", meta = (MultiLine = true))
	FText Message;

	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

private:
	UPROPERTY()
	UBehaviorTreeComponent* OwnerComponent = nullptr;
	UPROPERTY()
	UDialogHUD* HUD = nullptr;
};
