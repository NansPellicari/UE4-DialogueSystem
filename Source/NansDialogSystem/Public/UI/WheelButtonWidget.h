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
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NansUMGExtent/Public/Blueprint/NansUserWidget.h"

#include "WheelButtonWidget.generated.h"

UENUM(BlueprintType)
enum class EWheelDirection : uint8
{
	None,
	Clockwise,
	Anticlockwise
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FWheelButtonEvent,
	class UWheelButtonWidget*,
	Widget,
	EWheelDirection,
	Direction
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWheelButtonSimpleEvent);

DECLARE_MULTICAST_DELEGATE_TwoParams(
	FWheelButtonEventCpp,
	class UWheelButtonWidget* /*, Widget*/,
	EWheelDirection /*, Direction*/
);
DECLARE_MULTICAST_DELEGATE(FWheelButtonSimpleEventCpp);

/**
 *
 */
UCLASS(meta = (EnableNativeTick))
class NANSDIALOGSYSTEM_API UWheelButtonWidget : public UNansUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Event")
	FWheelButtonEvent OnMovement;
	FWheelButtonEventCpp OnMovementCpp;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Event")
	FWheelButtonSimpleEvent OnStopMovement;
	FWheelButtonSimpleEventCpp OnStopMovementCpp;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void ResetDistance();
	/**
	 * Negative values are CounterClockwise
	 * Positive values are Clockwise
	 */
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void DoTurnWheel(float TurnNumber);

	void GetWheelDistRatio(float& WheelRatioClockwise, float& WheelRatioAntiClockwise) const;

	float GetRoundDistance() const;
	float GetDragDistanceClockwise() const;
	float GetDragDistanceAntiClockwise() const;

	bool GetResetDistanceOnDrop()
	{
		return ResetDistanceOnDrop;
	};

	float GetMouseVelocityInfluenceRatio()
	{
		return MouseVelocityInfluenceRatio;
	};
	EWheelDirection GetDirection() const;

	virtual FReply
	NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual bool NativeOnDragOver(
		const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragDetected(
		const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	void DoMoveWheel(float ButtonDistance, EWheelDirection Direction/*, bool bByDrag = false*/);

	UPROPERTY(BlueprintReadWrite, Category = "Distance")
	float RoundDistance;

	UPROPERTY(BlueprintReadWrite, Category = "Distance")
	float DragDistanceClockwise;

	UPROPERTY(BlueprintReadWrite, Category = "Distance")
	float DragDistanceAntiClockwise;

	UPROPERTY(BlueprintReadWrite, Category = "Distance")
	EWheelDirection Direction;

	UPROPERTY(BlueprintReadWrite, Category = "Distance")
	FVector2D RingCenter;

	UPROPERTY(BlueprintReadWrite, Category = "Distance")
	float RingRadius;

	UPROPERTY(BlueprintReadWrite, Category = "Widget", meta=(BindWidget))
	class UImage* DraggableButton = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "Widget", meta=(BindWidget))
	class UImage* Circle = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Widget")
	float RingThicknessRatio = 0.1f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Widget")
	float MouseVelocityInfluenceRatio = 1.f;

	UPROPERTY(BlueprintReadWrite, Category = "Movement")
	FVector2D DragOffset;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement")
	bool ResetDistanceOnDrop = true;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void LimitButtonMovement(UPARAM(ref) FVector2D& DesiredPosition);

private:
	bool bIsDragging = false;
	bool bPositionAlreadyInit = false;
	FVector2D ButtonPosition;
	FVector2D LastButtonPosition;

	void ComputeBaseMetrics(const FGeometry& MyGeometry);
};
