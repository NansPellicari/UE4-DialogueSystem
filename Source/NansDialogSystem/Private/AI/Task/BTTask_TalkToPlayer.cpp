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

#include "AI/Task/BTTask_TalkToPlayer.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "NansUE4Utilities/public/Misc/ErrorUtils.h"
#include "NansUE4Utilities/public/Misc/TextLibrary.h"
#include "Service/DialogBTHelpers.h"
#include "Service/InteractiveBTHelpers.h"
#include "Setting/InteractiveSettings.h"
#include "UI/DialogHUD.h"
#include "UI/InteractiveHUDPlayer.h"

#define LOCTEXT_NAMESPACE "DialogSystem"

UBTTask_TalkToPlayer::UBTTask_TalkToPlayer(const FObjectInitializer& ObjectInitializer)
{
	const auto Settings = UInteractiveSettings::Get()->BehaviorTreeSettings;

	if (UINameKey == NAME_None)
	{
		UINameKey = Settings.UINameKey;
	}
}

EBTNodeResult::Type UBTTask_TalkToPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	OwnerComponent = &OwnerComp;
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	HUD = NDialogBTHelpers::GetHUDFromBlackboard(OwnerComp, Blackboard);

	if (!IsValid(HUD))
	{
		// Error is already manage in NDialogBTHelpers::GetHUDFromBlackboard()
		return EBTNodeResult::Aborted;
	}

	HUD->OnQuestion.Broadcast(Message);
	HUD->OnEndDisplayQuestion.AddUniqueDynamic(this, &UBTTask_TalkToPlayer::OnQuestionEnd);

	return EBTNodeResult::InProgress;
}

void UBTTask_TalkToPlayer::OnQuestionEnd()
{
	FinishLatentTask(*OwnerComponent, EBTNodeResult::Succeeded);
}

FString UBTTask_TalkToPlayer::GetStaticDescription() const
{
	FString ReturnDesc;
	if (Message.IsEmptyOrWhitespace())
	{
		ReturnDesc += "\n\nNo Message\n";
	}
	else
	{
		ReturnDesc += "\n\n" + UNTextLibrary::StringToLines("\"" + Message.ToString() + "\"", 50, "\t");
	}
	ReturnDesc += "\n\UINameKey: " + UINameKey.ToString();

	return ReturnDesc;
}

void UBTTask_TalkToPlayer::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTNodeResult::Type TaskResult)
{
	HUD->OnEndDisplayQuestion.RemoveAll(this);
	OwnerComponent = nullptr;
	HUD = nullptr;
}

#if WITH_EDITOR
FName UBTTask_TalkToPlayer::GetNodeIconName() const
{
	// TODO import my own icon
	return FName("BTEditor.Graph.BTNode.Task.PlaySound.Icon");
}
#endif	  // WITH_EDITOR

#undef LOCTEXT_NAMESPACE
