// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NansExtension/Public/UMG/Blueprint/NansUserWidget.h"
#include "PointSystemHelpers.h"

#include "ButtonSequenceWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FButtonSequenceEvent, UButtonSequenceWidget*, Button);

/**
 *
 */
UCLASS(hidecategories = ("Appearance"))
class NANSDIALOGSYSTEM_API UButtonSequenceWidget : public UNansUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Event")
	FButtonSequenceEvent OnBTClicked;

	void SetText(FString _Text);
	FString GetText() const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Display")
	FVector2D ButtonDisplaySize = FVector2D(100.f, 100.f);

	UPROPERTY(BlueprintReadOnly, Category = "Display")
	FLinearColor FinalColor;

	UPROPERTY(BlueprintReadOnly, Category = "Display")
	EResponseDirection Direction;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Widget")
	FString Text;

	UFUNCTION(BlueprintCallable, Category = "Event")
	void CallBTClicked();
};
