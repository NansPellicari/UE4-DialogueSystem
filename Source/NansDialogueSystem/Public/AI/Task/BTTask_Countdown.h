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
#include "BehaviorTree/BTTaskNode.h"
#include "AI/BehaviorTree/BTTask_NotifyAIOnAbort.h"

#include "BTTask_Countdown.generated.h"

class UDialogueProgressBarWidget;

/**
 *
 */
UCLASS(NotBlueprintable)
class NANSDIALOGUESYSTEM_API UBTTask_Countdown : public UBTTask_NotifyAIOnAbort
{
	GENERATED_BODY()
	UBTTask_Countdown(const FObjectInitializer& objectInitializer);

public:
	DECLARE_EVENT_OneParam(UBTTask_Countdown, FTaskCountdownEvent, UBehaviorTreeComponent*);

	FTaskCountdownEvent& OnCountdownEnds()
	{
		return CountdownEnds;
	}

	void Initialize(UDialogueProgressBarWidget* _ProgressBar, int32 _TimeToResponse);

	virtual FString GetStaticDescription() const override;
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
		EBTNodeResult::Type TaskResult) override;
	void SetTimeToResponse(int32 Time);

protected:
	UPROPERTY()
	UDialogueProgressBarWidget* ProgressBar;

	// If 0: no countdown
	int32 TimeToResponse = 0;

private:
	float Countdown = 0.f;
	FTaskCountdownEvent CountdownEnds;
};
