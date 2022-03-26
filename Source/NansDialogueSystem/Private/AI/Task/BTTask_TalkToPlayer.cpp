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

#include "AI/Task/BTTask_TalkToPlayer.h"

#include "NansDialogueSystemLog.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NansUE4Utilities/public/Misc/TextLibrary.h"
#include "Service/DialogueBTHelpers.h"
#include "Setting/DialogueSystemSettings.h"
#include "UI/DialogueUI.h"

#define LOCTEXT_NAMESPACE "DialogueSystem"

UBTTask_TalkToPlayer::UBTTask_TalkToPlayer(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	const auto Settings = UDialogueSystemSettings::Get()->BehaviorTreeSettings;

	if (UINameKey == NAME_None)
	{
		UINameKey = Settings.UINameKey;
	}
	NodeName = "Talk To Player";
}

EBTNodeResult::Type UBTTask_TalkToPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	OwnerComponent = MakeWeakObjectPtr(&OwnerComp);
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	DialogueUI = MakeWeakObjectPtr(NDialogueBTHelpers::GetUIFromBlackboard(OwnerComp, Blackboard));
	UE_LOG(LogTemp, Warning, TEXT("%s Here"), ANSI_TO_TCHAR(__FUNCTION__));

	if (!DialogueUI.IsValid())
	{
		// Error is already manage in NDialogueBTHelpers::GetUIFromBlackboard()
		return EBTNodeResult::Aborted;
	}

	DialogueUI->SetNPCText(Message, Title);
	DialogueUI->OnEndDisplayQuestion.AddUniqueDynamic(this, &UBTTask_TalkToPlayer::OnQuestionEnd);
	// TODO add more of these to helps debugging
	UE_VLOG(
		OwnerComponent.Get(),
		LogDialogueSystem,
		Log,
		TEXT("NPC talk to player: title: \"%s\", message: \"%s\""),
		*Title.ToString(),
		*Message.ToString()
	);

	return EBTNodeResult::InProgress;
}

void UBTTask_TalkToPlayer::OnQuestionEnd()
{
	FinishLatentTask(*OwnerComponent.Get(), EBTNodeResult::Succeeded);
}

FString UBTTask_TalkToPlayer::GetStaticDescription() const
{
	FString ReturnDesc;
	if (!Title.IsEmptyOrWhitespace())
	{
		ReturnDesc += "\n" + UNTextLibrary::StringToLines("\"" + Title.ToString() + "\"", 50, "\t");
		ReturnDesc += "\n";
	}

	if (Message.IsEmptyOrWhitespace())
	{
		ReturnDesc += "\nNo Message\n";
	}
	else
	{
		ReturnDesc += "\n" + UNTextLibrary::StringToLines("\"" + Message.ToString() + "\"", 50, "\t");
	}

	return ReturnDesc;
}

void UBTTask_TalkToPlayer::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTNodeResult::Type TaskResult)
{
	if (DialogueUI.IsValid())
	{
		DialogueUI->OnEndDisplayQuestion.RemoveAll(this);
	}
	OwnerComponent.Reset();
	DialogueUI.Reset();
}

#if WITH_EDITOR
FName UBTTask_TalkToPlayer::GetNodeIconName() const
{
	// TODO import my own icon
	return FName("BTEditor.Graph.BTNode.Task.PlaySound.Icon");
}
#endif	  // WITH_EDITOR

#undef LOCTEXT_NAMESPACE
