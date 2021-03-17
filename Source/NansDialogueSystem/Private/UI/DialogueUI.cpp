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

#include "UI/DialogueUI.h"

#include "Components/PanelWidget.h"
#include "Misc/ErrorUtils.h"
#include "Misc/TextLibrary.h"
#include "UI/ResponseButtonWidget.h"

#define LOCTEXT_NAMESPACE "DialogueSystem"

void UDialogueUI::NativePreConstruct()
{
	UClass* ButtonClass = ResponseButtonWidget.Get();
	if (!ButtonClass)
	{
		EDITOR_WARN(
			"DialogueSystem",
			LOCTEXT("MissingResponseButtonWidget", "You should set a ResponseButtonWidget class")
		);
	}
}

void UDialogueUI::NativeDestruct()
{
	Reset();
	Super::NativeDestruct();
}

void UDialogueUI::Reset_Implementation()
{
	TimeDisplayed = 0.f;
	RemoveResponses();
}

void UDialogueUI::DisplayMessage(EDialogMessageType MessageType)
{
	if (UIPlayerBox == nullptr || UINPCBox == nullptr) return;

	if (MessageType == EDialogMessageType::Question)
	{
		UIPlayerBox->SetVisibility(ESlateVisibility::Collapsed);
		UINPCBox->SetVisibility(ESlateVisibility::Visible);
		return;
	}

	UIPlayerBox->SetVisibility(ESlateVisibility::Visible);
	UINPCBox->SetVisibility(ESlateVisibility::Collapsed);
}

UWheelButtonWidget* UDialogueUI::GetWheelButton() const { return WheelButton; }

UResponseButtonWidget* UDialogueUI::AddNewResponse(FResponseButtonBuilderData& BuilderData)
{
	UClass* ButtonClass = ResponseButtonWidget.Get();
	if (!ButtonClass)
	{
		UE_LOG(LogTemp, Error, TEXT("%s You must set a ResponseButtonWidget class"), ANSI_TO_TCHAR(__FUNCTION__));
		return nullptr;
	}
	UResponseButtonWidget* ButtonWidget = CreateWidget<UResponseButtonWidget>(GetWorld(), ResponseButtonWidget);

	ButtonWidget->SetResponse(BuilderData.Response);
	ButtonWidget->InfluencedBy = BuilderData.InfluencedBy;
	ButtonWidget->DisplayOrder = BuilderData.DisplayOrder;
	ButtonWidget->ComputeColor(BuilderData.MaxPoints);
	ResponsesSlot->AddChild(ButtonWidget);
	OnAddedChild.Broadcast(ButtonWidget);
	OnAddResponse(ButtonWidget);
	return ButtonWidget;
}

bool UDialogueUI::RemoveResponses()
{
	if (!IsValid(ResponsesSlot))
	{
		EDITOR_ERROR("DialogueSystem", LOCTEXT("NoResponseSlot", "No responses slot in "));
		return false;
	}

	while (ResponsesSlot->GetChildrenCount() > 0)
	{
		UUserWidget* Widget = Cast<UUserWidget>(ResponsesSlot->GetChildAt(0));

		if (Widget == nullptr)
		{
			EDITOR_ERROR(
				"DialogueSystem",
				LOCTEXT("WrongWidgetInResponsesSlot", "A Wrong object type is in the responses slot in ")
			);
			return false;
		}
		UResponseButtonWidget* Button = Cast<UResponseButtonWidget>(Widget);

		if (IsValid(Button))
		{
			Button->OnBTClicked.Clear();
			OnRemoveResponse(Button);
			OnRemovedChild.Broadcast(Button);
			Button->MarkPendingKill();
		}

		ResponsesSlot->RemoveChildAt(0);
	}
	return true;
}

int32 UDialogueUI::CountVisibleResponses() const
{
	TArray<UWidget*> Children = ResponsesSlot->GetAllChildren();
	int32 CountVisible = 0;
	for (UWidget* Child : Children)
	{
		CountVisible += Child->IsVisible() ? 1 : 0;
	}
	return CountVisible;
}

int32 UDialogueUI::CountResponses() const
{
	return ResponsesSlot->GetChildrenCount();
}

TArray<UResponseButtonWidget*> UDialogueUI::GetResponsesButttons() const
{
	TArray<UResponseButtonWidget*> Buttons;
	for (auto Child : ResponsesSlot->GetAllChildren())
	{
		Buttons.Add(Cast<UResponseButtonWidget>(Child));
	}
	return Buttons;
}

UPanelWidget* UDialogueUI::GetUINPCBox() const { return UINPCBox; }
UPanelWidget* UDialogueUI::GetResponsesSlot() const { return ResponsesSlot; }
UPanelWidget* UDialogueUI::GetUIPlayerBox() const { return UIPlayerBox; }
UButton* UDialogueUI::GetButtonPassPlayerLine() const { return ButtonPassPlayerLine; }
UButton* UDialogueUI::GetButtonPassNPCLine() const { return ButtonPassNPCLine; }
UDialogueProgressBarWidget* UDialogueUI::GetProgressBar() const { return ProgressBar; }

void UDialogueUI::ChangeButtonVisibility(ESlateVisibility InVisibility, int32 Index) const
{
	UResponseButtonWidget* Button = Cast<UResponseButtonWidget>(ResponsesSlot->GetChildAt(Index));
	if (Button == nullptr) return; // only concerned about button
	Button->SetVisibility(InVisibility);
}

void UDialogueUI::ChangeButtonsVisibility(ESlateVisibility InVisibility) const
{
	for (int32 i = 0; i < ResponsesSlot->GetChildrenCount(); ++i)
	{
		ChangeButtonVisibility(InVisibility, i);
	}
}

void UDialogueUI::SetNPCText_Implementation(const FText& InText, const FText& InTitle)
{
	CurrentNPCDialogue.Text = InText;
	CurrentNPCDialogue.Title = InTitle;

	// FIXME alert !! wrong data is passed here!!! should be CurrentNPCDialogue
	OnQuestion.Broadcast(CurrentPlayerDialogue.Text, CurrentPlayerDialogue.Title);
}

void UDialogueUI::SetPlayerText_Implementation(const FText& InText, const FText& InTitle)
{
	CurrentPlayerDialogue.Text = InText;
	CurrentPlayerDialogue.Title = InTitle;
	if (UIPlayerBox == nullptr)
	{
		OnEndDisplayResponse.Broadcast();
		return;
	}

	TimeDisplayed = TimeToDisplay;

	OnResponse.Broadcast(CurrentPlayerDialogue.Text, CurrentPlayerDialogue.Title);
}

void UDialogueUI::GetCurrentPlayerDialog(FText& OutText, FText& OutTitle)
{
	OutText = CurrentPlayerDialogue.Text;
	OutTitle = CurrentPlayerDialogue.Title;
}

void UDialogueUI::GetCurrentNPCDialog(FText& OutText, FText& OutTitle)
{
	OutText = CurrentNPCDialogue.Text;
	OutTitle = CurrentNPCDialogue.Title;
}

void UDialogueUI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
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
#undef LOCTEXT_NAMESPACE
