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
#include "BTDialogueTypes.h"
#include "Components/Button.h"
#include "NansUMGExtent/Public/Blueprint/NansUserWidget.h"

#include "ResponseButtonWidget.generated.h"

class UResponseButtonWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FResponseBTEvent, UResponseButtonWidget*, Button);

class UBTDialogueResponseContainer;

struct FResponseButtonBuilderData
{
	FResponseButtonBuilderData()
	{
		Response.Text = NSLOCTEXT("DialogSystem", "DefaultDialogueResponseText", "Euh...");
	}

	FBTDialogueResponse Response;
	EResponseDirection InfluencedBy = EResponseDirection::NONE;
	int32 DisplayOrder = 0;
	int32 MaxPoints = 0;
};

/**
 *
 */
UCLASS(Abstract, Blueprintable, hidecategories = ("Appearance"))
class NANSDIALOGSYSTEM_API UResponseButtonWidget : public UNansUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Event")
	FResponseBTEvent OnBTClicked;

	UPROPERTY(BlueprintReadOnly, Category = "Color")
	EResponseDirection InfluencedBy;
	UPROPERTY(BlueprintReadOnly, Category = "Color")
	int32 DisplayOrder;

	UFUNCTION(BlueprintCallable, Category = "Event")
	FBTDialogueResponse GetResponse();
	UFUNCTION(BlueprintCallable, Category = "Event")
	void SetText(FString InText);
	UFUNCTION(BlueprintCallable, Category = "Event")
	FString GetText() const;


	UFUNCTION(BlueprintCallable, Category = "Event")
	void SetResponse(FBTDialogueResponse DialogueResponse);

	void ComputeColor(int32 MaxLevel);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Color")
	FLinearColor BaseColor;

	UPROPERTY(BlueprintReadOnly, Category = "Color")
	FLinearColor FinalColor;

	UPROPERTY(BlueprintReadOnly, Category = "Response", meta=(BindWidget))
	UButton* TheButton = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "Widget")
	FBTDialogueResponse Response;

	UPROPERTY(BlueprintReadOnly, Category = "Widget")
	FString Text;

	UFUNCTION(BlueprintCallable, Category = "Event")
	void CallBTClicked();
};
