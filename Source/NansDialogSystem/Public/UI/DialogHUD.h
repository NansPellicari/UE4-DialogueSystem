// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NansUMGExtent/Public/Blueprint/NansUserWidget.h"

#include "DialogHUD.generated.h"

class UPanelWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDialogHUDEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDialogHUDMessageEvent, FText, Message);

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
	void StartDisplayResponse(FText Response);
};
