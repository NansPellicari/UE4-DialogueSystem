#pragma once
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NansExtension/Public/UMG/Blueprint/NansUserWidget.h"

#include "WheelProgressBarWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FProgressBarEnd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FProgressEvent, float, Percent);

/**
 *
 */
UCLASS()
class NANSDIALOGSYSTEM_API UWheelProgressBarWidget : public UNansUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "ProgressBar")
	FProgressBarEnd OnProgressBarEnd;

	UPROPERTY(BlueprintAssignable, Category = "ProgressBar")
	FProgressEvent OnProgress;

	UFUNCTION(BlueprintCallable, Category = "ProgressBar")
	void SetPercent(float _Percent);

	// This is a wrapper of Setpercent but call OnProgress event, so the use of it implies a progression behavior.
	// Otherwise use SetPercent instead.
	UFUNCTION(BlueprintCallable, Category = "ProgressBar")
	void SetProgress(float _Percent);

protected:
	UPROPERTY(BlueprintReadWrite, Category = "ProgressBar")
	float Percent;

	UFUNCTION(BlueprintImplementableEvent, Category = "ProgressBar", meta = (DisplayName = "OnSetPercent"))
	void ReceiveSetPercent(const float& _Percent);

	UFUNCTION(BlueprintImplementableEvent, Category = "ProgressBar", meta = (DisplayName = "OnProgress"))
	void ReceiveOnSetProgress(const float& _Percent);
};
