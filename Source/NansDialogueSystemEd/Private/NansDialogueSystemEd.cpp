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

#include "CoreMinimal.h"
#include "INansDialogueSystemEd.h"
#include "Customization/AllOrNameCustomization.h"
#include "Customization/CurrentOrAllCustomization.h"
#include "Customization/DialogueCategoryCustomization.h"
#include "Customization/DialogueFactorSettingsCustomization.h"
#include "Customization/FullValueCustomization.h"
#include "Modules/ModuleManager.h"
#include "Pin/DialogueSettingsPinFactory.h"
#include "PropertyEditor/Public/PropertyEditorModule.h"

class FNansDialogueSystemEd : public INansDialogueSystemEd
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	TSharedPtr<FNDialogueSettingsPinFactory> DialogSettingsPinFactory;
};

IMPLEMENT_MODULE(FNansDialogueSystemEd, NansDialogueSystemEd)

void FNansDialogueSystemEd::StartupModule()
{
	// create a pin factory and shared pointer to it.
	DialogSettingsPinFactory = MakeShareable(new FNDialogueSettingsPinFactory());
	// and now register it.
	FEdGraphUtilities::RegisterVisualPinFactory(DialogSettingsPinFactory);

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	// Custom properties
	PropertyModule.RegisterCustomPropertyTypeLayout(
		"NDialogueCategory",
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FNDialogueCategoryCustomization::MakeInstance)
	);
	PropertyModule.RegisterCustomPropertyTypeLayout(
		"NDialogueFactorSettings",
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FNDialogueFactorSettingsCustomization::MakeInstance)
	);
	PropertyModule.RegisterCustomPropertyTypeLayout(
		"NCurrentOrAll",
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FNCurrentOrAllCustomization::MakeInstance)
	);
	PropertyModule.RegisterCustomPropertyTypeLayout(
		"NFullValue",
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FNFullValueCustomization::MakeInstance)
	);
	PropertyModule.RegisterCustomPropertyTypeLayout(
		"NAllOrName",
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FNAllOrNameCustomization::MakeInstance)
	);

	PropertyModule.NotifyCustomizationModuleChanged();
}

void FNansDialogueSystemEd::ShutdownModule()
{
	FEdGraphUtilities::UnregisterVisualPinFactory(DialogSettingsPinFactory);
	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		// unregister properties
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(
			"PropertyEditor"
		);
		PropertyModule.UnregisterCustomPropertyTypeLayout("NDialogueCategory");
		PropertyModule.UnregisterCustomPropertyTypeLayout("NDialogueFactorSettings");
		PropertyModule.UnregisterCustomPropertyTypeLayout("NCurrentOrAll");
		PropertyModule.UnregisterCustomPropertyTypeLayout("NFullValue");
		PropertyModule.UnregisterCustomPropertyTypeLayout("NAllOrName");

		PropertyModule.NotifyCustomizationModuleChanged();
	}
}
