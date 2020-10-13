// Fill out your copyright notice in the Description page of Project Settings.

#include "BTStepsWithPoints.h"

#include "NansBehaviorSteps/Public/BTSteps.h"

UBTStepsWithPoints::UBTStepsWithPoints()
{
	StepsHandler = CreateDefaultSubobject<UBTSteps>(FName(TEXT("Sub_BTSteps")));

	EarnedPoints = TMap<EAlignment, int32>();
	HeapResponses = TArray<FBTPointInStep>();
}

void UBTStepsWithPoints::BeginDestroy()
{
	EarnedPoints.Empty();
	HeapResponses.Empty();
	Super::BeginDestroy();
}

int32 UBTStepsWithPoints::GetCurrentStep_Implementation()
{
	return Execute_GetCurrentStep(StepsHandler);
}

void UBTStepsWithPoints::AddPoints(FPoint Point, int32 Position)
{
	int32 Step = Execute_GetCurrentStep(StepsHandler);
	HeapResponses.Emplace(FBTPointInStep(Step, Point, Position));
	int32& PointNumber = EarnedPoints.FindOrAdd(Point.Alignment);
	PointNumber += Point.Point;
}

bool UBTStepsWithPoints::StepIsAlreadyDone_Implementation(const int32 Step) const
{
	return Execute_StepIsAlreadyDone(StepsHandler, Step);
}

void UBTStepsWithPoints::getLastResponse(FBTPointInStep& PointInStep)
{
	PointInStep = HeapResponses.Last();
}
int32 UBTStepsWithPoints::GetPoints(EAlignment Alignment) const
{
	int32 TotalPoints = 0;
	for (FBTPointInStep Point : HeapResponses)
	{
		TotalPoints += Point.Point.Alignment == Alignment ? Point.Point.Point : 0;
	}

	return TotalPoints;
}

void UBTStepsWithPoints::ConcludeAllSteps_Implementation()
{
	Execute_ConcludeAllSteps(StepsHandler);
}

void UBTStepsWithPoints::getLastResponseFromStep(const int32 SearchStep, FBTPointInStep& PointInStep)
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

void UBTStepsWithPoints::Clear_Implementation()
{
	EarnedPoints.Empty();
	HeapResponses.Empty();
	Execute_Clear(StepsHandler);
}

int32 UBTStepsWithPoints::GetStepToGo_Implementation()
{
	return Execute_GetStepToGo(StepsHandler);
}

void UBTStepsWithPoints::AddFinishedStepWithPoint(int32 Step, FPoint Point, int32 Position)
{
	if (Execute_StepIsAlreadyDone(StepsHandler, Step))
	{
		if (bDebug) UE_LOG(LogTemp, Warning, TEXT("%s: Step %d already done"), *GetName(), Step);
		return;
	}

	StepsHandler->AddFinishedStep(Step);

	HeapResponses.Emplace(FBTPointInStep(Step, Point, Position));
	int32& PointNumber = EarnedPoints.FindOrAdd(Point.Alignment);
	PointNumber += Point.Point;
}

void UBTStepsWithPoints::FinishedCurrentStep_Implementation()
{
	Execute_FinishedCurrentStep(StepsHandler);
}

// A RedoStep keep the count of all earned points
void UBTStepsWithPoints::RedoStep_Implementation(int32 Step, bool FromLastPlay)
{
	Execute_RedoStep(StepsHandler, Step, FromLastPlay);
}

void UBTStepsWithPoints::JumpTo_Implementation(int32 Step)
{
	Execute_JumpTo(StepsHandler, Step);
}

bool UBTStepsWithPoints::StepIsPlayable_Implementation(const int32& Step, bool ResetStepToGoIfPlay)
{
	return Execute_StepIsPlayable(StepsHandler, Step, ResetStepToGoIfPlay);
}

bool UBTStepsWithPoints::PlayStepAndMoveForward_Implementation(const int32& Step)
{
	return Execute_PlayStepAndMoveForward(StepsHandler, Step);
}

bool UBTStepsWithPoints::StepCanPlayAndReset_Implementation(const int32& Step)
{
	return Execute_StepCanPlayAndReset(StepsHandler, Step);
}

bool UBTStepsWithPoints::StepCanPlay_Implementation(const int32& Step)
{
	return Execute_StepCanPlay(StepsHandler, Step);
}

void UBTStepsWithPoints::OnUserLeave_Implementation()
{
	if (bDebug) UE_LOG(LogTemp, Warning, TEXT("%s: I've been cleared"), *GetName());
	Execute_Clear(this);
}
