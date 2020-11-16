
#include "Customization/DialogFactorSettingsCustomization.h"

#include "Editor/PropertyEditor/Private/PropertyEditorHelpers.h"
#include "IDetailChildrenBuilder.h"
#include "NansDialogSystem/Public/BTDialogueTypes.h"
#include "NansUE4Utilities/public/Misc/TextLibrary.h"
#include "PropertyCustomizationHelpers.h"
#include "PropertyEditor/Public/DetailLayoutBuilder.h"
#include "PropertyEditor/Public/DetailWidgetRow.h"
#include "PropertyEditor/Public/PropertyHandle.h"
#include "SGraphPin.h"
#include "SNameComboBox.h"
#include "SlateBasics.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SWidget.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"

#define LOCTEXT_NAMESPACE "DialogSystem"

TSharedRef<IPropertyTypeCustomization> FNDialogFactorSettingsCustomization::MakeInstance()
{
	return MakeShareable(new FNDialogFactorSettingsCustomization());
}

void FNDialogFactorSettingsCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle,
	class FDetailWidgetRow& HeaderRow,
	IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	StructHandle = StructPropertyHandle;
}

// inspired from this: Engine\Source\Editor\DetailCustomizations\Private\InputStructCustomization.cpp
void FNDialogFactorSettingsCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle,
	class IDetailChildrenBuilder& StructBuilder,
	IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	TSharedPtr<IPropertyHandle> CategoryProperty =
		StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FNDialogFactorSettings, Category));
	TSharedPtr<IPropertyHandle> MultiplierProperty =
		StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FNDialogFactorSettings, Multiplier));
	TSharedPtr<IPropertyHandle> LhRangeProperty =
		StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FNDialogFactorSettings, RangeFrom));
	TSharedPtr<IPropertyHandle> RhRangeProperty =
		StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FNDialogFactorSettings, RangeTo));

	check(CategoryProperty.IsValid() && MultiplierProperty.IsValid() && LhRangeProperty.IsValid() && RhRangeProperty.IsValid());

	const TSharedPtr<IPropertyHandleArray> ParentArrayHandle = StructPropertyHandle->GetParentHandle()->AsArray();

	TSharedRef<SWidget> RemoveButton =
		(ParentArrayHandle.IsValid()
				? PropertyCustomizationHelpers::MakeDeleteButton(
					  FSimpleDelegate::CreateSP(this, &FNDialogFactorSettingsCustomization::RemoveButton_OnClick),
					  LOCTEXT("RemoveToolTip", "Removes Dialog Factor"))
				: SNullWidget::NullWidget);

	TSharedRef<SWidget> AddButton =
		(ParentArrayHandle.IsValid() ? PropertyCustomizationHelpers::MakeAddButton(
										   FSimpleDelegate::CreateSP(this, &FNDialogFactorSettingsCustomization::AddButton_OnClick),
										   LOCTEXT("AddToolTip", "Add a Dialog Factor"))
									 : SNullWidget::NullWidget);

	TSharedRef<SWidget> UpButton =
		(ParentArrayHandle.IsValid()
				? MakeUpButton(FSimpleDelegate::CreateSP(this, &FNDialogFactorSettingsCustomization::UpButton_OnClick),
					  LOCTEXT("UpToolTip", "Up to the list Factor"),
					  MakeAttributeLambda([=] { return StructPropertyHandle->GetIndexInArray() > 0; }))
				: SNullWidget::NullWidget);

	TSharedRef<SWidget> DownButton =
		(ParentArrayHandle.IsValid()
				? MakeDownButton(FSimpleDelegate::CreateSP(this, &FNDialogFactorSettingsCustomization::DownButton_OnClick),
					  LOCTEXT("DownToolTip", "Down to the list Factor"),
					  MakeAttributeLambda([=] {
						  if (ParentArrayHandle.IsValid())
						  {
							  uint32 NumEls;
							  ParentArrayHandle->GetNumElements(NumEls);
							  return (uint32) StructPropertyHandle->GetIndexInArray() < NumEls - 1;
						  }
						  return false;
					  }))
				: SNullWidget::NullWidget);

	CategoryProperty->GetChildHandle(FName("Name"))
		->SetOnPropertyValueChanged(
			FSimpleDelegate::CreateSP(this, &FNDialogFactorSettingsCustomization::OnCategoryChanged, CategoryProperty));

	BgColor = GetColor(CategoryProperty);

	// @formatter:off
	StructBuilder.AddCustomRow( LOCTEXT("DialogFactorRow", "DialogFactor") )
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

