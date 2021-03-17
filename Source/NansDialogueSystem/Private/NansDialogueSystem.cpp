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
#include "INansDialogueSystem.h"
#include "NansDialogueSystemLog.h"
#include "MessageLog/Public/MessageLogInitializationOptions.h"
#include "MessageLog/Public/MessageLogModule.h"
#include "Modules/ModuleManager.h"

class FNansDialogueSystem : public INansDialogueSystem
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

IMPLEMENT_MODULE(FNansDialogueSystem, NansDialogueSystem)

void FNansDialogueSystem::StartupModule()
{
	// create a message log to use in my module
	FMessageLogModule& MessageLogModule = FModuleManager::LoadModuleChecked<FMessageLogModule>("MessageLog");
	FMessageLogInitializationOptions InitOptions;
	InitOptions.bAllowClear = true;
	InitOptions.bShowFilters = true;
	MessageLogModule.RegisterLogListing(
		"DialogueSystem",
		NSLOCTEXT("DialogueSystem", "DialogueSystemLogLabel", "Dialog System"),
		InitOptions
	);
}

void FNansDialogueSystem::ShutdownModule()
{
	if (FModuleManager::Get().IsModuleLoaded("MessageLog"))
	{
		// unregister message log
		FMessageLogModule& MessageLogModule = FModuleManager::GetModuleChecked<FMessageLogModule>("MessageLog");
		MessageLogModule.UnregisterLogListing("DialogueSystem");
	}
}
