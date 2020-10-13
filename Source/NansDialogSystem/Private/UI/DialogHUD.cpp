// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/DialogHUD.h"

#include "Components/Border.h"
#include "Components/PanelWidget.h"

void UDialogHUD::NativeOnInitialized()
{
	OnResponse.AddUniqueDynamic(this, &UDialogHUD::StartDisplayResponse);
}

void UDialogHUD::Reset_Implementation()
{
	TimeDisplayed = 0.f;
}

void UDialogHUD::InitiliazedUI(UPanelWidget* _UIResponseBox, UPanelWidget* _UIQuestionBox)
{
	UIResponseBox = _UIResponseBox;
	UIQuestionBox = _UIQuestionBox;
}

void UDialogHUD::DisplayMessage(EDialogMessageType MessageType)
{
	if (UIResponseBox == nullptr || UIQuestionBox == nullptr) return;

	if (MessageType == EDialogMessageType::Question)
	{
		UIResponseBox->SetVisibility(ESlateVisibility::Collapsed);
		UIQuestionBox->SetVisibility(ESlateVisibility::Visible);
		return;
	}

	UIResponseBox->SetVisibility(ESlateVisibility::Visible);
	UIQuestionBox->SetVisibility(ESlateVisibility::Collapsed);
}

void UDialogHUD::StartDisplayResponse(FText Response)
{
	if (UIResponseBox == nullptr)
	{
		OnEndDisplayResponse.Broadcast();
		return;
	}

	TimeDisplayed = TimeToDisplay;
}

void UDialogHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (TimeDisplayed <= 0)
	{
		return;
	}

	TimeDisplayed -= InDeltaTime;

	if (TimeDisplayed <= 0)
	{
		OnEndDisplayResponse.Broadcast();
		Reset();
	}
}
