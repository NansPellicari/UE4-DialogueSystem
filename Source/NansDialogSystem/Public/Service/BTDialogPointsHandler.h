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
#include "PointSystemHelpers.h"
#include "AI/Decorator/BTDecorator_CheckInStep.h"
#include "Component/PlayerDialogComponent.h"
#include "Dialogue/DialogueHistorySearch.h"
#include "BTDialogPointsHandler.generated.h"

struct FDialogueSequence;
struct FNDialogFactorSettings;
class UNFactorsFactoryClientAdapter;
class IBTStepsHandler;
class UBehaviorTreeComponent;

UCLASS(BlueprintType)
class NANSDIALOGSYSTEM_API UBTDialogPointsHandler : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	bool bDebug = false;


	UBTDialogPointsHandler() {}
	bool Initialize(TScriptInterface<IBTStepsHandler> InStepsHandler, UBehaviorTreeComponent& OwnerComp,
		FDialogueSequence DialogueSequence);

	virtual void BeginDestroy() override;

	UFUNCTION(BlueprintCallable, Category = "PointsHandler")
	void AddPoints(FNPoint Point, int32 Position);

	UFUNCTION(BlueprintCallable, Category = "PointsHandler")
	void Clear();
	bool HasResults(const FNDialogueHistorySearch& Search);
	bool HasResults(const TArray<FNDialogueHistorySearch> Searches, TArray<FNansConditionOperator> ConditionsOperators);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "PointsHandler")
	int32 GetDialogPoints(FNDialogueCategory Category) const;

protected:
	UPROPERTY()
	TScriptInterface<IBTStepsHandler> StepsHandler;
	UPROPERTY()
	UAbilitySystemComponent* PlayerGASC;
	UPROPERTY()
	UPlayerDialogComponent* DialogComp;

	FString BehaviorTreePathName;
	FString AIPawnPathName;
};
