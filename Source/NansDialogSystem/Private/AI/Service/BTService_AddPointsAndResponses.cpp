// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Service/BTService_AddPointsAndResponses.h"

#include "BTDialogueResponseContainer.h"
#include "BTStepsWithPoints.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NansUE4Utilities/public/Misc/ErrorUtils.h"
#include "NansUE4Utilities/public/Misc/TextLibrary.h"
#include "UI/ResponseButtonWidget.h"

#define LOCTEXT_NAMESPACE "MyBTServices"

UBTService_AddPointsAndResponses::UBTService_AddPointsAndResponses(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = "Add Points and Responses";

	bNotifyTick = false;
	bTickIntervals = false;
	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = false;
}

void UBTService_AddPointsAndResponses::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	BTSteps = Cast<UBTStepsWithPoints>(BlackboardComp->GetValueAsObject(StepsKeyName));

	if (BTSteps == nullptr)
	{
		EDITOR_ERROR("DialogSystem", LOCTEXT("InvalidStepsHandlerKey", "Invalid key for Steps handler in "));
		return;
	}

	ResponseContainer = Cast<UBTDialogueResponseContainer>(BlackboardComp->GetValueAsObject(ResponseContainerName));

	FPoint Point;
	int32 Position = 0;
	Point.Point = 0;
	Point.Alignment = EAlignment::None;

	if (ResponseContainer != nullptr)
	{
		FBTDialogueResponse Response = ResponseContainer->GetResponse();
		Point.Point = Response.Level;
		Point.Alignment = Response.Alignment;
		Position = Response.DisplayOrder;
		ResponseContainer->MarkPendingKill();
	}

	BTSteps->AddPoints(Point, Position);

	BlackboardComp->ClearValue(ResponseContainerName);
}

#if WITH_EDITOR
FName UBTService_AddPointsAndResponses::GetNodeIconName() const
{
	// TODO import my own icon
	return FName("BTEditor.Graph.BTNode.Task.PlaySound.Icon");
}
#endif	  // WITH_EDITOR

FString UBTService_AddPointsAndResponses::GetStaticDescription() const
{
	FString ReturnDesc;
	ReturnDesc += "Steps: " + StepsKeyName.ToString();
	ReturnDesc += "\nResponseContainer Key: " + ResponseContainerName.ToString();

	return ReturnDesc;
}
#undef LOCTEXT_NAMESPACE
