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

#include "UI/WheelButtonWidget.h"

#include "Blueprint/DragDropOperation.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
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

	const FGeometry ButtonGeometry = DraggableButton->GetCachedGeometry();
	const FVector2D ButtonSize = ButtonGeometry.GetLocalSize();
	ButtonPosition = MyGeometry.AbsoluteToLocal(ButtonGeometry.GetAbsolutePosition());
	const FGeometry CircleGeometry = Circle->GetCachedGeometry();
	const FVector2D CircleSize = CircleGeometry.GetLocalSize();
	const FVector2D CirclePos = MyGeometry.AbsoluteToLocal(CircleGeometry.GetAbsolutePosition());
	const FVector2D MiddleCircle = CircleSize / 2;
	RingRadius = MiddleCircle.X - ((CircleSize.X * RingThicknessRatio) / 2);
	RingCenter = CirclePos + MiddleCircle - (ButtonSize / 2);
	RoundDistance = RingRadius * 2 * PI;
}

void UWheelButtonWidget::GetWheelDistRatio(float& WheelRatioClockwise, float& WheelRatioAntiClockwise) const
{
	WheelRatioClockwise = (DragDistanceClockwise / RoundDistance) * MouseVelocityInfluenceRatio;
	WheelRatioAntiClockwise = (DragDistanceAntiClockwise / RoundDistance) * MouseVelocityInfluenceRatio;
}

float UWheelButtonWidget::GetRoundDistance() const
{
	return RoundDistance;
}

float UWheelButtonWidget::GetDragDistanceClockwise() const
{
	return DragDistanceClockwise;
}

float UWheelButtonWidget::GetDragDistanceAntiClockwise() const
{
	return DragDistanceAntiClockwise;
}

EWheelDirection UWheelButtonWidget::GetDirection() const
{
	return Direction;
}

void UWheelButtonWidget::LimitButtonMovement(FVector2D& DesiredPosition)
{
	FVector2D DistVector = DesiredPosition - RingCenter;
	DistVector.Normalize();
	DesiredPosition = (DistVector * RingRadius) + RingCenter;
}

FReply UWheelButtonWidget::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	FReply Event = Super::NativeOnPreviewMouseButtonDown(InGeometry, InMouseEvent);

	if (DraggableButton == nullptr) return FReply::Unhandled();

	const FGeometry ButtonGeometry = DraggableButton->GetCachedGeometry();
	const FVector2D ButtonSize = ButtonGeometry.GetLocalSize();
	const FVector2D MousePosButton = ButtonGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
	FVector2D MousePos = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
	const bool MouseIn = MousePosButton <= ButtonSize && MousePosButton >= FVector2D(0, 0);

	if (!MouseIn || bIsDragging)
	{
		return Event;
	}

	return FReply::Handled().DetectDrag(TakeWidget(), FKey(EKeys::LeftMouseButton));
}

void UWheelButtonWidget::DoMoveWheel(const float ButtonDistance,
	const EWheelDirection InDirection/*, bool bByDrag= false*/)
{
	Direction = InDirection;
	DragDistanceAntiClockwise += Direction == EWheelDirection::Anticlockwise ? ButtonDistance : 0;
	DragDistanceClockwise += Direction == EWheelDirection::Clockwise ? ButtonDistance : 0;

	OnMovement.Broadcast(this, Direction);
	OnMovementCpp.Broadcast(this, Direction);

	// TODO should be great to reverse engineering the position of the DraggableButton when the turn is made programatically
	// if (!bByDrag)
	// {
	// }
}

bool UWheelButtonWidget::NativeOnDragOver(
	const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	const bool Ret = Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);

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

	const FVector2D OldPos = RingCenter - ButtonPosition;
	const FVector2D NewPos = RingCenter - DesiredPosition;

	const float Angle = FMath::Atan2(OldPos.X, OldPos.Y) - FMath::Atan2(NewPos.X, NewPos.Y);
	const float Dist = FVector2D::Distance(ButtonPosition, DesiredPosition);

	DoMoveWheel(Dist, Angle > 0 ? EWheelDirection::Clockwise : EWheelDirection::Anticlockwise);
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

void UWheelButtonWidget::DoTurnWheel(float TurnNumber)
{
	float TotalDist = RoundDistance * TurnNumber;
	Direction = EWheelDirection::Clockwise;
	if (FMath::IsNegativeFloat(TotalDist))
	{
		TotalDist *= -1;
		Direction = EWheelDirection::Anticlockwise;
	}

	DoMoveWheel(TotalDist, Direction);
	Direction = EWheelDirection::None;
	OnStopMovement.Broadcast();
	OnStopMovementCpp.Broadcast();
}
