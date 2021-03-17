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

#include "Pin/DialogueCategoryPin.h"

#include "ScopedTransaction.h"
#include "SNameComboBox.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraph/EdGraphSchema.h"
#include "Setting/DialogueSystemSettings.h"

void SNDialogueCategoryPin::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj)
{
	SGraphPin::Construct(SGraphPin::FArguments(), InGraphPinObj);
}

TSharedRef<SWidget> SNDialogueCategoryPin::GetDefaultValueWidget()
{
	TArray<FNDialogueCategorySettings> Settings;
	UDialogueSystemSettings::Get()->GetDialogueCategoryConfigs(Settings);
	for (const auto& Setting : Settings)
	{
		CategoryList.Add(MakeShareable(new FName(Setting.Name.ToString())));
	}

	Settings.Empty();

	TSharedPtr<FName> InitialSelectedName = GetSelectedName();

	// if no data selected, save the first one
	// Note: Null val is possible if no configured timelines have been created.
	if (!InitialSelectedName.IsValid() && CategoryList.Num() > 0 && CategoryList[0].IsValid())
	{
		InitialSelectedName = CategoryList[0];
		SetPropertyWithName(*InitialSelectedName);
		InitialSelectedName = GetSelectedName();
	}

	return SAssignNew(NameComboBox, SNameComboBox) // note you can display any widget here
		.ContentPadding(FMargin(6.0f, 2.0f))
		.OptionsSource(&CategoryList)
		.InitiallySelectedItem(InitialSelectedName)
		.Visibility(this, &SGraphPin::GetDefaultValueVisibility)
		.OnComboBoxOpening(this, &SNDialogueCategoryPin::OnComboBoxOpening)
		.OnSelectionChanged(this, &SNDialogueCategoryPin::OnAttributeSelected);
}

void SNDialogueCategoryPin::OnAttributeSelected(TSharedPtr<FName> ItemSelected, ESelectInfo::Type SelectInfo)
{
	if (ItemSelected.IsValid())
	{
		SetPropertyWithName(*ItemSelected);
	}
}

void SNDialogueCategoryPin::OnComboBoxOpening()
{
	TSharedPtr<FName> SelectedName = GetSelectedName();
	if (SelectedName.IsValid())
	{
		check(NameComboBox.IsValid());
		NameComboBox->SetSelectedItem(SelectedName);
	}
}

void SNDialogueCategoryPin::SetPropertyWithName(const FName& Name)
{
	check(GraphPinObj);
	check(GraphPinObj->PinType.PinSubCategoryObject == FNDialogueCategory::StaticStruct());

	if (GraphPinObj->HasAnyConnections()) return;
	// To set the property we need to use a FString
	// using this format: (MyPropertyName="My Value")
	FString PinString = TEXT("(Name=(TagName=\"");
	PinString += Name.ToString();
	PinString += TEXT("\"))");

	FString CurrentDefaultValue = GraphPinObj->GetDefaultAsString();

	if (CurrentDefaultValue != PinString)
	{
		GraphPinObj->GetSchema()->TrySetDefaultValue(*GraphPinObj, PinString);
	}
}

TSharedPtr<FName> SNDialogueCategoryPin::GetSelectedName() const
{
	int32 NameCount = CategoryList.Num();
	if (NameCount <= 0)
	{
		return nullptr;
	}

	FName Name;
	GetPropertyAsName(Name);

	for (int32 NameIndex = 0; NameIndex < NameCount; ++NameIndex)
	{
		if (Name == *CategoryList[NameIndex].Get())
		{
			return CategoryList[NameIndex];
		}
	}

	return nullptr;
}

void SNDialogueCategoryPin::GetPropertyAsName(FName& OutName) const
{
	check(GraphPinObj);
	check(GraphPinObj->PinType.PinSubCategoryObject == FNDialogueCategory::StaticStruct());

	// As we saw in the SNDialogueCategoryPin::SetPropertyWithName()
	// The value is saved in the format (MyPropertyName="My Value") as a FString.
	// So we have to retrieve the real value and convert it to a FName
	FString PinString = GraphPinObj->GetDefaultAsString();

	if (PinString.StartsWith(TEXT("(")) && PinString.EndsWith(TEXT(")")))
	{
		PinString = PinString.LeftChop(1); // to remove ")"
		PinString = PinString.RightChop(6); // to remove "(Name="
		if (PinString.StartsWith(TEXT("(")) && PinString.EndsWith(TEXT(")")))
		{
			PinString = PinString.LeftChop(1);
			PinString = PinString.RightChop(1);
			PinString.Split(FString("="), nullptr, &PinString);

			if (PinString.StartsWith(TEXT("\"")) && PinString.EndsWith(TEXT("\"")))
			{
				PinString = PinString.LeftChop(1);
				PinString = PinString.RightChop(1);
			}
		}
	}

	if (!PinString.IsEmpty())
	{
		OutName = *PinString;
	}
}
