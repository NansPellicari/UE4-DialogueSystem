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

#include "Customization/CurrentOrAllCustomization.h"


#include "IDetailChildrenBuilder.h"
#include "SGraphPin.h"
#include "SlateBasics.h"
#include "SNameComboBox.h"
#include "Dialogue/DialogueHistorySearch.h"
#include "NansUE4Utilities/public/Misc/TextLibrary.h"
#include "PropertyEditor/Public/DetailLayoutBuilder.h"
#include "PropertyEditor/Public/DetailWidgetRow.h"
#include "PropertyEditor/Public/PropertyHandle.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SWidget.h"

#define LOCTEXT_NAMESPACE "DialogSystem"

TSharedRef<IPropertyTypeCustomization> FNCurrentOrAllCustomization::MakeInstance()
{
	return MakeShareable(new FNCurrentOrAllCustomization());
}

void FNCurrentOrAllCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle,
	class FDetailWidgetRow& HeaderRow,
	IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	OnValuesChanged(StructPropertyHandle);

	// @formatter:off
	HeaderRow.NameContent()[StructPropertyHandle->CreatePropertyNameWidget()]
		.ValueContent()[
			SNew(STextBlock)
				.Font(FEditorStyle::GetFontStyle("PropertyWindow.NormalFont"))
				.Text(
					MakeAttributeLambda([=] { return FText::Format(LOCTEXT("ValueType", "{0}"), FText::FromString(OutText));})
				)
		];
	// @formatter:on
}

void FNCurrentOrAllCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle,
	class IDetailChildrenBuilder& StructBuilder,
	IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	TSharedPtr<IPropertyHandle> AllPropertyHandle = StructPropertyHandle->GetChildHandle(
		GET_MEMBER_NAME_CHECKED(FNCurrentOrAll, bIsAll)
	);
	TSharedPtr<IPropertyHandle> CurrentPropertyHandle = StructPropertyHandle->GetChildHandle(
		GET_MEMBER_NAME_CHECKED(FNCurrentOrAll, bIsCurrent)
	);

	AllPropertyHandle->SetOnPropertyValueChanged(
		FSimpleDelegate::CreateSP(this, &FNCurrentOrAllCustomization::OnValuesChanged, StructPropertyHandle)
	);
	CurrentPropertyHandle->SetOnPropertyValueChanged(
		FSimpleDelegate::CreateSP(this, &FNCurrentOrAllCustomization::OnValuesChanged, StructPropertyHandle)
	);

	StructBuilder.AddProperty(AllPropertyHandle.ToSharedRef());
	StructBuilder.AddProperty(CurrentPropertyHandle.ToSharedRef());
}


void FNCurrentOrAllCustomization::OnValuesChanged(TSharedRef<IPropertyHandle> StructPropertyHandle)
{
	if (StructPropertyHandle->IsValidHandle())
	{
		const TSharedPtr<IPropertyHandle> AllProperty = StructPropertyHandle->GetChildHandle(
			GET_MEMBER_NAME_CHECKED(FNCurrentOrAll, bIsAll)
		);
		const TSharedPtr<IPropertyHandle> CurrentProperty = StructPropertyHandle->GetChildHandle(
			GET_MEMBER_NAME_CHECKED(FNCurrentOrAll, bIsCurrent)
		);

		check(AllProperty.IsValid());

		FString Val1, Val2;
		AllProperty->GetValueAsDisplayString(Val1);
		CurrentProperty->GetValueAsDisplayString(Val2);

		OutText = TEXT("None");
		if (Val1.Equals(TEXT("True")))
		{
			OutText = TEXT("All");
			return;
		}
		if (Val2.Equals(TEXT("True")))
		{
			OutText = TEXT("Current");
		}
	}
}

#undef LOCTEXT_NAMESPACE
