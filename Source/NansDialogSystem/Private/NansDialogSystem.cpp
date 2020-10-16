// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "CoreMinimal.h"
#include "Editor/Pin/DialogSettingsPinFactory.h"
#include "INansDialogSystem.h"
#include "MessageLog/Public/MessageLogInitializationOptions.h"
#include "MessageLog/Public/MessageLogModule.h"
#include "Modules/ModuleManager.h"
#include "NansDialogSystemLog.h"
#include "PropertyEditor/Public/PropertyEditorModule.h"

class FNansDialogSystem : public INansDialogSystem
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

IMPLEMENT_MODULE(FNansDialogSystem, NansDialogSystem)

void FNansDialogSystem::StartupModule()
{
	// create a message log to use in my module
	FMessageLogModule& MessageLogModule = FModuleManager::LoadModuleChecked<FMessageLogModule>("MessageLog");
	FMessageLogInitializationOptions InitOptions;
	InitOptions.bAllowClear = true;
	InitOptions.bShowFilters = true;
	MessageLogModule.RegisterLogListing(
		"DialogSystem", NSLOCTEXT("DialogSystem", "DialogSystemLogLabel", "Dialog System"), InitOptions);

	// create a pin factory and shared pointer to it.
	TSharedPtr<FNDialogSettingsPinFactory> Factory = MakeShareable(new FNDialogSettingsPinFactory());
	// and now register it.
	FEdGraphUtilities::RegisterVisualPinFactory(Factory);

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	// Custom properties
	PropertyModule.RegisterCustomPropertyTypeLayout(
		"NResponseCategory", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FNResponseCategoryCustomization::MakeInstance));
}

void FNansDialogSystem::ShutdownModule()
{
	if (FModuleManager::Get().IsModuleLoaded("MessageLog"))
	{
		// unregister message log
		FMessageLogModule& MessageLogModule = FModuleManager::GetModuleChecked<FMessageLogModule>("MessageLog");
		MessageLogModule.UnregisterLogListing("DialogSystem");
	}

	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		// unregister properties
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.UnregisterCustomPropertyTypeLayout("NResponseCategory");
	}
}
