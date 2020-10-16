// Fill out your copyright notice in the Description page of Project Settings.

#include "BTStepsForDialog.h"

#include "Attribute/ResponseCategory.h"
#include "NansBehaviorSteps/Public/BTSteps.h"

UBTStepsForDialog::UBTStepsForDialog()
{
	StepsHandler = CreateDefaultSubobject<UBTSteps>(FName(TEXT("Sub_BTSteps")));

	EarnedPoints = TMap<FName, int32>();
	HeapResponses = TArray<FBTPointInStep>();
}

void UBTStepsForDialog::BeginDestroy()
{
	EarnedPoints.Empty();
	HeapResponses.Empty();
	Super::BeginDestroy();
}

int32 UBTStepsForDialog::GetCurrentStep_Implementation()
{
	return Execute_GetCurrentStep(StepsHandler);
}

void UBTStepsForDialog::AddPoints(FPoint Point, int32 Position)
{
	int32 Step = Execute_GetCurrentStep(StepsHandler);
	HeapResponses.Emplace(FBTPointInStep(Step, Point, Position));
	int32& PointNumber = EarnedPoints.FindOrAdd(Point.Category.Name);
	PointNumber += Point.Point;
}

bool UBTStepsForDialog::StepIsAlreadyDone_Implementation(const int32 Step) const
{
	return Execute_StepIsAlreadyDone(StepsHandler, Step);
}

void UBTStepsForDialog::getLastResponse(FBTPointInStep& PointInStep)
{
	PointInStep = HeapResponses.Last();
}
int32 UBTStepsForDialog::GetPoints(FNResponseCategory Category) const
{
	int32 TotalPoints = 0;
	for (FBTPointInStep Point : HeapResponses)
	{
		TotalPoints += Point.Point.Category.Name == Category.Name ? Point.Point.Point : 0;
	}

	return TotalPoints;
}

void UBTStepsForDialog::ConcludeAllSteps_Implementation()
{
	Execute_ConcludeAllSteps(StepsHandler);
}

void UBTStepsForDialog::getLastResponseFromStep(const int32 SearchStep, FBTPointInStep& PointInStep)
{
	for (int32 i = HeapResponses.Num() - 1; i >= 0; --i)
	{
		if (HeapResponses[i].Step == SearchStep)
		{
			PointInStep = HeapResponses[i];
			break;
		}
	}
}

void UBTStepsForDialog::Clear_Implementation()
{
	EarnedPoints.Empty();
	HeapResponses.Empty();
	Execute_Clear(StepsHandler);
}

int32 UBTStepsForDialog::GetStepToGo_Implementation()
{
	return Execute_GetStepToGo(StepsHandler);
}

void UBTStepsForDialog::AddFinishedStepWithPoint(int32 Step, FPoint Point, int32 Position)
{
	if (Execute_StepIsAlreadyDone(StepsHandler, Step))
	{
		if (bDebug) UE_LOG(LogTemp, Warning, TEXT("%s: Step %d already done"), *GetName(), Step);
		return;
	}

	StepsHandler->AddFinishedStep(Step);

	HeapResponses.Emplace(FBTPointInStep(Step, Point, Position));
	int32& PointNumber = EarnedPoints.FindOrAdd(Point.Category.Name);
	PointNumber += Point.Point;
}

void UBTStepsForDialog::FinishedCurrentStep_Implementation()
{
	Execute_FinishedCurrentStep(StepsHandler);
}

// A RedoStep keep the count of all earned points
void UBTStepsForDialog::RedoStep_Implementation(int32 Step, bool FromLastPlay)
{
	Execute_RedoStep(StepsHandler, Step, FromLastPlay);
}

void UBTStepsForDialog::JumpTo_Implementation(int32 Step)
{
	Execute_JumpTo(StepsHandler, Step);
}

bool UBTStepsForDialog::StepIsPlayable_Implementation(const int32& Step, bool ResetStepToGoIfPlay)
{
	return Execute_StepIsPlayable(StepsHandler, Step, ResetStepToGoIfPlay);
}

bool UBTStepsForDialog::PlayStepAndMoveForward_Implementation(const int32& Step)
{
	return Execute_PlayStepAndMoveForward(StepsHandler, Step);
}

bool UBTStepsForDialog::StepCanPlayAndReset_Implementation(const int32& Step)
{
	return Execute_StepCanPlayAndReset(StepsHandler, Step);
}

bool UBTStepsForDialog::StepCanPlay_Implementation(const int32& Step)
{
	return Execute_StepCanPlay(StepsHandler, Step);
}

void UBTStepsForDialog::OnUserLeave_Implementation()
{
	if (bDebug) UE_LOG(LogTemp, Warning, TEXT("%s: I've been cleared"), *GetName());
	Execute_Clear(this);
}
