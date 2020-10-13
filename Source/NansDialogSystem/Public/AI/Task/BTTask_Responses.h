// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AI/BehaviorTree/Tasks/BTTask_Base.h"
#include "BTDialogueTypes.h"
#include "CoreMinimal.h"

#include "BTTask_Responses.generated.h"

class UDialogHUD;
class UPanelWidget;
class UResponseButtonWidget;
class UWheelProgressBarWidget;
class UBTTask_Countdown;

/**
 * TODO create a mother class "UBTTask_WaitingForUser" which is connected to StepContext event "OnTimeElapsedForUser" which call
 * "OnTaskFinished"
 */
UCLASS(Abstract)
class NANSDIALOGSYSTEM_API UBTTask_Responses : public UBTTask_Base
{
	GENERATED_BODY()

	/**
	 * This checks if each parameters are well set, creates all the buttons, bind event on them.
	 * @see UBTTask_Responses::CreateButtons() for more details
	 */
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

#if WITH_EDITOR
	virtual void PostLoad() override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif	  // WITH_EDITOR

public:
	UBTTask_Responses(const FObjectInitializer& objectInitializer);
	virtual FString GetStaticDescription() const override;

	UFUNCTION()
	void OnButtonClicked(UResponseButtonWidget* DialogueResponse);

	UFUNCTION()
	void OnCountdownEnds(UBehaviorTreeComponent* OwnerComp);

	UFUNCTION()
	void OnEndDisplayResponse();

protected:
	UPROPERTY(EditInstanceOnly, Category = "HUD")
	FName HUD = FName("HUD");

	UPROPERTY(EditInstanceOnly, Category = "HUD")
	TSubclassOf<UResponseButtonWidget> ResponseButtonWidget;

	UPROPERTY(EditInstanceOnly, Category = "HUD")
	FName ResponsesSlotName = FName("ResponsesSlot");

	UPROPERTY(EditInstanceOnly, Category = "HUD")
	FName ButtonAfterName = FName("ButtonAfter");

	UPROPERTY(EditInstanceOnly, Category = "Responses")
	FName ResponseContainerName = FName("ResponseContainer");

	UPROPERTY(EditInstanceOnly, Category = "Responses")
	TArray<FBTDialogueResponse> ReponsesCNV;

	UPROPERTY(EditInstanceOnly, Category = "Responses")
	TArray<FBTDialogueResponse> ReponsesCSV;

	UPROPERTY(EditInstanceOnly, Category = "Responses")
	FText NeutralResponse;

	UPROPERTY(EditInstanceOnly, Category = "Responses")
	int32 NeutralResponsePoint = 1;

	UPROPERTY(EditInstanceOnly, Category = "Countdown")
	int32 TimeToResponse = 0;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

	UPROPERTY()
	UBehaviorTreeComponent* OwnerComponent;
	UPROPERTY()
	UBlackboardComponent* Blackboard;
	UPROPERTY()
	UDialogHUD* DialogHUD;
	UPROPERTY()
	UPanelWidget* ResponsesSlot;
	UPROPERTY()
	UBTTask_Countdown* CountDownTask;
	TMap<FString, int32> ListButtonIndexes;

private:
	int32 NeutralResponseIndex = -1;
	EBTNodeResult::Type ResponseStatus = EBTNodeResult::InProgress;
	void CreateButtons();
	void CreateButton(FBTDialogueResponse Response, int8 Index, int32 MaxLevel);
	FString DisplayStaticResponses(
		const TArray<FBTDialogueResponse>& Responses, int32& Position, FString Title, bool Reverse) const;

	virtual UDialogHUD* GetHUDFromBlackboard(UBlackboardComponent* _Blackboard);
	virtual void ReceiveOnTick(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds);
};
