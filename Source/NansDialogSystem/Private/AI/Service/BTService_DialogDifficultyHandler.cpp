// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Service/BTService_DialogDifficultyHandler.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "NansUE4Utilities/public/Misc/ErrorUtils.h"
#include "NansUE4Utilities/public/Misc/TextLibrary.h"
#include "Service/BTDialogDifficultyHandler.h"

#define LOCTEXT_NAMESPACE "DialogSystem"

UBTService_DialogDifficultyHandler::UBTService_DialogDifficultyHandler(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = "Dialog Difficulty Handler";

	bNotifyTick = true;
	bTickIntervals = true;
	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = false;
}

void UBTService_DialogDifficultyHandler::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	BTDialogDifficultyHandler = Cast<UBTDialogDifficultyHandler>(BlackboardComp->GetValueAsObject(DifficultyHandlerKeyName));

	if (BTDialogDifficultyHandler == nullptr)
	{
		BTDialogDifficultyHandler = NewObject<UBTDialogDifficultyHandler>(&OwnerComp);
		BlackboardComp->SetValueAsObject(DifficultyHandlerKeyName, BTDialogDifficultyHandler);
	}
}

#if WITH_EDITOR
FName UBTService_DialogDifficultyHandler::GetNodeIconName() const
{
	// TODO import my own icon
	return FName("BTEditor.Graph.BTNode.Task.PlaySound.Icon");
}
#endif	  // WITH_EDITOR

FString UBTService_DialogDifficultyHandler::GetStaticDescription() const
{
	FString ReturnDesc = Super::GetStaticDescription();
	ReturnDesc += "Difficulty Handler Key: " + DifficultyHandlerKeyName.ToString();

	return ReturnDesc;
}
#undef LOCTEXT_NAMESPACE
