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
#include "BTTask_Responses.h"

#include "BTTask_WheelResponses.generated.h"

class UNansUserWidget;
class UPanelWidget;
class UResponseButtonWidget;

/**
 *
 */
UCLASS()
class NANSDIALOGUESYSTEM_API UBTTask_WheelResponses : public UBTTask_Responses
{
	GENERATED_BODY()
public:

	explicit UBTTask_WheelResponses(const FObjectInitializer& ObjectInitializer);

	virtual FString GetStaticDescription() const override;

#if WITH_EDITOR
	virtual FName GetNodeIconName() const override;
#endif	  // WITH_EDITOR

protected:
	UPROPERTY(EditInstanceOnly, Category = "Blackboard")
	FName DifficultyHandlerKeyName = FName("DifficultyHandler");
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
