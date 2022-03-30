// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTTask_EndDialogue.h"

#include "NDialogueSubsystem.h"
#include "NDSFunctionLibrary.h"
#include "Service/DialogueBTHelpers.h"

UBTTask_EndDialogue::UBTTask_EndDialogue(const FObjectInitializer& ObjectInitializer)
{
	NodeName = "End Dialogue";
}

EBTNodeResult::Type UBTTask_EndDialogue::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	const AAIController* AIOwner = OwnerComp.GetAIOwner();
	check(IsValid(AIOwner));
	UNDialogueSubsystem* DialSys = UNDSFunctionLibrary::GetDialogSubsystem();
	check(IsValid(DialSys));
	DialSys->EndDialogSequence(AIOwner);
	NDialogueBTHelpers::RemoveUIFromBlackboard(OwnerComp, BlackboardComp);
	return EBTNodeResult::Succeeded;
}

#if WITH_EDITOR
FName UBTTask_EndDialogue::GetNodeIconName() const
{
	// TODO import my own icon
	return FName("BTEditor.Graph.BTNode.Task.PlaySound.Icon");
}
#endif	  // WITH_EDITOR
