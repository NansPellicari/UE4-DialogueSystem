//  Copyright 2020-present Nans Pellicari (nans.pellicari@gmail.com).
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "CoreMinimal.h"
#include "BTDialogueTypes.h"
#include "AI/BehaviorTree/Tasks/BTTask_Base.h"

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
	UPROPERTY(VisibleAnywhere, Category = "HUD")
	FName UINameKey = NAME_None;

	UPROPERTY(EditInstanceOnly, Category = "HUD")
	TSubclassOf<UResponseButtonWidget> ResponseButtonWidget;

	UPROPERTY(EditInstanceOnly, Category = "HUD")
	FName ResponsesSlotName = FName("ResponsesSlot");

	UPROPERTY(EditInstanceOnly, Category = "HUD")
	FName ButtonAfterName = FName("ButtonAfter");

	UPROPERTY(EditInstanceOnly, Category = "Responses")
	FName ResponseContainerName = FName("ResponseContainer");

	UPROPERTY(EditInstanceOnly, Category = "Responses")
	TArray<FBTDialogueResponse> ReponsesUP;

	UPROPERTY(EditInstanceOnly, Category = "Responses")
	FNResponseCategory MiddleResponseCategory;

	UPROPERTY(EditInstanceOnly, Category = "Responses")
	FText MiddleResponse;

	UPROPERTY(EditInstanceOnly, Category = "Responses")
	int32 MiddleResponsePoint = 1;

	UPROPERTY(EditInstanceOnly, Category = "Responses")
	TArray<FBTDialogueResponse> ReponsesDOWN;

	UPROPERTY(EditInstanceOnly, Category = "Countdown")
	int32 TimeToResponse = 0;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
		EBTNodeResult::Type TaskResult) override;

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
	int32 MiddleResponseIndex = -1;
	EBTNodeResult::Type ResponseStatus = EBTNodeResult::InProgress;
	void CreateButtons();
	void CreateButton(FBTDialogueResponse Response, int8 Index, int32 Position, int32 MaxLevel);
	FString DisplayStaticResponses(
		const TArray<FBTDialogueResponse>& Responses, int32& Position, FString Title, bool Reverse) const;

	virtual UDialogHUD* GetHUDFromBlackboard(UBehaviorTreeComponent& OwnerComp);
	virtual void ReceiveOnTick(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds);
};
