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

#include "Customization/DialogueFactorSettingsCustomization.h"

#include "IDetailChildrenBuilder.h"
#include "PropertyCustomizationHelpers.h"
#include "SGraphPin.h"
#include "SlateBasics.h"
#include "SNameComboBox.h"
#include "Editor/PropertyEditor/Private/PropertyEditorHelpers.h"
#include "NansDialogueSystem/Public/BTDialogueTypes.h"
#include "NansUE4Utilities/public/Misc/TextLibrary.h"
#include "PropertyEditor/Public/DetailLayoutBuilder.h"
#include "PropertyEditor/Public/DetailWidgetRow.h"
#include "PropertyEditor/Public/PropertyHandle.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SWidget.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"

#define LOCTEXT_NAMESPACE "DialogueSystem"

TSharedRef<IPropertyTypeCustomization> FNDialogueFactorSettingsCustomization::MakeInstance()
{
	return MakeShareable(new FNDialogueFactorSettingsCustomization());
}

void FNDialogueFactorSettingsCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle,
	class FDetailWidgetRow& HeaderRow,
	IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	StructHandle = StructPropertyHandle;
}

// inspired from this: Engine\Source\Editor\DetailCustomizations\Private\InputStructCustomization.cpp
void FNDialogueFactorSettingsCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle,
	class IDetailChildrenBuilder& StructBuilder,
	IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	TSharedPtr<IPropertyHandle> CategoryProperty =
		StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FNDialogueFactorSettings, Category));
	TSharedPtr<IPropertyHandle> MultiplierProperty =
		StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FNDialogueFactorSettings, Multiplier));
	TSharedPtr<IPropertyHandle> LhRangeProperty =
		StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FNDialogueFactorSettings, RangeFrom));
	TSharedPtr<IPropertyHandle> RhRangeProperty =
		StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FNDialogueFactorSettings, RangeTo));

	check(
		CategoryProperty.IsValid() && MultiplierProperty.IsValid() && LhRangeProperty.IsValid() && RhRangeProperty.
		IsValid()
	);

	const TSharedPtr<IPropertyHandleArray> ParentArrayHandle = StructPropertyHandle->GetParentHandle()->AsArray();

	TSharedRef<SWidget> RemoveButton =
	(ParentArrayHandle.IsValid()
		 ? PropertyCustomizationHelpers::MakeDeleteButton(
			 FSimpleDelegate::CreateSP(this, &FNDialogueFactorSettingsCustomization::RemoveButton_OnClick),
			 LOCTEXT("RemoveToolTip", "Removes Dialogue Factor")
		 )
		 : SNullWidget::NullWidget);

	TSharedRef<SWidget> AddButton =
	(ParentArrayHandle.IsValid()
		 ? PropertyCustomizationHelpers::MakeAddButton(
			 FSimpleDelegate::CreateSP(this, &FNDialogueFactorSettingsCustomization::AddButton_OnClick),
			 LOCTEXT("AddToolTip", "Add a Dialogue Factor")
		 )
		 : SNullWidget::NullWidget);

	TSharedRef<SWidget> UpButton =
	(ParentArrayHandle.IsValid()
		 ? MakeUpButton(
			 FSimpleDelegate::CreateSP(this, &FNDialogueFactorSettingsCustomization::UpButton_OnClick),
			 LOCTEXT("UpToolTip", "Up to the list Factor"),
			 MakeAttributeLambda([=] { return StructPropertyHandle->GetIndexInArray() > 0; })
		 )
		 : SNullWidget::NullWidget);

	TSharedRef<SWidget> DownButton =
	(ParentArrayHandle.IsValid()
		 ? MakeDownButton(
			 FSimpleDelegate::CreateSP(this, &FNDialogueFactorSettingsCustomization::DownButton_OnClick),
			 LOCTEXT("DownToolTip", "Down to the list Factor"),
			 MakeAttributeLambda(
				 [=]
				 {
					 if (ParentArrayHandle.IsValid())
					 {
						 uint32 NumEls;
						 ParentArrayHandle->GetNumElements(NumEls);
						 return static_cast<uint32>(StructPropertyHandle->GetIndexInArray()) < NumEls - 1;
					 }
					 return false;
				 }
			 )
		 )
		 : SNullWidget::NullWidget);

	CategoryProperty->GetChildHandle(FName("Name"))
					->SetOnPropertyValueChanged(
						FSimpleDelegate::CreateSP(
							this,
							&FNDialogueFactorSettingsCustomization::OnCategoryChanged,
							CategoryProperty
						)
					);

	BgColor = GetColor(CategoryProperty);

	// @formatter:off
	StructBuilder.AddCustomRow( LOCTEXT("DialogueFactorRow", "DialogueFactor") )
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SBorder)
			.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
			.BorderBackgroundColor(MakeAttributeLambda([=] { return FSlateColor(BgColor); }))
			.Content()
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(5.f)
				.VAlign(VAlign_Center)
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						UpButton
					]
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						DownButton
					]
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(5.f, 0.f)
				.VAlign(VAlign_Center)
				[
					SNew(SBox)
					.MinDesiredWidth(150.f)
					[
						SNew(SVerticalBox)
						+ SVerticalBox::Slot()
						.AutoHeight()
						[
							CategoryProperty->CreatePropertyNameWidget()
						]
						+ SVerticalBox::Slot()
						.AutoHeight()
						[
							StructBuilder.GenerateStructValueWidget(CategoryProperty.ToSharedRef())
						]
					]
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(5.f, 0.f)
				.VAlign(VAlign_Center)
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						MultiplierProperty->CreatePropertyNameWidget()
					]
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						MultiplierProperty->CreatePropertyValueWidget()
					]
				]
				+SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(10.f, 5.f)
				.VAlign(VAlign_Center)
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.Padding(5.f)
					.AutoHeight()
					[
						// Show the name of the asset or actor
						SNew(STextBlock)
						.Font(FEditorStyle::GetFontStyle("PropertyWindow.NormalFont"))
						.Text(LOCTEXT("Range", "Range"))
					]
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SHorizontalBox)
						+SHorizontalBox::Slot()
						.AutoWidth()
						.Padding(5.f, 0.f)
						[
							SNew(SBox)
							.MinDesiredWidth(100.f)
							[
								LhRangeProperty->CreatePropertyValueWidget()
							]
						]
						+SHorizontalBox::Slot()
						.AutoWidth()
						.Padding(5.f, 0.f)
						[
							SNew(SBox)
							.MinDesiredWidth(100.f)
							[
								RhRangeProperty->CreatePropertyValueWidget()
							]
						]
					]
				]
				+SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(5.f)
				.VAlign(VAlign_Center)
				[
					AddButton
				]
				+SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(5.f)
				.VAlign(VAlign_Center)
				[
					RemoveButton
				]
			]
		]
	];
	// @formatter:on
}

