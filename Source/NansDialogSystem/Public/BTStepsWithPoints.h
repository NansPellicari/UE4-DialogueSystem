// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactive/Public/AI/BehaviorTree/InteractiveBlackboardObjectInterface.h"
#include "NansBehaviorSteps/Public/BTStepsHandler.h"
#include "PointSystemHelpers.h"
#include "UObject/NoExportTypes.h"

#include "BTStepsWithPoints.generated.h"

class UBTSteps;

/**
 * Points Associated to step
 */
USTRUCT(BlueprintType)
struct NANSDIALOGSYSTEM_API FBTPointInStep
{
	GENERATED_USTRUCT_BODY()

	FBTPointInStep(int32 _Step = 0, FPoint _Point = FPoint(), int32 _Position = 0)
		: Step(_Step), Point(_Point), Position(_Position){};
	int32 Step;
	FPoint Point;
	int32 Position;
};

/**
 * TODO try to remove the IInteractiveBlackboardObjectInterface dependency
 */
UCLASS(BlueprintType)
class NANSDIALOGSYSTEM_API UBTStepsWithPoints : public UObject, public IBTStepsHandler, public IInteractiveBlackboardObjectInterface
{
	GENERATED_BODY()

public:
	UBTStepsWithPoints();
	virtual void BeginDestroy() override;

	UFUNCTION(BlueprintCallable, Category = "Step")
	void AddPoints(FPoint Point, int32 Position);

	UFUNCTION(BlueprintCallable, Category = "Step")
	void AddFinishedStepWithPoint(int32 Step, FPoint Point, int32 Position);

	UFUNCTION(BlueprintCallable, Category = "Step")
	void getLastResponse(FBTPointInStep& PointInStep);

	UFUNCTION(BlueprintCallable, Category = "Step")
	void getLastResponseFromStep(const int32 SearchStep, FBTPointInStep& PointInStep);

	UFUNCTION(BlueprintCallable, Category = "Step")
	int32 GetPoints(EAlignment Alignment) const;

	virtual int32 GetCurrentStep_Implementation() override;
	virtual void ConcludeAllSteps_Implementation() override;
	virtual void FinishedCurrentStep_Implementation() override;
	virtual void RedoStep_Implementation(int32 Step, bool FromLastPlay = false) override;
	virtual void JumpTo_Implementation(int32 Step) override;
	virtual bool StepIsAlreadyDone_Implementation(const int32 Step) const override;
	virtual void Clear_Implementation() override;
	virtual bool StepCanPlay_Implementation(const int32& Step) override;
	// If the step is the same as StepToGo, this method reset StepToGo to 0
	virtual bool StepCanPlayAndReset_Implementation(const int32& Step) override;
	virtual int32 GetStepToGo_Implementation() override;
	virtual bool StepIsPlayable_Implementation(const int32& Step, bool ResetStepToGoIfPlay = true) override;
	virtual bool PlayStepAndMoveForward_Implementation(const int32& Step) override;

	UFUNCTION(BlueprintNativeEvent, Category = "Blackboard")
	void OnUserLeave();
	virtual void OnUserLeave_Implementation() override;

protected:
	UPROPERTY(BlueprintReadWrite, Category = "Step")
	bool bDebug = false;

	/** A HEAP on point by step, the HEAP allow to keep trace of action's chronologicaly */
	TArray<FBTPointInStep> HeapResponses;

	UPROPERTY(BlueprintReadOnly, Category = "Step")
	TMap<EAlignment, int32> EarnedPoints;

private:
	UPROPERTY()
	UBTSteps* StepsHandler;
};
