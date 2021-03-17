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

#include "AI/Service/BTService_PrepareDialogue.h"

#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Dialogue/DialogueSequence.h"
#include "GameFramework/Character.h"
#include "Service/BTDialoguePointsHandler.h"
#include "Service/DialogueBTHelpers.h"
#include "Setting/DialogueSystemSettings.h"

#define LOCTEXT_NAMESPACE "DialogueSystem"

UBTService_PrepareDialogue::UBTService_PrepareDialogue(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = "Prepare Dialogue";

	bNotifyTick = false;
	bTickIntervals = true;
	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = false;
}

void UBTService_PrepareDialogue::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	const auto Settings = UDialogueSystemSettings::Get()->BehaviorTreeSettings;
	BTDialogPointsHandler = Cast<UBTDialoguePointsHandler>(BlackboardComp->GetValueAsObject(Settings.PointsHandlerKey));

	if (!IsValid(BTDialogPointsHandler))
	{
		BTDialogPointsHandler = NewObject<UBTDialoguePointsHandler>(&OwnerComp);
		BlackboardComp->SetValueAsObject(Settings.PointsHandlerKey, BTDialogPointsHandler);
		UBTStepsHandlerContainer* BTSteps = Cast<UBTStepsHandlerContainer>(
			BlackboardComp->GetValueAsObject(StepsKeyName)
		);

		if (BTSteps != nullptr && BTDialogPointsHandler != nullptr)
		{
			FDialogueSequence NewDialogueSequence;
			NewDialogueSequence.Name = OwnerComp.GetCurrentTree()->GetFName();
			// TODO use the new Protagonist name
			NewDialogueSequence.Owner = ActorOwner->GetPathName();

			BTDialogPointsHandler->Initialize(
				BTSteps,
				OwnerComp,
				NewDialogueSequence
			);

			BTDialogPointsHandler->bDebug = bDebugPointsHandler;

			// TODO create a decorator for this
			// if (!bCanDialogue)
			// {
			// 	OwnerComp.StopLogic(FString("Player can't speak"));
			// }
		}
	}

	BTDialogDifficultyHandler = Cast<UBTDialogueDifficultyHandler>(
		BlackboardComp->GetValueAsObject(Settings.DifficultyHandlerKey)
	);

	if (!IsValid(BTDialogDifficultyHandler))
	{
		BTDialogDifficultyHandler = NewObject<UBTDialogueDifficultyHandler>(&OwnerComp);
		BTDialogDifficultyHandler->Initialize(OwnerComp);
		BlackboardComp->SetValueAsObject(Settings.DifficultyHandlerKey, BTDialogDifficultyHandler);
	}

	const auto PlayerHUD = NDialogueBTHelpers::GetPlayerHUD(OwnerComp, FString(__FUNCTION__));
	if (!IsValid(PlayerHUD.GetObject()))
	{
		return;
	}

	const FName PreviousUINameKey = Settings.PreviousUINameKey;
	const FName PreviousUIClassKey = Settings.PreviousUIClassKey;

	// Save previous UIPanel to reload it next time. 
	const FName PreviousName = BlackboardComp->GetValueAsName(PreviousUINameKey);
	if (PreviousName == NAME_None)
	{
		FName OldUIName;
		TSubclassOf<UUserWidget> OldUIClass;
		IDialogueHUD::Execute_GetFullCurrentUIPanel(PlayerHUD.GetObject(), OldUIClass, OldUIName);
		BlackboardComp->SetValueAsName(PreviousUINameKey, OldUIName);
		BlackboardComp->SetValueAsClass(PreviousUIClassKey, OldUIClass);
	}

	NDialogueBTHelpers::RemoveUIFromBlackboard(OwnerComp, BlackboardComp);
}

#if WITH_EDITOR
FName UBTService_PrepareDialogue::GetNodeIconName() const
{
	// TODO import my own icon
	return FName("BTEditor.Graph.BTNode.Task.PlaySound.Icon");
}
#endif	  // WITH_EDITOR

FString UBTService_PrepareDialogue::GetStaticDescription() const
{
	FString ReturnDesc;
	return ReturnDesc;
}
#undef LOCTEXT_NAMESPACE