void FNDialogueFactorSettingsCustomization::OnCategoryChanged(TSharedPtr<IPropertyHandle> CategoryHandle)
{
	BgColor = GetColor(CategoryHandle);
}

FLinearColor FNDialogueFactorSettingsCustomization::GetColor(TSharedPtr<IPropertyHandle> CategoryHandle) const
{
	FLinearColor Color = FLinearColor(0, 0, 0);
	if (CategoryHandle->GetChildHandle(FName("Name")).IsValid())
	{
		FString CategoryValue;
		CategoryHandle->GetChildHandle(FName("Name"))->GetValueAsFormattedString(CategoryValue);
		// Because it's in format (TagName="My.Tag")
		CategoryValue.Split("=", nullptr, &CategoryValue);
		CategoryValue = CategoryValue.LeftChop(2);
		CategoryValue = CategoryValue.RightChop(1);
		FNDialogueCategory Cat;
		Cat.Name = FGameplayTag::RequestGameplayTag(*CategoryValue);
		if (!CategoryValue.IsEmpty() && Cat.Name.IsValid())
		{
			Color = FNDialogueCategory::GetColorFromSettings(Cat);
		}
	}
	return Color;
}

TSharedRef<SWidget> FNDialogueFactorSettingsCustomization::MakeUpButton(
	FSimpleDelegate OnClicked, TAttribute<FText> OptionalToolTipText, TAttribute<bool> IsEnabled)
{
	return SNew(SButton)
		.ButtonStyle(FEditorStyle::Get(), "HoverHintOnly")
		.OnClicked_Lambda(
							[OnClicked]() -> FReply
							{
								OnClicked.ExecuteIfBound();
								return FReply::Handled();
							}
						)
		.Text(LOCTEXT("UpButtonLabel", "Up"))
		.ToolTipText(
							OptionalToolTipText.Get().IsEmpty()
								? LOCTEXT("UpButtonToolTipText", "Ups Element")
								: OptionalToolTipText
						)
		.ContentPadding(4.0f)
		.IsEnabled(IsEnabled)
		.ForegroundColor(FSlateColor::UseForeground())
		.IsFocusable(false)[SNew(SImage).Image(FEditorStyle::GetBrush("ArrowUp")).ColorAndOpacity(
								FSlateColor::UseForeground()
							)];
}

