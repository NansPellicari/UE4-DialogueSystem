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
#include "StepsHandler.h"
#include "AI/Decorator/BTDecorator_CheckInStep.h"
#include "Component/PlayerDialogueComponent.h"
#include "Dialogue/DialogueHistorySearch.h"

struct FDialogueSequence;
struct FNDialogueFactorSettings;
class UNFactorsFactoryClientAdapter;
class UBehaviorTreeComponent;

class NANSDIALOGUESYSTEM_API NDialoguePointsHandler
{
public:
	NDialoguePointsHandler(const TSharedPtr<NStepsHandler>& InStepsHandler, UPlayerDialogueComponent* InDialogComp,
	                       const AAIController* InOwner, bool InbDebug);
	virtual ~NDialoguePointsHandler();
	void AddPoints(FNPoint Point, int32 Position);
	bool HasResults(const FNDialogueHistorySearch& Search) const;
	bool HasResults(const TArray<FNDialogueHistorySearch> Searches,
	                TArray<FNansConditionOperator> ConditionsOperators) const;
	int32 GetDialoguePoints(FNDialogueCategory Category) const;
	const AAIController* GetOwner() const;
	bool bDebug = false;
private:
	TSharedPtr<NStepsHandler> StepsHandler;
	TWeakObjectPtr<UPlayerDialogueComponent> DialogComp;
	TWeakObjectPtr<const AAIController> Owner;
};
