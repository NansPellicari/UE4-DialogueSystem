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
#include "BTStepsLibrary.h"
#include "NansDialogueSystemLog.h"
#include "NDSFunctionLibrary.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Dialogue/DialogueSequence.h"
#include "Kismet/GameplayStatics.h"
#include "NansUE4Utilities/public/Misc/ErrorUtils.h"
#include "Service/DialogueBTHelpers.h"
#include "Setting/DialogueSystemSettings.h"

#define LOCTEXT_NAMESPACE "DialogueSystem"

void UBTDialoguePointsHandler::BeginDestroy()
{
	Super::BeginDestroy();
}

void UBTDialoguePointsHandler::Clear()
{
	bDebug = false;
	PlayerGASC->CancelAbilities(&UDialogueSystemSettings::Get()->TagsForDialogueAbility);
}

bool UBTDialoguePointsHandler::Initialize(
	UBTStepsHandlerContainer* InStepsHandler, UBehaviorTreeComponent& OwnerComp,
	FDialogueSequence DialogueSequence)
{
	StepsHandler = InStepsHandler;
	PlayerGASC = NDialogueBTHelpers::GetGASC(OwnerComp);
	BehaviorTreePathName = DialogueSequence.Name.ToString();
	AIPawnPathName = DialogueSequence.Owner;

	check(GetWorld());

	// TODO retrieve index for splitted screen 
	verify(UNDSFunctionLibrary::IsPlayerCanDialogue(&OwnerComp, 0));

	DialogComp = NDialogueBTHelpers::GetDialogueComponent(OwnerComp);
	verify(IsValid(DialogComp));

	DialogComp->AddSequence(DialogueSequence);

	// Activating Dialog Gameplay Activity
	FGameplayEventData Payload;
	Payload.Instigator = OwnerComp.GetAIOwner()->GetPawn();
	Payload.Target = PlayerGASC->GetOwnerActor();
	Payload.EventTag = UDialogueSystemSettings::Get()->TriggerAbilityTag;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		PlayerGASC->GetOwnerActor(),
		UDialogueSystemSettings::Get()->TriggerAbilityTag,
		Payload
	);
	return true;
}

void UBTDialoguePointsHandler::AddPoints(FNPoint Point, int32 Position)
{
	verify(IsValid(DialogComp));
	const FBTStep Step = UBTStepsLibrary::GetCurrent(StepsHandler);
	DialogComp->AddPoints(Point, Position, Step);
}

int32 UBTDialoguePointsHandler::GetDialoguePoints(FNDialogueCategory Category) const
{
	verify(IsValid(DialogComp));
	return DialogComp->GetDialoguePoints(Category);
}

bool UBTDialoguePointsHandler::HasResults(const TArray<FNDialogueHistorySearch> Searches,
	TArray<FNansConditionOperator> ConditionsOperators) const
{
	verify(IsValid(DialogComp));
	const bool bOldValue = DialogComp->bDebugSearch;
	DialogComp->bDebugSearch = bDebug;
	const bool Result = DialogComp->HasResultsOnSearches(Searches, ConditionsOperators);
	DialogComp->bDebugSearch = bOldValue;
	return Result;
}

bool UBTDialoguePointsHandler::HasResults(const FNDialogueHistorySearch& Search) const
{
	verify(IsValid(DialogComp));
	const bool bOldValue = DialogComp->bDebugSearch;
	DialogComp->bDebugSearch = bDebug;
	const bool bItHas = DialogComp->HasResults(Search);
	DialogComp->bDebugSearch = bOldValue;
	return bItHas;
}

#undef LOCTEXT_NAMESPACE
