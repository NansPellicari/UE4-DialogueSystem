
#include "Editor/Customization/ResponseCategoryCustomization.h"

#include "NansUE4Utilities/public/Misc/TextLibrary.h"
#include "PropertyEditor/Public/DetailLayoutBuilder.h"
#include "PropertyEditor/Public/DetailWidgetRow.h"
#include "PropertyEditor/Public/PropertyHandle.h"
#include "SGraphPin.h"
#include "SNameComboBox.h"
#include "SlateBasics.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SWidget.h"

#define LOCTEXT_NAMESPACE "DialogSystem"

TSharedRef<IPropertyTypeCustomization> FNResponseCategoryCustomization::MakeInstance()
{
	return MakeShareable(new FNResponseCategoryCustomization());
}

void FNResponseCategoryCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle,
	class FDetailWidgetRow& HeaderRow,
	IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	NameProperty = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FNResponseCategory, Name));

	check(NameProperty.IsValid());

	FString Val;
	NameProperty->GetValueAsDisplayString(Val);
	FName NameSelected = FName(*Val);

	TArray<FNDialogResponseCategorySettings> Settings;
	UDialogSystemSettings::GetResponseCategoryConfigs(Settings);
	int32 Index = 0;
	CategoryList.Empty();
	TSharedPtr<FName> InitialSelectedName;

	for (const auto& Setting : Settings)
	{
		CategoryList.Add(MakeShareable(new FName(Setting.Name)));
		if (NameSelected == Setting.Name)
		{
			InitialSelectedName = CategoryList[Index];
		}
		Index++;
	}

	Settings.Empty();

	OnAttributeSelected(MakeShareable(new FName(*Val)), ESelectInfo::Direct);

	HeaderRow.NameContent()[StructPropertyHandle->CreatePropertyNameWidget()]
		.ValueContent()[SAssignNew(NameComboBox, SNameComboBox)	   // note you can display any widget here
							.ContentPadding(FMargin(6.0f, 2.0f))
							.OptionsSource(&CategoryList)
							.InitiallySelectedItem(InitialSelectedName)
							.OnSelectionChanged(this, &FNResponseCategoryCustomization::OnAttributeSelected)];
}

void FNResponseCategoryCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle,
	class IDetailChildrenBuilder& StructBuilder,
	IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
}
void FNResponseCategoryCustomization::OnAttributeSelected(TSharedPtr<FName> Selection, ESelectInfo::Type SelectInfo)
{
	if (NameProperty.IsValid())
	{
		FPropertyAccess::Result Result = NameProperty->SetValueFromFormattedString(Selection->ToString());
		// FString StrResult = Result == FPropertyAccess::Fail ? "Fail" : "Success";
		// StrResult = Result == FPropertyAccess::MultipleValues ? "MultipleValues" : StrResult;
		// UE_LOG(LogTemp, Warning, TEXT("%s: %s - access: %s"), ANSI_TO_TCHAR(__FUNCTION__), *Selection->ToString(), *StrResult);
	}
}

#undef LOCTEXT_NAMESPACE
