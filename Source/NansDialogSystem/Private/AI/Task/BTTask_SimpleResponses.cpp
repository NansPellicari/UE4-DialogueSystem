// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Task/BTTask_SimpleResponses.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "NansUMGExtent/Public/Blueprint/NansUserWidget.h"
#include "UI/ResponseButtonWidget.h"

#define LOCTEXT_NAMESPACE "DialogSystem"

UBTTask_SimpleResponses::UBTTask_SimpleResponses(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	bNotifyTick = true;
	bNotifyTaskFinished = true;
}

void UBTTask_SimpleResponses::ReceiveOnTick(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	// ...
}

#if WITH_EDITOR
FName UBTTask_SimpleResponses::GetNodeIconName() const
{
	// TODO import my own icon
	return FName("BTEditor.Graph.BTNode.Task.PlaySound.Icon");
}
#endif	  // WITH_EDITOR

#undef LOCTEXT_NAMESPACE
