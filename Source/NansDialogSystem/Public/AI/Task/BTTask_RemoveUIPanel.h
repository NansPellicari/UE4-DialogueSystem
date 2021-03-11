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
#include "AI/BehaviorTree/BTTask_NotifyAIOnAbort.h"
#include "BehaviorTree/BTTaskNode.h"

#include "BTTask_RemoveUIPanel.generated.h"

/**
 * Remove the UI set by a previous BTTask_CreateUIPanel node.
 * Don't forget to parameter DialogSystem in project settings.
 */
UCLASS()
class NANSDIALOGSYSTEM_API UBTTask_RemoveUIPanel : public UBTTask_NotifyAIOnAbort
{
	GENERATED_BODY()
protected:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;

#if WITH_EDITOR
	virtual FName GetNodeIconName() const override;
#endif	  // WITH_EDITOR
};
