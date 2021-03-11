// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Task/BTTask_Countdown.h"

#include "Runtime/UMG/Public/Components/PanelWidget.h"
#include "UI/DialogueProgressBarWidget.h"

UBTTask_Countdown::UBTTask_Countdown(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	// Because corresponding function has to be called manually in each task which depending on
	bNotifyTick = false;
	bNotifyTaskFinished = false;
}

void UBTTask_Countdown::Initialize(UDialogueProgressBarWidget* _ProgressBar, int32 _TimeToResponse)
{
	ProgressBar = _ProgressBar;
	TimeToResponse = _TimeToResponse;
	ProgressBar->SetVisibility(ESlateVisibility::Collapsed);
}

EBTNodeResult::Type UBTTask_Countdown::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (TimeToResponse > 0)
	{
		Countdown = TimeToResponse;
	}

	return EBTNodeResult::InProgress;
}

FString UBTTask_Countdown::GetStaticDescription() const
{
	FString Desc = Super::GetStaticDescription();
	Desc += "\nTime To Response: " + FString::FromInt(TimeToResponse);
	return Desc;
}

void UBTTask_Countdown::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (Countdown <= 0 || ProgressBar == nullptr) return;

	Countdown -= DeltaSeconds;
	ProgressBar->SetVisibility(ESlateVisibility::Visible);
	ProgressBar->SetProgress(Countdown / TimeToResponse);
	if (Countdown <= 0)
	{
		OnCountdownEnds().Broadcast(&OwnerComp);
		return;
	}
}

void UBTTask_Countdown::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	if (Countdown <= 0 || ProgressBar == nullptr) return;

	ProgressBar->SetVisibility(ESlateVisibility::Collapsed);
}

void UBTTask_Countdown::SetTimeToResponse(int32 Time)
{
	TimeToResponse = Time;
}
