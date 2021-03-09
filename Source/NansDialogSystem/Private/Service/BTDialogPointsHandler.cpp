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

#include "Service/BTDialogPointsHandler.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AIController.h"
#include "BTDialogueTypes.h"
#include "BTStepsLibrary.h"
#include "NansDialogSystemLog.h"
#include "NDSFunctionLibrary.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Dialogue/DialogueSequence.h"
#include "Kismet/GameplayStatics.h"
#include "NansUE4Utilities/public/Misc/ErrorUtils.h"
#include "Service/DialogBTHelpers.h"
#include "Setting/DialogSystemSettings.h"

#define LOCTEXT_NAMESPACE "DialogSystem"

void UBTDialogPointsHandler::BeginDestroy()
{
	Super::BeginDestroy();
}

void UBTDialogPointsHandler::Clear()
{
	bDebug = false;
	PlayerGASC->CancelAbilities(&UDialogSystemSettings::Get()->TagsForDialogAbility);
}

bool UBTDialogPointsHandler::Initialize(
	UBTStepsHandlerContainer* InStepsHandler, UBehaviorTreeComponent& OwnerComp,
	FDialogueSequence DialogueSequence)
{
	StepsHandler = InStepsHandler;
	PlayerGASC = NDialogBTHelpers::GetGASC(OwnerComp);
	BehaviorTreePathName = DialogueSequence.Name.ToString();
	AIPawnPathName = DialogueSequence.Owner;

	check(GetWorld());

	// TODO retrieve index for splitted screen 
	verify(UNDSFunctionLibrary::IsPlayerCanDialogue(&OwnerComp, 0));

	DialogComp = NDialogBTHelpers::GetDialogComponent(OwnerComp);
	verify(IsValid(DialogComp));

	DialogComp->AddSequence(DialogueSequence);

	// Activating Dialog Gameplay Activity
	FGameplayEventData Payload;
	// TODO Check: need more data ?
	Payload.Instigator = OwnerComp.GetAIOwner()->GetPawn();
	Payload.Target = PlayerGASC->GetOwnerActor();
	Payload.EventTag = UDialogSystemSettings::Get()->TriggerAbilityTag;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		PlayerGASC->GetOwnerActor(),
		UDialogSystemSettings::Get()->TriggerAbilityTag,
		Payload
	);
	return true;
}

void UBTDialogPointsHandler::AddPoints(FNPoint Point, int32 Position)
{
	verify(IsValid(DialogComp));
	const FBTStep Step = UBTStepsLibrary::GetCurrent(StepsHandler);
	DialogComp->AddPoints(Point, Position, Step);
}

int32 UBTDialogPointsHandler::GetDialogPoints(FNDialogueCategory Category) const
{
	verify(IsValid(DialogComp));
	return DialogComp->GetDialogPoints(Category);
}

bool UBTDialogPointsHandler::HasResults(const TArray<FNDialogueHistorySearch> Searches,
	TArray<FNansConditionOperator> ConditionsOperators) const
{
	verify(IsValid(DialogComp));
	const bool bOldValue = DialogComp->bDebugSearch;
	DialogComp->bDebugSearch = bDebug;
	const bool Result = DialogComp->HasResultsOnSearches(Searches, ConditionsOperators);
	DialogComp->bDebugSearch = bOldValue;
	return Result;
}

bool UBTDialogPointsHandler::HasResults(const FNDialogueHistorySearch& Search) const
{
	verify(IsValid(DialogComp));
	const bool bOldValue = DialogComp->bDebugSearch;
	DialogComp->bDebugSearch = bDebug;
	const bool bItHas = DialogComp->HasResults(Search);
	DialogComp->bDebugSearch = bOldValue;
	return bItHas;
}

#undef LOCTEXT_NAMESPACE
