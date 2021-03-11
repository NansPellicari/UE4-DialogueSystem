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

#include "BTTask_NoteToPlayer.generated.h"

class AMessageableHUD;
class INMessageable;

UENUM(BlueprintType)
enum class EMessageDisplayOn : uint8
{
	PlayerHUD UMETA(DisplayName = "Player HUD"),
	NPC UMETA(DisplayName = "NPC's balloon"),
};

/**
 *
 */
UCLASS()
class NANSDIALOGSYSTEM_API UBTTask_NoteToPlayer : public UBTTask_NotifyAIOnAbort
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditInstanceOnly, Category = "Message")
	float MessageDuration = 5.f;

	UPROPERTY(EditInstanceOnly, Category = "Message", meta = (MultiLine = true))
	FText Message;

	UPROPERTY(EditInstanceOnly, Category = "Message")
	EMessageDisplayOn WhereToDisplay = EMessageDisplayOn::NPC;

	UPROPERTY(EditInstanceOnly, Category = "Message")
	bool WaitAfterMessage = false;

	virtual FString GetStaticDescription() const override;
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
		EBTNodeResult::Type TaskResult) override;

#if WITH_EDITOR
	virtual FName GetNodeIconName() const override;
#endif // WITH_EDITOR

private:
	UPROPERTY()
	TArray<TScriptInterface<INMessageable>> MessageWidgets;
	UPROPERTY()
	UBehaviorTreeComponent* OwnerComponent = nullptr;
	UFUNCTION()
	void OnMessageEnd();

	int32 MessageFinished = 0;
};