void FNDialogFactorSettingsCustomization::OnCategoryChanged(TSharedPtr<IPropertyHandle> CategoryHandle)
{
	BgColor = GetColor(CategoryHandle);
}

FLinearColor FNDialogFactorSettingsCustomization::GetColor(TSharedPtr<IPropertyHandle> CategoryHandle) const
{
	FLinearColor Color = FLinearColor(0, 0, 0);
	if (CategoryHandle->GetChildHandle(FName("Name")).IsValid())
	{
		FString CategoryValue;
		CategoryHandle->GetChildHandle(FName("Name"))->GetValueAsFormattedString(CategoryValue);
		FNResponseCategory Cat;
		Cat.Name = FName(*CategoryValue);
		if (!CategoryValue.IsEmpty() && Cat.Name != NAME_None)
		{
			Color = Cat.GetColor();
		}
	}
	return Color;
}

TSharedRef<SWidget> FNDialogFactorSettingsCustomization::MakeUpButton(
	FSimpleDelegate OnClicked, TAttribute<FText> OptionalToolTipText, TAttribute<bool> IsEnabled)
{
	return SNew(SButton)
		.ButtonStyle(FEditorStyle::Get(), "HoverHintOnly")
		.OnClicked_Lambda([OnClicked]() -> FReply {
			OnClicked.ExecuteIfBound();
			return FReply::Handled();
		})
		.Text(LOCTEXT("UpButtonLabel", "Up"))
		.ToolTipText(OptionalToolTipText.Get().IsEmpty() ? LOCTEXT("UpButtonToolTipText", "Ups Element") : OptionalToolTipText)
		.ContentPadding(4.0f)
		.IsEnabled(IsEnabled)
		.ForegroundColor(FSlateColor::UseForeground())
		.IsFocusable(false)[SNew(SImage).Image(FEditorStyle::GetBrush("ArrowUp")).ColorAndOpacity(FSlateColor::UseForeground())];
}

TSharedRef<SWidget> FNDialogFactorSettingsCustomization::MakeDownButton(
	FSimpleDelegate OnClicked, TAttribute<FText> OptionalToolTipText, TAttribute<bool> IsEnabled)
{
	return SNew(SButton)
		.ButtonStyle(FEditorStyle::Get(), "HoverHintOnly")
		.OnClicked_Lambda([OnClicked]() -> FReply {
			OnClicked.ExecuteIfBound();
			return FReply::Handled();
		})
		.Text(LOCTEXT("DownButtonLabel", "Down"))
		.ToolTipText(OptionalToolTipText.Get().IsEmpty() ? LOCTEXT("DownButtonToolTipText", "Downs Element") : OptionalToolTipText)
		.ContentPadding(4.0f)
		.IsEnabled(IsEnabled)
		.ForegroundColor(FSlateColor::UseForeground())
		.IsFocusable(false)[SNew(SImage).Image(FEditorStyle::GetBrush("ArrowDown")).ColorAndOpacity(FSlateColor::UseForeground())];
}

void FNDialogFactorSettingsCustomization::DownButton_OnClick()
{
	if (StructHandle->IsValidHandle())
	{
		const TSharedPtr<IPropertyHandle> ParentHandle = StructHandle->GetParentHandle();
		const TSharedPtr<IPropertyHandleArray> ParentArrayHandle = ParentHandle->AsArray();

		int32 Index = StructHandle->GetIndexInArray();

		FPropertyAccess::Result Res =
			ParentArrayHandle->MoveElementTo(Index, Index + 2);	   // +2 is weird, but this is how it works!
	}
}

void FNDialogFactorSettingsCustomization::UpButton_OnClick()
{
	if (StructHandle->IsValidHandle())
	{
		const TSharedPtr<IPropertyHandle> ParentHandle = StructHandle->GetParentHandle();
		const TSharedPtr<IPropertyHandleArray> ParentArrayHandle = ParentHandle->AsArray();

		int32 Index = StructHandle->GetIndexInArray();

		ParentArrayHandle->MoveElementTo(Index, Index - 1);
	}
}

void FNDialogFactorSettingsCustomization::RemoveButton_OnClick()
{
	if (StructHandle->IsValidHandle())
	{
		const TSharedPtr<IPropertyHandle> ParentHandle = StructHandle->GetParentHandle();
		const TSharedPtr<IPropertyHandleArray> ParentArrayHandle = ParentHandle->AsArray();

		ParentArrayHandle->DeleteItem(StructHandle->GetIndexInArray());
	}
}

void FNDialogFactorSettingsCustomization::AddButton_OnClick()
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
