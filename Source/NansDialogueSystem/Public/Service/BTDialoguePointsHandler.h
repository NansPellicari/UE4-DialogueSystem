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
#include "BTStepsHandlerContainer.h"
#include "PointSystemHelpers.h"
#include "AI/Decorator/BTDecorator_CheckInStep.h"
#include "Component/PlayerDialogueComponent.h"
#include "Dialogue/DialogueHistorySearch.h"
#include "BTDialoguePointsHandler.generated.h"

struct FDialogueSequence;
struct FNDialogueFactorSettings;
class UNFactorsFactoryClientAdapter;
class UBehaviorTreeComponent;

UCLASS(BlueprintType)
class NANSDIALOGUESYSTEM_API UBTDialoguePointsHandler : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	bool bDebug = false;


	UBTDialoguePointsHandler() {}
	bool Initialize(UBTStepsHandlerContainer* InStepsHandler, UBehaviorTreeComponent& OwnerComp,
		FDialogueSequence DialogueSequence);

	virtual void BeginDestroy() override;

	UFUNCTION(BlueprintCallable, Category = "PointsHandler")
	void AddPoints(FNPoint Point, int32 Position);

	UFUNCTION(BlueprintCallable, Category = "PointsHandler")
	void Clear();
	bool HasResults(const FNDialogueHistorySearch& Search) const;
	bool HasResults(const TArray<FNDialogueHistorySearch> Searches,
		TArray<FNansConditionOperator> ConditionsOperators) const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "PointsHandler")
	int32 GetDialoguePoints(FNDialogueCategory Category) const;

protected:
	UPROPERTY()
	UBTStepsHandlerContainer* StepsHandler;
	UPROPERTY()
	UAbilitySystemComponent* PlayerGASC;
	UPROPERTY()
	UPlayerDialogueComponent* DialogComp;

	FString BehaviorTreePathName;
	FString AIPawnPathName;
};
