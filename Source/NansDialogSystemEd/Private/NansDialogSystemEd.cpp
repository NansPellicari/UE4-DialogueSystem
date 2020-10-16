// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "CoreMinimal.h"
#include "INansDialogSystemEd.h"
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
		"NResponseCategory", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FNResponseCategoryCustomization::MakeInstance));
}

void FNansDialogSystemEd::ShutdownModule()
{
	FEdGraphUtilities::UnregisterVisualPinFactory(DialogSettingsPinFactory);
	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		// unregister properties
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.UnregisterCustomPropertyTypeLayout("NResponseCategory");
	}
}
