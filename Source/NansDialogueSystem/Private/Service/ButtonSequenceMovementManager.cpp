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

#include "Service/ButtonSequenceMovementManager.h"

#include "BTDialogueTypes.h"
#include "Components/CanvasPanelSlot.h"
#include "NansCoreHelpers/Public/Math/MathUtilities.h"
#include "NansUE4Utilities/public/Misc/ErrorUtils.h"
#include "Runtime/UMG/Public/Components/PanelWidget.h"
#include "UI/ButtonSequenceWidget.h"
#include "UI/WheelButtonWidget.h"

#define LOCTEXT_NAMESPACE "DialogueSystem"

void NButtonSequenceMovementManager::Initialize(UPanelWidget* _ButtonsSlot, float _Velocity,
	UWheelButtonWidget* _WheelButton)
{
	if (!ensureMsgf(_ButtonsSlot != nullptr, TEXT("The ButtonsSlotName set doesn't exists in UI")))
	{
		return;
	}
	if (!ensureMsgf(_WheelButton != nullptr, TEXT("The WheelButtonName set doesn't exists in UI")))
	{
		return;
	}

	WheelButton = _WheelButton;
	ButtonsSlot = _ButtonsSlot;
	InitialVelocity = _Velocity;
	VelocityClock = InitialVelocity;
	VelocityAntiClock = InitialVelocity;

	if (WheelButton != nullptr)
	{
		WheelButton->OnMovementCpp.AddRaw(this, &NButtonSequenceMovementManager::ChangeVelocity);
		WheelButton->OnStopMovementCpp.AddRaw(this, &NButtonSequenceMovementManager::ResetVelocity);
	}
}

void NButtonSequenceMovementManager::Reset()
{
	ButtonsSlot = nullptr;
	InitialVelocity = 0;
	VelocityClock = 0;
	VelocityAntiClock = 0;
	ButtonsPositions.Empty();
	ForbiddenZone.Empty();
	if (WheelButton != nullptr)
	{
		WheelButton->OnMovementCpp.RemoveAll(this);
		WheelButton->OnStopMovementCpp.RemoveAll(this);
	}
	WheelButton = nullptr;
}

void NButtonSequenceMovementManager::MoveButtons(float DeltaSeconds)
{
	const FGeometry CachedGeometry = ButtonsSlot->GetCachedGeometry();
	const FVector2D CanvasSize = CachedGeometry.GetLocalSize();
	const float MaxDistPerSec = 1000.f;
	float MultiplierClock = (DeltaSeconds * VelocityClock * MaxDistPerSec);
	float MultiplierAntiClock = (DeltaSeconds * VelocityAntiClock * MaxDistPerSec);
	float MultiplierNeutral = (DeltaSeconds * 0.2f * MaxDistPerSec);

	ComputeForbiddenZone();

	for (int32 i = 0; i < ButtonsSlot->GetChildrenCount(); i++)
	{
		UButtonSequenceWidget* Button = Cast<UButtonSequenceWidget>(ButtonsSlot->GetChildAt(i));
		if (Button == nullptr) continue;
		UCanvasPanelSlot* ButtonSlot = Cast<UCanvasPanelSlot>(Button->Slot);
		if (ButtonSlot == nullptr) continue;

		TMap<FString, UCanvasPanelSlot*> OverlappedBt;
		FVector2D Centroid;

		// this allow button bouncing rather than overlapping
		if (InButtonsZone(i, Button, OverlappedBt, Centroid))
		{
			if (OverlappedBt.Num() > 0)
			{
				for (TPair<FString, UCanvasPanelSlot*> Bt : OverlappedBt)
				{
					FVector2D BtOverPos = Bt.Value->GetPosition();
					const FVector2D BtOverSize = Bt.Value->GetSize();

					FVector2D TempOverPos;
					FVector2D Direction = BtOverPos - Centroid;
					const FVector2D Dist = (Direction) + BtOverPos;
					TempOverPos.X =
						FMath::Clamp<float>(
							Dist.X * (Direction.GetSafeNormal().X * BtOverSize.X),
							0,
							CanvasSize.X - BtOverSize.X
						);
					TempOverPos.Y =
						FMath::Clamp<float>(
							Dist.Y * (Direction.GetSafeNormal().Y * BtOverSize.Y),
							0,
							CanvasSize.Y - BtOverSize.Y
						);
					ButtonsPositions.Add(Bt.Key, TempOverPos);
				}
			}
		}

		FVector2D BtPos = ButtonSlot->GetPosition();
		FVector2D BtSize = ButtonSlot->GetSize();
		FVector2D* AimPos = ButtonsPositions.Find(ButtonSlot->GetName());

		// When button reaches the desired position or when it hits forbidden zone
		if (AimPos == nullptr || FMath::IsNearlyEqual(AimPos->X, BtPos.X, 10.f) || InZone(ForbiddenZone, BtPos, BtSize))
		{
			FVector2D NewPos = FindRandomCoordInSlot(Button);
			ButtonsPositions.Add(ButtonSlot->GetName(), NewPos);
			AimPos = &NewPos;
		}

		FVector2D Distance = *AimPos - BtPos;
		float Multiplier = Button->InfluencedBy == EResponseDirection::UP ? MultiplierClock : MultiplierAntiClock;
		Multiplier = Button->InfluencedBy == EResponseDirection::NONE ? MultiplierNeutral : Multiplier;
		FVector2D NewTrajectory = Distance.GetSafeNormal() * Multiplier;

		ButtonSlot->SetPosition(BtPos + NewTrajectory);
	}
}

