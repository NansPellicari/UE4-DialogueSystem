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

#pragma once

#include "CoreMinimal.h"
#include "NansUMGExtent/Public/Blueprint/NansUserWidget.h"

#include "DialogHUD.generated.h"

class UPanelWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDialogHUDEvent);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDialogHUDMessageEvent, FText, Message, FText, Title);

UENUM(BlueprintType)
enum class EDialogMessageType : uint8
{
	Question,
	Response,
};

/**
 *
 */
UCLASS()
class NANSDIALOGSYSTEM_API UDialogHUD : public UNansUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Event")
	FDialogHUDMessageEvent OnResponse;

	UPROPERTY(BlueprintAssignable, Category = "Event")
	FDialogHUDMessageEvent OnQuestion;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Event")
	FDialogHUDEvent OnEndDisplayResponse;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Event")
	FDialogHUDEvent OnEndDisplayQuestion;

protected:
	UPROPERTY(EditAnywhere, Category = "Event")
	float TimeToDisplay = 2.f;
	// This to display response for a duration, and send OnEndDisplayResponse when finished
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeOnInitialized() override;
	virtual void Reset_Implementation() override;
	UFUNCTION(BlueprintCallable, Category = "Setup")
	void InitiliazedUI(UPanelWidget* _UIResponseBox, UPanelWidget* _UIQuestionBox);
	UFUNCTION(BlueprintCallable, Category = "Setup")
	void DisplayMessage(EDialogMessageType MessageType);

private:
	float TimeDisplayed = 0.f;
	UPanelWidget* UIResponseBox;
	UPanelWidget* UIQuestionBox;
	UFUNCTION()
	void StartDisplayResponse(FText Response, FText Title);
};