TSharedRef<SWidget> FNDialogueFactorSettingsCustomization::MakeDownButton(
	FSimpleDelegate OnClicked, TAttribute<FText> OptionalToolTipText, TAttribute<bool> IsEnabled)
{
	return SNew(SButton)
		.ButtonStyle(FEditorStyle::Get(), "HoverHintOnly")
		.OnClicked_Lambda(
							[OnClicked]() -> FReply
							{
								OnClicked.ExecuteIfBound();
								return FReply::Handled();
							}
						)
		.Text(LOCTEXT("DownButtonLabel", "Down"))
		.ToolTipText(
							OptionalToolTipText.Get().IsEmpty()
								? LOCTEXT("DownButtonToolTipText", "Downs Element")
								: OptionalToolTipText
						)
		.ContentPadding(4.0f)
		.IsEnabled(IsEnabled)
		.ForegroundColor(FSlateColor::UseForeground())
		.IsFocusable(false)[SNew(SImage).Image(FEditorStyle::GetBrush("ArrowDown")).ColorAndOpacity(
								FSlateColor::UseForeground()
							)];
}

void FNDialogueFactorSettingsCustomization::DownButton_OnClick()
{
	if (StructHandle->IsValidHandle())
	{
		const TSharedPtr<IPropertyHandle> ParentHandle = StructHandle->GetParentHandle();
		const TSharedPtr<IPropertyHandleArray> ParentArrayHandle = ParentHandle->AsArray();

		int32 Index = StructHandle->GetIndexInArray();

		FPropertyAccess::Result Res =
			ParentArrayHandle->MoveElementTo(Index, Index + 2); // +2 is weird, but this is how it works!
	}
}

void FNDialogueFactorSettingsCustomization::UpButton_OnClick()
{
	if (StructHandle->IsValidHandle())
	{
		const TSharedPtr<IPropertyHandle> ParentHandle = StructHandle->GetParentHandle();
		const TSharedPtr<IPropertyHandleArray> ParentArrayHandle = ParentHandle->AsArray();

		int32 Index = StructHandle->GetIndexInArray();

		ParentArrayHandle->MoveElementTo(Index, Index - 1);
	}
}

void FNDialogueFactorSettingsCustomization::RemoveButton_OnClick()
{
	if (StructHandle->IsValidHandle())
	{
		const TSharedPtr<IPropertyHandle> ParentHandle = StructHandle->GetParentHandle();
		const TSharedPtr<IPropertyHandleArray> ParentArrayHandle = ParentHandle->AsArray();

		ParentArrayHandle->DeleteItem(StructHandle->GetIndexInArray());
	}
}

void FNDialogueFactorSettingsCustomization::AddButton_OnClick()
{
	if (StructHandle->IsValidHandle())
	{
		const TSharedPtr<IPropertyHandle> ParentHandle = StructHandle->GetParentHandle();
		const TSharedPtr<IPropertyHandleArray> ParentArrayHandle = ParentHandle->AsArray();

		uint32 NumEls;
		ParentArrayHandle->GetNumElements(NumEls);

		if (StructHandle->GetIndexInArray() == NumEls - 1)
		{
			ParentArrayHandle->AddItem();
		}
		else
		{
			ParentArrayHandle->Insert(StructHandle->GetIndexInArray() + 1);
		}
	}
}

#undef LOCTEXT_NAMESPACE
