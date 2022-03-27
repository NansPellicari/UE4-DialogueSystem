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

#include "WheelButtonWidget.h"
#include "Components/Button.h"
#include "Blueprint/NansUserWidget.h"

#include "DialogueUI.generated.h"

class UDialogueProgressBarWidget;
struct FResponseButtonBuilderData;
class UResponseButtonWidget;
class UPanelWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDialogueUIEvent);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDialogueUIMessageEvent, FText, Message, FText, Title);

UENUM(BlueprintType)
enum class EDialogMessageType : uint8
{
	Question,
	Response,
};

struct NTextData
{
	FText Text;
	FText Title;
};

/**
 *
 */
UCLASS()
class NANSDIALOGUESYSTEM_API UDialogueUI : public UNansUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Event")
	FDialogueUIMessageEvent OnResponse;

	UPROPERTY(BlueprintAssignable, Category = "Event")
	FDialogueUIMessageEvent OnQuestion;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Event")
	FDialogueUIEvent OnEndDisplayResponse;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Event")
	FDialogueUIEvent OnEndDisplayQuestion;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Response")
	int32 CountVisibleResponses() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Response")
	int32 CountResponses() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Response")
	TArray<UResponseButtonWidget*> GetResponsesButttons() const;

	UFUNCTION(BlueprintNativeEvent, Category="Response")
	void SetNPCText(const FText& InText, const FText& InTitle);
	virtual void SetNPCText_Implementation(const FText& InText, const FText& InTitle);

	UFUNCTION(BlueprintNativeEvent, Category="Response")
	void SetPlayerText(const FText& InText, const FText& InTitle);
	virtual void SetPlayerText_Implementation(const FText& InText, const FText& InTitle);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Response")
	void GetCurrentPlayerDialog(FText& OutText, FText& OutTitle);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Response")
	void GetCurrentNPCDialog(FText& OutText, FText& OutTitle);

	UFUNCTION(BlueprintImplementableEvent, Category="Response")
	void OnRemoveResponse(UResponseButtonWidget* Widget);

	UFUNCTION(BlueprintImplementableEvent, Category="Response")
	void OnAddResponse(UResponseButtonWidget* Widget);

	virtual UResponseButtonWidget* AddNewResponse(FResponseButtonBuilderData& BuilderData);
	virtual bool RemoveResponses();

	UPanelWidget* GetUINPCBox() const;
	UPanelWidget* GetResponsesSlot() const;
	UPanelWidget* GetUIPlayerBox() const;
	UWheelButtonWidget* GetWheelButton() const;
	UButton* GetButtonPassPlayerLine() const;
	UButton* GetButtonPassNPCLine() const;
	UDialogueProgressBarWidget* GetProgressBar() const;
	void ChangeButtonVisibility(ESlateVisibility Visibility, int32 Index) const;
	void ChangeButtonsVisibility(ESlateVisibility Visibility) const;

protected:
	UPROPERTY(EditAnywhere, Category = "Event")
	float TimeToDisplay = 2.f;
	// This to display response for a duration, and send OnEndDisplayResponse when finished
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativePreConstruct() override;
	virtual void NativeDestruct() override;
	virtual void Reset_Implementation() override;
	UFUNCTION(BlueprintCallable, Category = "Setup")
	void DisplayMessage(EDialogMessageType MessageType);
	UPROPERTY(BlueprintReadOnly, Category = "Response", meta=(BindWidget, OptionalWidget=true))
	UWheelButtonWidget* WheelButton = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "Response", meta=(BindWidget, OptionalWidget=true))
	UDialogueProgressBarWidget* ProgressBar = nullptr;
public:
protected:
	UPROPERTY(BlueprintReadWrite, Category = "Response", meta=(BindWidget))
	UPanelWidget* UIPlayerBox = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "Response", meta=(BindWidget))
	UPanelWidget* ResponsesSlot = nullptr;
	UPROPERTY(BlueprintReadWrite, Category = "Question", meta=(BindWidget))
	UPanelWidget* UINPCBox = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "Response", meta=(BindWidget))
	UButton* ButtonPassPlayerLine = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "Response", meta=(BindWidget))
	UButton* ButtonPassNPCLine = nullptr;
	UPROPERTY(EditAnywhere, Category = "Response")
	TSubclassOf<UResponseButtonWidget> ResponseButtonWidget;

private:
	float TimeDisplayed = 0.f;
	NTextData CurrentNPCDialogue;
	NTextData CurrentPlayerDialogue;
};
