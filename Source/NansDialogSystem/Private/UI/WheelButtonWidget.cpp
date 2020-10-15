// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/WheelButtonWidget.h"

#include "Blueprint/DragDropOperation.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Button.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "EngineGlobals.h"
#include "Input/Events.h"
#include "Input/Reply.h"
#include "Layout/Geometry.h"
#include "NansUE4Utilities/public/Misc/ErrorUtils.h"

void UWheelButtonWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// This is for the first button position init
	if (bPositionAlreadyInit == false)
	{
		if (DraggableButton == nullptr) return;
		if (Circle == nullptr) return;

		ComputeBaseMetrics(MyGeometry);
		UCanvasPanelSlot* ButtonSlot = Cast<UCanvasPanelSlot>(DraggableButton->Slot);

		// At the first tick, geometry is not well set, Panelslot as aproximately the good size,
		// so we can wait until all metrics are well computed
		if (!ButtonSlot->GetPosition().Equals(ButtonPosition, 10.f)) return;

		LimitButtonMovement(ButtonPosition);
		ButtonSlot->SetPosition(ButtonPosition);
		Direction = EWheelDirection::None;
		bPositionAlreadyInit = true;
	}
}

void UWheelButtonWidget::ComputeBaseMetrics(const FGeometry& MyGeometry)
{
	if (DraggableButton == nullptr) return;
	if (Circle == nullptr) return;

	FGeometry ButtonGeometry = DraggableButton->GetCachedGeometry();
	FVector2D ButtonSize = ButtonGeometry.GetLocalSize();
	ButtonPosition = MyGeometry.AbsoluteToLocal(ButtonGeometry.GetAbsolutePosition());
	FGeometry CircleGeometry = Circle->GetCachedGeometry();
	FVector2D CircleSize = CircleGeometry.GetLocalSize();
	FVector2D CirclePos = MyGeometry.AbsoluteToLocal(CircleGeometry.GetAbsolutePosition());
	FVector2D MiddleCircle = CircleSize / 2;
	RingRadius = MiddleCircle.X - ((CircleSize.X * RingThicknessRatio) / 2);
	RingCenter = CirclePos + MiddleCircle - (ButtonSize / 2);
	RoundDistance = RingRadius * 2 * PI;
}

void UWheelButtonWidget::GetWheelDistRatio(float& WheelRatioClockwise, float& WheelRatioAntiClockwise)
{
	WheelRatioClockwise = (DragDistanceClockwise / RoundDistance) * MouseVelocityInfluenceRatio;
	WheelRatioAntiClockwise = (DragDistanceAntiClockwise / RoundDistance) * MouseVelocityInfluenceRatio;
}

float UWheelButtonWidget::GetRoundDistance()
{
	return RoundDistance;
}

float UWheelButtonWidget::GetDragDistanceClockwise()
{
	return DragDistanceClockwise;
}

float UWheelButtonWidget::GetDragDistanceAntiClockwise()
{
	return DragDistanceAntiClockwise;
}

EWheelDirection UWheelButtonWidget::GetDirection()
{
	return Direction;
}

void UWheelButtonWidget::LimitButtonMovement(FVector2D& DesiredPosition)
{
	FVector2D DistVector = DesiredPosition - RingCenter;
	DistVector.Normalize();
	DesiredPosition = (DistVector * RingRadius) + RingCenter;
}

FReply UWheelButtonWidget::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Event = Super::NativeOnPreviewMouseButtonDown(InGeometry, InMouseEvent);

	if (DraggableButton == nullptr) return FReply::Unhandled();

	FGeometry ButtonGeometry = DraggableButton->GetCachedGeometry();
	FVector2D ButtonSize = ButtonGeometry.GetLocalSize();
	FVector2D MousePosButton = ButtonGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
	FVector2D MousePos = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
	bool MouseIn = MousePosButton <= ButtonSize && MousePosButton >= FVector2D(0, 0);

	if (!MouseIn || bIsDragging)
	{
		return Event;
	}

	return FReply::Handled().DetectDrag(TakeWidget(), FKey(EKeys::LeftMouseButton));
}

bool UWheelButtonWidget::NativeOnDragOver(
	const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	bool Ret = Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);

	if (DraggableButton == nullptr) return false;
	if (Circle == nullptr) return false;

	ComputeBaseMetrics(InGeometry);

	FVector2D DesiredPosition = InGeometry.AbsoluteToLocal(InDragDropEvent.GetScreenSpacePosition());
	LimitButtonMovement(DesiredPosition);
	UCanvasPanelSlot* ButtonSlot = Cast<UCanvasPanelSlot>(DraggableButton->Slot);
	ButtonSlot->SetPosition(DesiredPosition);

	if (LastButtonPosition == ButtonPosition)
	{
		return false;
	}

	FVector2D OldPos = RingCenter - ButtonPosition;
	FVector2D NewPos = RingCenter - DesiredPosition;

	float Angle = FMath::Atan2(OldPos.X, OldPos.Y) - FMath::Atan2(NewPos.X, NewPos.Y);
	float Dist = FVector2D::Distance(ButtonPosition, DesiredPosition);

	// A real positive angle is Anticlockwise, there is a bug here
	Direction = Angle > 0 ? EWheelDirection::Clockwise : EWheelDirection::Anticlockwise;

	DragDistanceAntiClockwise += Direction == EWheelDirection::Anticlockwise ? Dist : 0;
	DragDistanceClockwise += Direction == EWheelDirection::Clockwise ? Dist : 0;

	OnMovement.Broadcast(this, Direction);
	OnMovementCpp.Broadcast(this, Direction);

	LastButtonPosition = ButtonPosition;
	return Ret;
}

void UWheelButtonWidget::NativeOnDragDetected(
	const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	if (DraggableButton == nullptr) return;
	OutOperation = UWidgetBlueprintLibrary::CreateDragDropOperation(UDragDropOperation::StaticClass());
	bIsDragging = true;
}

void UWheelButtonWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);

	if (DraggableButton == nullptr) return;

	Direction = EWheelDirection::None;

	if (ResetDistanceOnDrop)
	{
		ResetDistance();
	}

	bIsDragging = false;
	OnStopMovement.Broadcast();
	OnStopMovementCpp.Broadcast();
}

void UWheelButtonWidget::ResetDistance()
{
	DragDistanceAntiClockwise = 0;
	DragDistanceClockwise = 0;
}
