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

#include "Customization/FullValueCustomization.h"


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

TSharedRef<IPropertyTypeCustomization> FNFullValueCustomization::MakeInstance()
{
	return MakeShareable(new FNFullValueCustomization());
}

void FNFullValueCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle,
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

void FNFullValueCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle,
	class IDetailChildrenBuilder& StructBuilder,
	IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	TSharedPtr<IPropertyHandle> AllPropertyHandle = StructPropertyHandle->GetChildHandle(
		GET_MEMBER_NAME_CHECKED(FNFullValue, bIsAll)
	);
	TSharedPtr<IPropertyHandle> CurrentPropertyHandle = StructPropertyHandle->GetChildHandle(
		GET_MEMBER_NAME_CHECKED(FNFullValue, bIsCurrent)
	);
	TSharedPtr<IPropertyHandle> ValuePropertyHandle = StructPropertyHandle->GetChildHandle(
		GET_MEMBER_NAME_CHECKED(FNFullValue, Value)
	);

	AllPropertyHandle->SetOnPropertyValueChanged(
		FSimpleDelegate::CreateSP(this, &FNFullValueCustomization::OnValuesChanged, StructPropertyHandle)
	);
	CurrentPropertyHandle->SetOnPropertyValueChanged(
		FSimpleDelegate::CreateSP(this, &FNFullValueCustomization::OnValuesChanged, StructPropertyHandle)
	);
	ValuePropertyHandle->SetOnPropertyValueChanged(
		FSimpleDelegate::CreateSP(this, &FNFullValueCustomization::OnValuesChanged, StructPropertyHandle)
	);

	StructBuilder.AddProperty(AllPropertyHandle.ToSharedRef());
	StructBuilder.AddProperty(CurrentPropertyHandle.ToSharedRef());
	StructBuilder.AddProperty(ValuePropertyHandle.ToSharedRef());
}


void FNFullValueCustomization::OnValuesChanged(TSharedRef<IPropertyHandle> StructPropertyHandle)
{
	if (StructPropertyHandle->IsValidHandle())
	{
		const TSharedPtr<IPropertyHandle> AllProperty = StructPropertyHandle->GetChildHandle(
			GET_MEMBER_NAME_CHECKED(FNFullValue, bIsAll)
		);
		const TSharedPtr<IPropertyHandle> CurrentProperty = StructPropertyHandle->GetChildHandle(
			GET_MEMBER_NAME_CHECKED(FNFullValue, bIsCurrent)
		);
		const TSharedPtr<IPropertyHandle> ValuePropertyHandle = StructPropertyHandle->GetChildHandle(
			GET_MEMBER_NAME_CHECKED(FNFullValue, Value)
		);

		check(AllProperty.IsValid());

		FString Val1, Val2, Val3;
		AllProperty->GetValueAsDisplayString(Val1);
		CurrentProperty->GetValueAsDisplayString(Val2);
		ValuePropertyHandle->GetValueAsDisplayString(Val3);

		OutText = TEXT("None");
		if (Val1.Equals(TEXT("True")))
		{
			OutText = TEXT("All");
			return;
		}
		if (Val2.Equals(TEXT("True")))
		{
			OutText = TEXT("Current");
			return;
		}

		if (!Val3.IsEmpty())
		{
			OutText = Val3;
		}
	}
}

#undef LOCTEXT_NAMESPACE
