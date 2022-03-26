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

#include "NDialogueSubsystem.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "Service/DialoguePointsHandler.h"

#include "BTStepsSubsystem.h"
#include "NDSFunctionLibrary.h"
#include "Service/DialogueBTHelpers.h"
#include "Service/DialogueDifficultyHandler.h"
#include "Setting/DialogueSystemSettings.h"

#define LOCTEXT_NAMESPACE "DialogueSystem"

void UNDialogueSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	Collection.InitializeDependency(UBTStepsSubsystem::StaticClass());
}

ACharacter* UNDialogueSubsystem::GetPlayerCharacter() const
{
	const ULocalPlayer* Player = GetLocalPlayerChecked();
	const APlayerController* PC = Player->GetPlayerController(GetWorld());
	check(PC);
	ACharacter* Char = PC->GetCharacter();
	check(Char);
	return Char;
}

UAbilitySystemComponent* UNDialogueSubsystem::GetPlayerAbs() const
{
	const ULocalPlayer* Player = GetLocalPlayerChecked();
	const APlayerController* PC = Player->GetPlayerController(GetWorld());
	check(PC);
	const ACharacter* Char = PC->GetCharacter();
	check(Char);
	UAbilitySystemComponent* ABS = Char->FindComponentByClass<UAbilitySystemComponent>();
	if (!ensureMsgf(IsValid(ABS), TEXT("The UAbilitySystemComponent was not found")))
	{
		return nullptr;
	}
	return ABS;
}

void UNDialogueSubsystem::Deinitialize()
{
	Super::Deinitialize();
	PointsHandler.Reset();
	DifficultyHandler.Reset();
}

UBTStepsSubsystem& UNDialogueSubsystem::GetBTStepsSubsystem() const
{
	UBTStepsSubsystem* BTStepsSubsys = GetLocalPlayerChecked()->GetSubsystem<UBTStepsSubsystem>();
	check(BTStepsSubsys);
	return *BTStepsSubsys;
}

bool UNDialogueSubsystem::PlayerIsInDialogSequenceWith(const AAIController* Owner) const
{
	const UAbilitySystemComponent* PlayerAbs = GetPlayerAbs();
	verify(IsValid(PlayerAbs))
	const bool bIsPlayerCanDialogue = UNDSFunctionLibrary::IsPlayerABSCanDialogue(PlayerAbs);
	verify(bIsPlayerCanDialogue);
	return PointsHandler.IsValid() && PointsHandler->GetOwner() == Owner;
}

bool UNDialogueSubsystem::CreateDialogSequence(const AAIController* Owner)
{
	if (PlayerIsInDialogSequenceWith(Owner)) return false;

	TSharedPtr<NStepsHandler> StepsHandler = GetBTStepsSubsystem().GetStepsHandler(Owner);
	if (!StepsHandler.IsValid())
	{
		StepsHandler = GetBTStepsSubsystem().CreateStepsHandler(Owner);
	}
	check(StepsHandler.IsValid());
	UPlayerDialogueComponent* DialogComp = NDialogueBTHelpers::GetDialogueComponent(GetPlayerCharacter());
	check(DialogComp);

	PointsHandler = MakeShared<NDialoguePointsHandler>(StepsHandler, DialogComp, Owner, bDebugPointsHandler);
	DifficultyHandler = MakeShared<NDialogueDifficultyHandler>(
		MakeWeakObjectPtr(GetPlayerAbs()),
		bDebugDifficultyHandler
	);
	return true;
}

void UNDialogueSubsystem::EndDialogSequence(const AAIController* Owner)
{
	if (!ensureMsgf(PlayerIsInDialogSequenceWith(Owner), TEXT("Dialogue already stopped")))
	{
		return;
	}

	// TODO move this in a dedicated component BTStep
	// A dialog can be embed to another Behavior Tree,
	// or be just a part of a Behavior tree.
	// So StepsHandler should be completely independent from dialog.
	GetBTStepsSubsystem().RemoveStepsHandler(Owner);

	UAbilitySystemComponent* PlayerAbs = GetPlayerAbs();
	if (PlayerAbs)
	{
		PlayerAbs->CancelAbilities(&UDialogueSystemSettings::Get()->TagsForDialogueAbility);
	}
	PointsHandler.Reset();
	DifficultyHandler.Reset();
}

const TSharedPtr<NDialoguePointsHandler>& UNDialogueSubsystem::GetPointsHandler(const AAIController* Owner) const
{
	verify(IsValid(Owner));
	check(PlayerIsInDialogSequenceWith(Owner));
	return PointsHandler;
}

const TSharedPtr<NDialogueDifficultyHandler>& UNDialogueSubsystem::GetDifficultyHandler(
	const AAIController* Owner) const
{
	verify(IsValid(Owner));
	check(PlayerIsInDialogSequenceWith(Owner));
	return DifficultyHandler;
}

#undef LOCTEXT_NAMESPACE