// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BTDialogueTypes.h"
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "BTDialogueResponseContainer.generated.h"

/**
 * This a bit a shame, because Blackboard doesn't support struct in variable, need to create this object container.
 */
UCLASS(BlueprintType)
class NANSDIALOGSYSTEM_API UBTDialogueResponseContainer : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Response")
	FBTDialogueResponse GetResponse();

	void SetResponse(FBTDialogueResponse _Response);

	static UBTDialogueResponseContainer* CreateNullObject(UObject* Outer);

private:
	FBTDialogueResponse Response;
};
