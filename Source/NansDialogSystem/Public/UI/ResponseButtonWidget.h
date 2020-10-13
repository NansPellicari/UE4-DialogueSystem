// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BTDialogueTypes.h"
#include "CoreMinimal.h"
#include "NansExtension/Public/UMG/Blueprint/NansUserWidget.h"

#include "ResponseButtonWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FResponseBTEvent, UResponseButtonWidget*, Button);

class UBTDialogueResponseContainer;

/**
 *
 */
UCLASS(Abstract, Blueprintable)
class NANSDIALOGSYSTEM_API UResponseButtonWidget : public UNansUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Event")
	FResponseBTEvent OnBTClicked;

	UFUNCTION(BlueprintCallable, Category = "Event")
	UBTDialogueResponseContainer* GetResponse();

	UFUNCTION(BlueprintCallable, Category = "Event")
	void SetResponse(UBTDialogueResponseContainer* DialogueResponse);

	void ComputeColor(int32 MaxLevel);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Color")
	FLinearColor ColorCNV;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Color")
	FLinearColor ColorNeutral;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Color")
	FLinearColor ColorCSV;

	UPROPERTY(BlueprintReadOnly, Category = "Color")
	FLinearColor FinalColor;

	UPROPERTY(BlueprintReadWrite, Category = "Widget")
	UBTDialogueResponseContainer* Response;

	UFUNCTION(BlueprintCallable, Category = "Event")
	void CallBTClicked(UResponseButtonWidget* Button);
};
