// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BTDialogueTypes.h"
#include "BTTask_Responses.h"
#include "CoreMinimal.h"

#include "BTTask_WheelResponses.generated.h"

class UNansUserWidget;
class UPanelWidget;
class UResponseButtonWidget;

/**
 *
 */
UCLASS()
class NANSDIALOGSYSTEM_API UBTTask_WheelResponses : public UBTTask_Responses
{
	GENERATED_BODY()
public:
	virtual FString GetStaticDescription() const override;

#if WITH_EDITOR
	virtual FName GetNodeIconName() const override;
#endif	  // WITH_EDITOR

protected:
	UPROPERTY(EditInstanceOnly, Category = "Responses")
	float ResponsesTypeTolerance;

private:
	float WheelRatioClockwise = 0.f;
	float WheelRatioAntiClockwise = 0.f;

	/**
	 * This check the wheel distances (clockwise and anti) to display appropriate buttons.
	 */
	virtual void ReceiveOnTick(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
