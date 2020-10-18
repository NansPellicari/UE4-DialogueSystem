// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Task/BTTask_TalkToPlayer.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "NansUE4Utilities/public/Misc/ErrorUtils.h"
#include "NansUE4Utilities/public/Misc/TextLibrary.h"
#include "UI/DialogHUD.h"

#define LOCTEXT_NAMESPACE "DialogSystem"

EBTNodeResult::Type UBTTask_TalkToPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	OwnerComponent = &OwnerComp;
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	HUD = Cast<UDialogHUD>(Blackboard->GetValueAsObject(HUDName));

	if (!ensure(HUD != nullptr))
	{
		EDITOR_ERROR("DialogSystem", LOCTEXT("WrongHUDType", "The HUD set is not valid (UNansUserWidget is expected) in "));
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
	ReturnDesc += "\n\nHUD: " + HUDName.ToString();

	return ReturnDesc;
}

void UBTTask_TalkToPlayer::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
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
