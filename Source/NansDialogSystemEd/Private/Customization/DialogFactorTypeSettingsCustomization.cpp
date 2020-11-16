
#include "Customization/DialogFactorTypeSettingsCustomization.h"

#include "IDetailChildrenBuilder.h"
#include "NansDialogSystem/Public/BTDialogueTypes.h"
#include "NansUE4Utilities/public/Misc/TextLibrary.h"
#include "PropertyCustomizationHelpers.h"
#include "PropertyEditor/Private/SSingleProperty.h"
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

TSharedRef<IPropertyTypeCustomization> FNDialogFactorTypeSettingsCustomization::MakeInstance()
{
	return MakeShareable(new FNDialogFactorTypeSettingsCustomization());
}

void FNDialogFactorTypeSettingsCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle,
	class FDetailWidgetRow& HeaderRow,
	IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	StructHandle = StructPropertyHandle;
}

// inspired from this: Engine\Source\Editor\DetailCustomizations\Private\InputStructCustomization.cpp
void FNDialogFactorTypeSettingsCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle,
	class IDetailChildrenBuilder& StructBuilder,
	IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	TSharedPtr<IPropertyHandle> FactorProperty =
		StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FNDialogFactorTypeSettings, Factor));
	TSharedPtr<IPropertyHandle> TypeProperty =
		StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FNDialogFactorTypeSettings, Type));
	TSharedPtr<IPropertyHandle> HasRangeProperty =
		StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FNDialogFactorTypeSettings, bHasRange));
	TSharedPtr<IPropertyHandle> RangeProperty =
		StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FNDialogFactorTypeSettings, Range));
	TSharedPtr<IPropertyHandle> LhRangeProperty = RangeProperty->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRange, Lh));
	TSharedPtr<IPropertyHandle> RhRangeProperty = RangeProperty->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRange, Rh));

	check(FactorProperty.IsValid() && TypeProperty.IsValid() && HasRangeProperty.IsValid() && RangeProperty.IsValid());

	const TSharedPtr<IPropertyHandleArray> ParentArrayHandle = StructPropertyHandle->GetParentHandle()->AsArray();

	TSharedRef<SWidget> RemoveButton =
		(ParentArrayHandle.IsValid()
				? PropertyCustomizationHelpers::MakeDeleteButton(
					  FSimpleDelegate::CreateSP(this, &FNDialogFactorTypeSettingsCustomization::RemoveButton_OnClick),
					  LOCTEXT("RemoveToolTip", "Removes Factor Type"))
				: SNullWidget::NullWidget);

	TSharedRef<SWidget> AddButton =
		(ParentArrayHandle.IsValid()
				? PropertyCustomizationHelpers::MakeAddButton(
					  FSimpleDelegate::CreateSP(this, &FNDialogFactorTypeSettingsCustomization::AddButton_OnClick),
					  LOCTEXT("AddToolTip", "Add a Factor Type"))
				: SNullWidget::NullWidget);

	// @formatter:off
	StructBuilder.AddCustomRow( LOCTEXT("FactorRow", "Factor") )
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SBorder)
			.BorderImage(FEditorStyle::GetBrush("ToolPanel.DarkGroupBorder"))
			.Content()
			// .BorderBackgroundColor(FLinearColor(255, 0, 0))
			[
				SNew(SHorizontalBox)
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
							FactorProperty->CreatePropertyNameWidget()
						]
						+ SVerticalBox::Slot()
						.AutoHeight()
						[
							StructBuilder.GenerateStructValueWidget( FactorProperty.ToSharedRef() )
						]
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
							TypeProperty->CreatePropertyNameWidget()
						]
						+ SVerticalBox::Slot()
						.AutoHeight()
						[
							TypeProperty->CreatePropertyValueWidget()
						]
					]
				]
				+SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(10.f, 5.f)
				.VAlign(VAlign_Center)
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.Padding(5.f, 0.f)
						.AutoWidth()
						[
							SNew( SCheckBox )
							.OnCheckStateChanged( this, &FNDialogFactorTypeSettingsCustomization::OnCheckStateChanged, HasRangeProperty )
							.IsChecked( this, &FNDialogFactorTypeSettingsCustomization::OnGetCheckState, HasRangeProperty, TypeProperty )
							.IsEnabled(MakeAttributeLambda([=] {
								int32 Type;
								TypeProperty->GetValue(Type);
								return (Type & (int32) EFactorType::Difficulty) == 0;
							}))
						]
						+ SHorizontalBox::Slot()
						.Padding(0, 0.f, 5.f, 0.f)
						.AutoWidth()
						[
							HasRangeProperty->CreatePropertyNameWidget()
						]
					]
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SHorizontalBox)
						.IsEnabled(MakeAttributeLambda([=] { return OnGetCheckState(HasRangeProperty, TypeProperty) == ECheckBoxState::Checked; }))
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

void FNDialogFactorTypeSettingsCustomization::RemoveButton_OnClick()
{
	if (StructHandle->IsValidHandle())
	{
		const TSharedPtr<IPropertyHandle> ParentHandle = StructHandle->GetParentHandle();
		const TSharedPtr<IPropertyHandleArray> ParentArrayHandle = ParentHandle->AsArray();

		ParentArrayHandle->DeleteItem(StructHandle->GetIndexInArray());
	}
}
void FNDialogFactorTypeSettingsCustomization::AddButton_OnClick()
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

void FNDialogFactorTypeSettingsCustomization::OnCheckStateChanged(
	ECheckBoxState InNewState, TSharedPtr<IPropertyHandle> PropertyHandle)
{
	if (PropertyHandle->IsValidHandle())
	{
		PropertyHandle->SetValue(InNewState == ECheckBoxState::Checked);
	}
}

ECheckBoxState FNDialogFactorTypeSettingsCustomization::OnGetCheckState(
	TSharedPtr<IPropertyHandle> PropertyHandle, TSharedPtr<IPropertyHandle> TypePropertyHandle) const
{
	bool bShouldBeChecked = false;
	ECheckBoxState State = ECheckBoxState::Unchecked;
	if (TypePropertyHandle->IsValidHandle())
	{
		int32 Type;
		TypePropertyHandle->GetValue(Type);
		if ((Type & (int32) EFactorType::Difficulty) != 0)
		{
			bShouldBeChecked = true;
			State = ECheckBoxState::Checked;
		}
	}

	if (PropertyHandle->IsValidHandle())
	{
		bool EnabledValue;
		if (PropertyHandle->GetValue(EnabledValue) == FPropertyAccess::Success)
		{
			if (EnabledValue)
			{
				State = ECheckBoxState::Checked;
			}
		}

		if (bShouldBeChecked)
		{
			PropertyHandle->SetValue(true);
		}
	}

	return State;
}

#undef LOCTEXT_NAMESPACE
