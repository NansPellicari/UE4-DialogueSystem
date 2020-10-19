// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Service/BTService_ClearDialog.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "NansBehaviorSteps/Public/BTStepsHandler.h"
#include "NansUE4Utilities/public/Misc/ErrorUtils.h"
#include "NansUE4Utilities/public/Misc/TextLibrary.h"
#include "Service/BTDialogPointsHandler.h"

#define LOCTEXT_NAMESPACE "DialogSystem"

UBTService_ClearDialog::UBTService_ClearDialog(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = "Clear Dialog";

	bNotifyTick = false;
	bTickIntervals = false;
	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = false;
}

void UBTService_ClearDialog::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	UBTDialogPointsHandler* BTDialogPointsHandler =
		Cast<UBTDialogPointsHandler>(BlackboardComp->GetValueAsObject(PointsHandlerKeyName));

	if (BTDialogPointsHandler == nullptr)
	{
		return;
	}

	BTDialogPointsHandler->Clear();
}

#if WITH_EDITOR
FName UBTService_ClearDialog::GetNodeIconName() const
{
	// TODO import my own icon
	return FName("BTEditor.Graph.BTNode.Task.PlaySound.Icon");
}
#endif	  // WITH_EDITOR

FString UBTService_ClearDialog::GetStaticDescription() const
{
	FString ReturnDesc;
	ReturnDesc += "Points Handler Key: " + PointsHandlerKeyName.ToString();

	return ReturnDesc;
}
#undef LOCTEXT_NAMESPACE
