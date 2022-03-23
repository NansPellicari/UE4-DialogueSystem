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

#include "Service/BTDialoguePointsHandler.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AIController.h"
#include "BTDialogueTypes.h"
#include "NDSFunctionLibrary.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Dialogue/DialogueSequence.h"
#include "Kismet/GameplayStatics.h"
#include "NansUE4Utilities/public/Misc/ErrorUtils.h"
#include "Service/DialogueBTHelpers.h"
#include "Setting/DialogueSystemSettings.h"

#define LOCTEXT_NAMESPACE "DialogueSystem"

NBTDialoguePointsHandler::NBTDialoguePointsHandler(const TSharedPtr<NStepsHandler>& InStepsHandler,
	UPlayerDialogueComponent* InDialogComp, const AAIController* InOwner, bool InbDebug)
{
	Owner = MakeWeakObjectPtr(InOwner);
	StepsHandler = InStepsHandler;
	DialogComp = MakeWeakObjectPtr(InDialogComp);
	bDebug = InbDebug;
	check(DialogComp.IsValid())
	const UBrainComponent* Brain = Owner->GetBrainComponent();
	check(Brain);
	const UBehaviorTreeComponent* BTComp = Cast<UBehaviorTreeComponent>(Brain);
	check(BTComp);

	FDialogueSequence NewDialogueSequence;
	NewDialogueSequence.Name = BTComp->GetCurrentTree()->GetFName();
	// TODO use the new Protagonist name
	NewDialogueSequence.Owner = Owner->GetPathName();
	DialogComp->AddSequence(NewDialogueSequence);

	// Activating Dialog Gameplay Activity
	FGameplayEventData Payload;
	Payload.Instigator = Owner->GetPawn();
	Payload.Target = InDialogComp->GetOwner();
	Payload.EventTag = UDialogueSystemSettings::Get()->TriggerAbilityTag;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		DialogComp->GetOwner(),
		UDialogueSystemSettings::Get()->TriggerAbilityTag,
		Payload
	);
}

NBTDialoguePointsHandler::~NBTDialoguePointsHandler()
{
	Owner.Reset();
	DialogComp.Reset();
}

void NBTDialoguePointsHandler::AddPoints(FNPoint Point, int32 Position)
{
	verify(DialogComp.IsValid());
	const FNStep Step = StepsHandler->GetCurrent();
	DialogComp->AddPoints(Point, Position, Step);
}

int32 NBTDialoguePointsHandler::GetDialoguePoints(FNDialogueCategory Category) const
{
	verify(DialogComp.IsValid());
	return DialogComp->GetDialoguePoints(Category);
}

const AAIController* NBTDialoguePointsHandler::GetOwner() const
{
	return Owner.Get();
}

bool NBTDialoguePointsHandler::HasResults(const TArray<FNDialogueHistorySearch> Searches,
	TArray<FNansConditionOperator> ConditionsOperators) const
{
	verify(DialogComp.IsValid());
	const bool bOldValue = DialogComp->bDebugSearch;
	DialogComp->bDebugSearch = bDebug;
	const bool Result = DialogComp->HasResultsOnSearches(Searches, ConditionsOperators);
	DialogComp->bDebugSearch = bOldValue;
	return Result;
}

bool NBTDialoguePointsHandler::HasResults(const FNDialogueHistorySearch& Search) const
{
	verify(DialogComp.IsValid());
	const bool bOldValue = DialogComp->bDebugSearch;
	DialogComp->bDebugSearch = bDebug;
	const bool bItHas = DialogComp->HasResults(Search);
	DialogComp->bDebugSearch = bOldValue;
	return bItHas;
}

#undef LOCTEXT_NAMESPACE
