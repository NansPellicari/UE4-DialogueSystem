// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "CoreMinimal.h"
#include "INansDialogSystem.h"
#include "MessageLog/Public/MessageLogInitializationOptions.h"
#include "MessageLog/Public/MessageLogModule.h"
#include "Modules/ModuleManager.h"
#include "NansDialogSystemLog.h"

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
}

void FNansDialogSystem::ShutdownModule()
{
	if (FModuleManager::Get().IsModuleLoaded("MessageLog"))
	{
		// unregister message log
		FMessageLogModule& MessageLogModule = FModuleManager::GetModuleChecked<FMessageLogModule>("MessageLog");
		MessageLogModule.UnregisterLogListing("DialogSystem");
	}
}
