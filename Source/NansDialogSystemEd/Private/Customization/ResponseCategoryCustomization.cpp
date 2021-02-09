//  Copyright 2020-present Nans Pellicari (nans.pellicari@gmail.com).
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

#include "Customization/ResponseCategoryCustomization.h"

#include "SGraphPin.h"
#include "SlateBasics.h"
#include "SNameComboBox.h"
#include "NansUE4Utilities/public/Misc/TextLibrary.h"
#include "PropertyEditor/Public/DetailLayoutBuilder.h"
#include "PropertyEditor/Public/DetailWidgetRow.h"
#include "PropertyEditor/Public/PropertyHandle.h"
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
	// Because it's in format (TagName="My.Tag")
	Val.Split("=", nullptr, &Val);
	Val = Val.LeftChop(2);
	Val = Val.RightChop(1);
	FName NameSelected = FName(*Val);

	TArray<FNDialogResponseCategorySettings> Settings;
	UDialogSystemSettings::Get()->GetResponseCategoryConfigs(Settings);
	int32 Index = 0;
	CategoryList.Empty();
	TSharedPtr<FName> InitialSelectedName;

	for (const auto& Setting : Settings)
	{
		CategoryList.Add(MakeShareable(new FName(Setting.Name.ToString())));
		if (NameSelected.ToString() == Setting.Name.ToString())
		{
			InitialSelectedName = CategoryList[Index];
		}
		Index++;
	}

	Settings.Empty();

	OnAttributeSelected(MakeShareable(new FName(*Val)), ESelectInfo::Direct);

	HeaderRow.NameContent()[StructPropertyHandle->CreatePropertyNameWidget()]
		.ValueContent()[SAssignNew(NameComboBox, SNameComboBox) // note you can display any widget here
							.ContentPadding(FMargin(6.0f, 2.0f))
							.OptionsSource(&CategoryList)
							.InitiallySelectedItem(InitialSelectedName)
							.OnSelectionChanged(this, &FNResponseCategoryCustomization::OnAttributeSelected)];
}

void FNResponseCategoryCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle,
	class IDetailChildrenBuilder& StructBuilder,
	IPropertyTypeCustomizationUtils& StructCustomizationUtils) {}

void FNResponseCategoryCustomization::OnAttributeSelected(TSharedPtr<FName> Selection, ESelectInfo::Type SelectInfo)
{
	if (NameProperty.IsValid())
	{
		FString TagString = TEXT("(");
		TagString += TEXT("TagName=\"");
		TagString += Selection->ToString();
		TagString += TEXT("\"");
		TagString += TEXT(")");
		FPropertyAccess::Result Result = NameProperty->SetValueFromFormattedString(TagString);
		// FString StrResult = Result == FPropertyAccess::Fail ? "Fail" : "Success";
		// StrResult = Result == FPropertyAccess::MultipleValues ? "MultipleValues" : StrResult;
		// UE_LOG(LogTemp, Warning, TEXT("%s: %s - access: %s"), ANSI_TO_TCHAR(__FUNCTION__), *Selection->ToString(), *StrResult);
	}
}

#undef LOCTEXT_NAMESPACE
