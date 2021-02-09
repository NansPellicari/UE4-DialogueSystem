//  Copyright 2020-present Nans Pellicari (nans.pellicari@gmail.com).
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
#include "AI/BehaviorTree/Tasks/BTTask_Base.h"

#include "BTTask_UpdateDialogueStep.generated.h"

class UBTDialogueResponseContainer;

/**
 *
 */
UCLASS()
class NANSDIALOGSYSTEM_API UBTTask_UpdateDialogueStep : public UBTTask_Base
{
	GENERATED_BODY()

	UPROPERTY(EditInstanceOnly, Category = "Blackboard")
	FName ResponseContainerName = FName("ResponseContainer");

	UPROPERTY(EditInstanceOnly, Category = "Blackboard")
	FName PointsHandlerKeyName = FName("PointsHandler");

	UPROPERTY(EditInstanceOnly, Category = "Blackboard")
	FName StepsKeyName = FName("Steps");

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;

#if WITH_EDITOR
	virtual FName GetNodeIconName() const override;
#endif	  // WITH_EDITOR

private:
	UPROPERTY()
	UBTDialogueResponseContainer* ResponseContainer;
};
