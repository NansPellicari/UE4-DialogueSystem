// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Task/BTTask_RemoveHUD.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/UserWidget.h"
#include "BlueprintNodeHelpers.h"
#include "Logging/MessageLog.h"
#include "Runtime/CoreUObject/Public/Misc/UObjectToken.h"

#define LOCTEXT_NAMESPACE "DialogSystem"

EBTNodeResult::Type UBTTask_RemoveHUD::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	UUserWidget* Widget = Cast<UUserWidget>(Blackboard->GetValueAsObject(HUD));

	// Widget not exists or already remove
	if (Widget == nullptr || Widget->IsInViewport() == false)
	{
		return EBTNodeResult::Type::Succeeded;
	}

	Widget->StopAllAnimations();
	Widget->RemoveFromViewport();

	return EBTNodeResult::Type::Succeeded;
}

FString UBTTask_RemoveHUD::GetStaticDescription() const
{
	FString ReturnDesc = Super::GetStaticDescription();

	ReturnDesc += ":\nHUD: " + HUD.ToString();

	return ReturnDesc;
}

#if WITH_EDITOR
FName UBTTask_RemoveHUD::GetNodeIconName() const
{
	// TODO import my own icon
	return FName("BTEditor.Graph.BTNode.Task.Wait.Icon");
}
#endif	  // WITH_EDITOR

#undef LOCTEXT_NAMESPACE
