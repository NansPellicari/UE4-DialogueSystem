// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/BTTaskNode.h"
#include "CoreMinimal.h"

#include "BTTask_Countdown.generated.h"

class UWheelProgressBarWidget;

/**
 *
 */
UCLASS(NotBlueprintable)
class NANSDIALOGSYSTEM_API UBTTask_Countdown : public UBTTaskNode
{
	GENERATED_BODY()
	UBTTask_Countdown(const FObjectInitializer& objectInitializer);

public:
	DECLARE_EVENT_OneParam(UBTTask_Countdown, FTaskCountdownEvent, UBehaviorTreeComponent*);
	FTaskCountdownEvent& OnCountdownEnds()
	{
		return CountdownEnds;
	}
	void Initialize(UWheelProgressBarWidget* _ProgressBar, int32 _TimeToResponse);

	virtual FString GetStaticDescription() const override;
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult);
	void SetTimeToResponse(int32 Time);

protected:
	UPROPERTY()
	UWheelProgressBarWidget* ProgressBar;

	// If 0: no countdown
	int32 TimeToResponse = 0;

private:
	float Countdown = 0.f;
	FTaskCountdownEvent CountdownEnds;
};