void NButtonSequenceMovementManager::PlaceButtons()
{
	const int32 MaxTentative = 10;
	ComputeForbiddenZone();

	TMap<FString, UCanvasPanelSlot*> OverlappedBt;
	FVector2D Centroid;

	for (int32 i = 0; i < ButtonsSlot->GetChildrenCount(); i++)
	{
		UButtonSequenceWidget* Button = Cast<UButtonSequenceWidget>(ButtonsSlot->GetChildAt(i));
		if (Button == nullptr) continue;
		UCanvasPanelSlot* ButtonSlot = Cast<UCanvasPanelSlot>(Button->Slot);
		if (ButtonSlot == nullptr) continue;

		FVector2D NewPos = FindRandomCoordInSlot(Button);
		FVector2D BtSize = ButtonSlot->GetSize();

		ButtonSlot->SetPosition(NewPos);

		for (int32 j = 0; j < MaxTentative; ++j)
		{
			if (!InButtonsZone(i, Button, OverlappedBt, Centroid))
			{
				break;
			}
			NewPos = FindRandomCoordInSlot(Button);
			ButtonSlot->SetPosition(NewPos);
		}
	}
}

bool NButtonSequenceMovementManager::InButtonsZone(const int32 Position,
	const UButtonSequenceWidget* Button,
	TMap<FString, UCanvasPanelSlot*>& OverlappedBt,
	FVector2D& Centroid) const
{
	UCanvasPanelSlot* ButtonSlot = Cast<UCanvasPanelSlot>(Button->Slot);
	if (ButtonSlot == nullptr) return false;

	const FVector2D Pos = ButtonSlot->GetPosition();
	const FVector2D Size = ButtonSlot->GetSize();

	TArray<FVector2D> Positions;
	Positions.Add(Pos);

	bool IsInZone = false;
	for (int32 i = 0; i < ButtonsSlot->GetChildrenCount(); i++)
	{
		UButtonSequenceWidget* OtherButton = Cast<UButtonSequenceWidget>(ButtonsSlot->GetChildAt(i));

		if (OtherButton == nullptr) continue;

		UCanvasPanelSlot* OtherButtonSlot = Cast<UCanvasPanelSlot>(OtherButton->Slot);

		if (OtherButtonSlot == nullptr || ButtonSlot == OtherButtonSlot) continue;

		TArray<FVector2D, TFixedAllocator<2>> OtherBt;
		FVector2D InitCoord = OtherButtonSlot->GetPosition();
		OtherBt.Add(InitCoord);
		OtherBt.Add(InitCoord + OtherButtonSlot->GetSize());

		if (InZone(OtherBt, Pos, Size))
		{
			OverlappedBt.Emplace(ButtonSlot->GetName(), ButtonSlot);
			OverlappedBt.Emplace(OtherButtonSlot->GetName(), OtherButtonSlot);
			Positions.Add(InitCoord);
			IsInZone = true;
		}
	}

	if (IsInZone)
	{
		Centroid = NMathUtilities::FindCentroid(Positions);
	}

	return IsInZone;
}

bool NButtonSequenceMovementManager::InZone(
	const TArray<FVector2D, TFixedAllocator<2>>& Zone, const FVector2D& Pos, const FVector2D& Size) const
{
	if (Zone.Num() <= 0)
	{
		return false;
	}

	const bool XInZone = Pos.X + Size.X >= Zone[0].X && Pos.X <= Zone[1].X;
	const bool YInZone = Pos.Y + Size.Y >= Zone[0].Y && Pos.Y <= Zone[1].Y;

	return XInZone && YInZone;
}

