// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Service/BTService_DialogPointsHandler.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "NansBehaviorSteps/Public/BTStepsHandler.h"
#include "NansUE4Utilities/public/Misc/ErrorUtils.h"
#include "NansUE4Utilities/public/Misc/TextLibrary.h"
#include "Service/BTDialogPointsHandler.h"

#define LOCTEXT_NAMESPACE "DialogSystem"

UBTService_DialogPointsHandler::UBTService_DialogPointsHandler(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = "Dialog Points Handler";

	bNotifyTick = true;
	bTickIntervals = true;
	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = false;
}

void UBTService_DialogPointsHandler::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	BTDialogPointsHandler = Cast<UBTDialogPointsHandler>(BlackboardComp->GetValueAsObject(PointsHandlerKeyName));

	if (BTDialogPointsHandler == nullptr)
	{
		BTDialogPointsHandler = NewObject<UBTDialogPointsHandler>(&OwnerComp);
		BlackboardComp->SetValueAsObject(PointsHandlerKeyName, BTDialogPointsHandler);
	}

	UObject* BTSteps = BlackboardComp->GetValueAsObject(StepsKeyName);
	if (BTSteps != nullptr && BTDialogPointsHandler != nullptr)
	{
		if (!BTSteps->Implements<UBTStepsHandler>())
		{
			EDITOR_ERROR(
				"DialogSystem", LOCTEXT("InvalidStepsHandlerClass", "Invalid class for Steps, should implements IBTStepsHandler"));
			return;
		}
		TScriptInterface<IBTStepsHandler> BTStepsHandler;
		BTStepsHandler.SetObject(BTSteps);
		BTStepsHandler.SetInterface(Cast<IBTStepsHandler>(BTSteps));
		BTDialogPointsHandler->Initialize(BTStepsHandler);
	}
}

#if WITH_EDITOR
FName UBTService_DialogPointsHandler::GetNodeIconName() const
{
	// TODO import my own icon
	return FName("BTEditor.Graph.BTNode.Task.PlaySound.Icon");
}
#endif	  // WITH_EDITOR

FString UBTService_DialogPointsHandler::GetStaticDescription() const
{
	FString ReturnDesc;
	ReturnDesc += "Points Handler Key: " + PointsHandlerKeyName.ToString();

	return ReturnDesc;
}
#undef LOCTEXT_NAMESPACE
