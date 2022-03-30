// Copyright 2020-present Nans Pellicari (nans.pellicari@gmail.com).
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
#include "BehaviorTree/BTTaskNode.h"

#include "BTTask_Responses.generated.h"

class UDialogueUI;
class UPanelWidget;
class UResponseButtonWidget;
class UDialogueProgressBarWidget;
class UBTTask_Countdown;
class NDialoguePointsHandler;

/**
 */
UCLASS(Abstract)
class NANSDIALOGUESYSTEM_API UBTTask_Responses : public UBTTaskNode
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
public:
	virtual FName GetNodeIconName() const override;
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
	UPROPERTY(EditInstanceOnly, Category = "Dialogue")
	FName DialogName = NAME_None;

	UPROPERTY(EditInstanceOnly, Category = "Blackboard")
	bool bShowDialogueDetails = true;

	/** This is just here to indicate this values for developers, it is set in the developer's settings */
	UPROPERTY(VisibleAnywhere, Category = "UI")
	FName UINameKey = NAME_None;

	UPROPERTY(EditInstanceOnly, Category = "Responses")
	TArray<FBTDialogueResponse> ReponsesUP;

	UPROPERTY(EditInstanceOnly, Category = "Responses")
	FNDialogueCategory MiddleResponseCategory;

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
	UDialogueUI* DialogueUI;
	UPROPERTY()
	UPanelWidget* ResponsesSlot;
	UPROPERTY()
	UBTTask_Countdown* CountDownTask;
	TSharedPtr<NDialoguePointsHandler> PointsHandler;

private:
	int32 MiddleResponseIndex = -1;
	EBTNodeResult::Type ResponseStatus = EBTNodeResult::InProgress;
	void CreateButtons();
	void CreateButton(FBTDialogueResponse Response, int8 Index, int32 Position, int32 MaxLevel);
	FString DisplayStaticResponses(
		const TArray<FBTDialogueResponse>& Responses, FString Title, bool Reverse, int32 ForcePosition = -1000) const;
	FString DisplayStaticResponse(const FBTDialogueResponse& Response, int32 Position) const;
	virtual void ReceiveOnTick(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds);
};
