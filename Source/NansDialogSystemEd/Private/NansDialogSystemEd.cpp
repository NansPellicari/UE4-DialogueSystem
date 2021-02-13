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

#include "CoreMinimal.h"
#include "INansDialogSystemEd.h"
#include "Customization/AllOrNameCustomization.h"
#include "Customization/CurrentOrAllCustomization.h"
#include "Customization/DialogFactorSettingsCustomization.h"
#include "Customization/DialogFactorTypeSettingsCustomization.h"
#include "Customization/FullValueCustomization.h"
#include "Customization/ResponseCategoryCustomization.h"
#include "Modules/ModuleManager.h"
#include "Pin/DialogSettingsPinFactory.h"
#include "PropertyEditor/Public/PropertyEditorModule.h"

class FNansDialogSystemEd : public INansDialogSystemEd
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	TSharedPtr<FNDialogSettingsPinFactory> DialogSettingsPinFactory;
};

IMPLEMENT_MODULE(FNansDialogSystemEd, NansDialogSystemEd)

void FNansDialogSystemEd::StartupModule()
{
	// create a pin factory and shared pointer to it.
	DialogSettingsPinFactory = MakeShareable(new FNDialogSettingsPinFactory());
	// and now register it.
	FEdGraphUtilities::RegisterVisualPinFactory(DialogSettingsPinFactory);

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	// Custom properties
	PropertyModule.RegisterCustomPropertyTypeLayout(
		"NResponseCategory",
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FNResponseCategoryCustomization::MakeInstance)
	);
	PropertyModule.RegisterCustomPropertyTypeLayout(
		"NDialogFactorTypeSettings",
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FNDialogFactorTypeSettingsCustomization::MakeInstance)
	);
	PropertyModule.RegisterCustomPropertyTypeLayout(
		"NDialogFactorSettings",
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FNDialogFactorSettingsCustomization::MakeInstance)
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

void FNansDialogSystemEd::ShutdownModule()
{
	FEdGraphUtilities::UnregisterVisualPinFactory(DialogSettingsPinFactory);
	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		// unregister properties
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(
			"PropertyEditor"
		);
		PropertyModule.UnregisterCustomPropertyTypeLayout("NResponseCategory");
		PropertyModule.UnregisterCustomPropertyTypeLayout("NDialogFactorTypeSettings");
		PropertyModule.UnregisterCustomPropertyTypeLayout("NDialogFactorSettings");
		PropertyModule.UnregisterCustomPropertyTypeLayout("NCurrentOrAll");
		PropertyModule.UnregisterCustomPropertyTypeLayout("NFullValue");
		PropertyModule.UnregisterCustomPropertyTypeLayout("NAllOrName");

		PropertyModule.NotifyCustomizationModuleChanged();
	}
}
