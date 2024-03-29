﻿// Copyright 2020-present Nans Pellicari (nans.pellicari@gmail.com).
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

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DialogueHUD.generated.h"

UINTERFACE()
class UDialogueHUD : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class NANSDIALOGUESYSTEM_API IDialogueHUD
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="UI")
	void HideUIPanel(FName Name);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="UI")
	bool IsDisplayed(const FName Name) const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="UI")
	UUserWidget* DisplayUIPanel(TSubclassOf<UUserWidget> WidgetClass, FName Name);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="UI")
	UUserWidget* GetCurrentUIPanel() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="UI")
	UUserWidget* GetFullCurrentUIPanel(TSubclassOf<UUserWidget>& OutWidgetClass, FName& OutName) const;
};