FVector2D NButtonSequenceMovementManager::FindRandomCoordInSlot(UButtonSequenceWidget* Button) const
{
	UCanvasPanelSlot* Slot = Cast<UCanvasPanelSlot>(Button->Slot);
	const FGeometry CachedGeometry = ButtonsSlot->GetCachedGeometry();
	const FVector2D CanvasSize = CachedGeometry.GetLocalSize();
	const FVector2D SlotSize = Slot->GetSize();
	FVector2D NewPos;

	NewPos.X = FMath::Clamp<float>(FMath::FRand() * CanvasSize.X, 0, CanvasSize.X - SlotSize.X);
	NewPos.Y = FMath::Clamp<float>(FMath::FRand() * CanvasSize.Y, 0, CanvasSize.Y - SlotSize.Y);
	bool IsInZone = InZone(ForbiddenZone, NewPos, SlotSize);

	// If this random coordinate IsInZone we translate it out of the zone's boundaries
	if (IsInZone)
	{
		const bool BeforeYAvailable = ForbiddenZone[0].Y > SlotSize.Y;
		const bool AfterYAvailable = CanvasSize.Y - ForbiddenZone[1].Y > SlotSize.Y;
		const bool BeforeXAvailable = ForbiddenZone[0].X > SlotSize.X;
		const bool AfterXAvailable = CanvasSize.X - ForbiddenZone[1].X > SlotSize.X;
		const float DistanceXEndBoundary = ForbiddenZone[1].X - (NewPos.X + SlotSize.X);
		const float DistanceYEndBoundary = ForbiddenZone[1].Y - (NewPos.Y + SlotSize.Y);
		const float DistanceXBeginBoundary = NewPos.X - ForbiddenZone[0].X;
		const float DistanceYBeginBoundary = NewPos.Y - ForbiddenZone[0].Y;

		// Closer of the bottom boundary of the zone
		if (DistanceYEndBoundary < DistanceYBeginBoundary)
		{
			NewPos.Y = AfterYAvailable ? ForbiddenZone[1].Y : ForbiddenZone[0].Y - SlotSize.Y;
		}

		// Closer of the top boundary of the zone
		if (DistanceYEndBoundary > DistanceYBeginBoundary)
		{
			NewPos.Y = BeforeYAvailable ? ForbiddenZone[0].Y - SlotSize.Y : ForbiddenZone[1].Y;
		}

		// Closer of the right boundary of the zone
		if (DistanceXEndBoundary < DistanceXBeginBoundary)
		{
			NewPos.X = AfterXAvailable ? ForbiddenZone[1].X : ForbiddenZone[0].X - SlotSize.X;
		}

		// Closer of the left boundary of the zone
		if (DistanceXEndBoundary > DistanceXBeginBoundary)
		{
			NewPos.X = BeforeXAvailable ? ForbiddenZone[0].X - SlotSize.X : ForbiddenZone[1].X;
		}
	}

	return NewPos;
}

void NButtonSequenceMovementManager::ComputeForbiddenZone()
{
	if (WheelButton == nullptr)
	{
		return;
	}

	const FGeometry GeoWheel = WheelButton->GetCachedGeometry();
	const FVector2D NormalCoord = FVector2D(0, 0);
	const FVector2D InitCoord = GeoWheel.GetLocalPositionAtCoordinates(NormalCoord);

	if (ForbiddenZone.Num() > 0 && ForbiddenZone[0] == InitCoord && ForbiddenZone[1] == InitCoord + GeoWheel.
		GetLocalSize())
	{
		return;
	}
	ForbiddenZone.Empty();
	ForbiddenZone.Add(InitCoord);
	ForbiddenZone.Add(InitCoord + GeoWheel.GetLocalSize());
}

void NButtonSequenceMovementManager::ChangeVelocity(UWheelButtonWidget* _WheelButton, EWheelDirection Direction)
{
	float Clock = 0;
	float Anti = 0;
	_WheelButton->GetWheelDistRatio(Clock, Anti);
	/// float Divider = Direction == EWheelDirection::Clockwise ? Clock : Anti;
	VelocityClock = FMath::Clamp(InitialVelocity / Clock, 0.f, InitialVelocity);
	VelocityAntiClock = FMath::Clamp(InitialVelocity / Anti, 0.f, InitialVelocity);
}

void NButtonSequenceMovementManager::ResetVelocity()
{
	if (WheelButton->GetResetDistanceOnDrop())
	{
		VelocityClock = InitialVelocity;
		VelocityAntiClock = InitialVelocity;
	}
}

#undef LOCTEXT_NAMESPACE
