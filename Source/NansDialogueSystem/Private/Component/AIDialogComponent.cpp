﻿// Copyright 2020-present Nans Pellicari (nans.pellicari@gmail.com).
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

#include "Component/AIDialogComponent.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Service/BTDialogueDifficultyHandler.h"
#include "Service/BTDialoguePointsHandler.h"
#include "Service/DialogueBTHelpers.h"
#include "Setting/DialogueSystemSettings.h"

// Sets default values for this component's properties
UAIDialogComponent::UAIDialogComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

// Called when the game starts
void UAIDialogComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

void UAIDialogComponent::OnBTTaskAbort(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const auto Settings = UDialogueSystemSettings::Get()->BehaviorTreeSettings;
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	UBTDialoguePointsHandler* BTDialogPointsHandler =
		Cast<UBTDialoguePointsHandler>(BlackboardComp->GetValueAsObject(Settings.PointsHandlerKey));

	if (IsValid(BTDialogPointsHandler))
	{
		// This will end player's dialog skill
		BTDialogPointsHandler->Clear();
	}
	UBTDialogueDifficultyHandler* BTDialogDifficultyHandler =
		Cast<UBTDialogueDifficultyHandler>(BlackboardComp->GetValueAsObject(Settings.PointsHandlerKey));

	if (IsValid(BTDialogDifficultyHandler))
	{
		BTDialogDifficultyHandler->Clear();
	}
	NDialogueBTHelpers::RemoveUIFromBlackboard(OwnerComp, BlackboardComp);
}
