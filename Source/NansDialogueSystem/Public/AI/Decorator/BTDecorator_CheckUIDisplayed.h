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
#include "BehaviorTree/BTDecorator.h"

#include "BTDecorator_CheckUIDisplayed.generated.h"

/**
 * 
 */
UCLASS()
class NANSDIALOGUESYSTEM_API UBTDecorator_CheckUIDisplayed : public UBTDecorator
{
	GENERATED_BODY()
public:
	UBTDecorator_CheckUIDisplayed(const FObjectInitializer& ObjectInitializer);

protected:
	UPROPERTY(EditInstanceOnly, Category = "HUD")
	FName UIName = NAME_None;

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
