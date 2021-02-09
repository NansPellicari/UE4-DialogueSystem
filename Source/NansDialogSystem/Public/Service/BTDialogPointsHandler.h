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
#include "BTDialogueTypes.h"
#include "PointSystemHelpers.h"
#include "Factor/DialogFactorUnit.h"
#include "BTDialogPointsHandler.generated.h"

struct FNDialogFactorSettings;
class UNFactorsFactoryClientAdapter;
class IBTStepsHandler;

/**
 * Points Associated to step
 * TODO remove this and replace by factor
 */
USTRUCT(BlueprintType)
struct NANSDIALOGSYSTEM_API FBTPointInStep
{
	GENERATED_USTRUCT_BODY()

	FBTPointInStep(int32 _Step = 0, FNPoint _Point = FNPoint(), int32 _Position = 0)
		: Step(_Step), Point(_Point), Position(_Position) {};
	int32 Step;
	FNPoint Point;
	int32 Position;
};

UCLASS(BlueprintType)
class NANSDIALOGSYSTEM_API UBTDialogPointsHandler : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	bool bDebug = false;

	UBTDialogPointsHandler();
	void Initialize(TScriptInterface<IBTStepsHandler> _StepsHandler, FString _BehaviorTreePathName,
		FString _AIPawnPathName);

	virtual void BeginDestroy() override;

	UFUNCTION(BlueprintCallable, Category = "PointsHandler")
	void AddPoints(FNPoint Point, int32 Position);

	UFUNCTION(BlueprintCallable, Category = "PointsHandler")
	UNDialogFactorUnit* GetLastResponse();

	UFUNCTION(BlueprintCallable, Category = "PointsHandler")
	UNDialogFactorUnit* GetLastResponseFromStep(const int32 SearchStep);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "PointsHandler")
	int32 GetDialogPoints(FNResponseCategory Category) const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "PointsHandler")
	float GetPoints() const;
	UFUNCTION(BlueprintCallable, Category = "PointsHandler")
	void Clear();

protected:
	UPROPERTY()
	TScriptInterface<IBTStepsHandler> StepsHandler;
	TMap<FGameplayTag, TArray<FNDialogFactorTypeSettings>> PointsMultipliers;
	FName PointsCollector;

	/** A HEAP on point by step, the HEAP allow to keep trace of action's chronologicaly */
	TArray<FBTPointInStep> HeapResponses;
	TArray<int32> FactorUnitKeys;
	FString BehaviorTreePathName;
	FString AIPawnPathName;

	UPROPERTY()
	UNFactorsFactoryClientAdapter* FactorsClient;
	float FactorsPointsAtStart;
};
