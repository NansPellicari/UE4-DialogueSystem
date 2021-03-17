// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WheelButtonWidget.h"	 // TODO remove this dependency, EWheelDirection should be in a light files dedicated for types

class UPanelWidget;
class UCanvasPanelSlot;
class UButtonSequenceWidget;
class UWheelButtonWidget;

class NANSDIALOGUESYSTEM_API NButtonSequenceMovementManager
{
public:
	void Initialize(UPanelWidget* _ButtonsSlot, float _Velocity, UWheelButtonWidget* _WheelButton = nullptr);
	void Reset();
	void MoveButtons(float DeltaSeconds);
	void PlaceButtons();
	void ChangeVelocity(UWheelButtonWidget* _WheelButton, EWheelDirection Direction);
	void ResetVelocity();

private:
	UPanelWidget* ButtonsSlot;
	UWheelButtonWidget* WheelButton;
	TMap<FString, FVector2D> ButtonsPositions;
	TArray<FVector2D, TFixedAllocator<2>> ForbiddenZone;

	float InitialVelocity = 0;
	float VelocityClock = 0;
	float VelocityAntiClock = 0;

	bool InZone(const TArray<FVector2D, TFixedAllocator<2>>& Zone, const FVector2D& Pos, const FVector2D& Size) const;
	bool InButtonsZone(const int32 Position,
		const UButtonSequenceWidget* Button,
		TMap<FString, UCanvasPanelSlot*>& OverlappedBt,
		FVector2D& Centroid) const;
	FVector2D FindRandomCoordInSlot(UButtonSequenceWidget* Button) const;
	void ComputeForbiddenZone();
};
