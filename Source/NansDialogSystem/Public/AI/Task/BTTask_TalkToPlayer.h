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
#include "BehaviorTree/BTTaskNode.h"

#include "BTTask_TalkToPlayer.generated.h"

class UDialogHUD;
class UButton;

/**
 *
 */
UCLASS()
class NANSDIALOGSYSTEM_API UBTTask_TalkToPlayer : public UBTTaskNode
{
	GENERATED_BODY()

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
	UBTTask_TalkToPlayer(const FObjectInitializer& ObjectInitializer);
	UFUNCTION()
	void OnQuestionEnd();

	virtual FString GetStaticDescription() const override;

#if WITH_EDITOR
	virtual FName GetNodeIconName() const override;
#endif	  // WITH_EDITOR

protected:
	UPROPERTY(VisibleAnywhere, Category = "HUD")
	FName UINameKey = NAME_None;

	UPROPERTY(EditInstanceOnly, Category = "Message", meta = (MultiLine = true))
	FText Message;

	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
		EBTNodeResult::Type TaskResult) override;

private:
	UPROPERTY()
	UBehaviorTreeComponent* OwnerComponent = nullptr;
	UPROPERTY()
	UDialogHUD* HUD = nullptr;
};
