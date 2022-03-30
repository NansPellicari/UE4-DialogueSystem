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
#include "Service/DialogueDifficultyHandler.h"
#include "Service/DialoguePointsHandler.h"

#include "NDialogueSubsystem.generated.h"

class UAbilitySystemComponent;
class NDialoguePointsHandler;
class NDialogueDifficultyHandler;

/**
 * 
 */
UCLASS()
class NANSDIALOGUESYSTEM_API UNDialogueSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()
public:
	// Begin USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// End USubsystem

	FORCEINLINE void SetDebugPointsHandler(bool bInDebugPointsHandler)
	{
		bDebugPointsHandler = bInDebugPointsHandler;
		if (PointsHandler.IsValid())
		{
			PointsHandler->bDebug = bDebugPointsHandler;
		}
	}

	FORCEINLINE void SetDebugDifficultyHandler(bool bInDebugDifficultyHandler)
	{
		bDebugDifficultyHandler = bInDebugDifficultyHandler;
		if (DifficultyHandler.IsValid())
		{
			DifficultyHandler->bDebug = bDebugDifficultyHandler;
		}
	}

	bool CreateDialogSequence(const AAIController* Owner);
	void EndDialogSequence(const AAIController* Owner);

	bool PlayerIsInDialogSequenceWith(const AAIController* Owner) const;
	const TSharedPtr<NDialoguePointsHandler>& GetPointsHandler(const AAIController* Owner) const;
	const TSharedPtr<NDialogueDifficultyHandler>& GetDifficultyHandler(const AAIController* Owner) const;
	UAbilitySystemComponent* GetPlayerAbs() const;
	ACharacter* GetPlayerCharacter() const;
private:
	UPROPERTY(EditAnywhere)
	bool bDebugPointsHandler = false;
	UPROPERTY(EditAnywhere)
	bool bDebugDifficultyHandler = false;
	TSharedPtr<NDialoguePointsHandler> PointsHandler;
	TSharedPtr<NDialogueDifficultyHandler> DifficultyHandler;
};
