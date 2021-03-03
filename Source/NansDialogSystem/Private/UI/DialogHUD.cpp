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

void UDialogHUD::StartDisplayResponse(FText Response, FText Title)
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
