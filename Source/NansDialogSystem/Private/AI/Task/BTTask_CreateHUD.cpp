// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Task/BTTask_CreateHUD.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/UserWidget.h"
#include "NansExtension/Public/UMG/Blueprint/NansUserWidget.h"
#include "NansUE4Utilities/public/Misc/ErrorUtils.h"

#define LOCTEXT_NAMESPACE "DialogSystem"

EBTNodeResult::Type UBTTask_CreateHUD::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

	if (HUDToCreate == NULL || HUDToCreate->GetDefaultObject()->GetClass()->GetFullName().IsEmpty())
	{
		EDITOR_ERROR("DialogSystem", LOCTEXT("NotSetHUDToCreate", "Set a HUD class (inherited from UNansUserWidget) in "));
		return EBTNodeResult::Type::Aborted;
	}

	UNansUserWidget* OldWidget = Cast<UNansUserWidget>(Blackboard->GetValueAsObject(HUD));

	// Same widget already exists
	if (OldWidget != nullptr && OldWidget->IsInViewport() == true &&
		OldWidget->GetClass() == HUDToCreate->GetDefaultObject()->GetClass())
	{
		OldWidget->StopAllAnimations();
		return EBTNodeResult::Type::Succeeded;
	}

	if (OldWidget != nullptr && OldWidget->IsInViewport() == true)
	{
		OldWidget->RemoveFromViewport();
	}

	UNansUserWidget* NewWidget = CreateWidget<UNansUserWidget>(GetWorld(), HUDToCreate->GetDefaultObject()->GetClass());
	Blackboard->SetValueAsObject(HUD, NewWidget);
	NewWidget->AddToViewport();

	return EBTNodeResult::Type::Succeeded;
}

FString UBTTask_CreateHUD::GetStaticDescription() const
{
	FString ReturnDesc = Super::GetStaticDescription();

	ReturnDesc += ":\nHUD: " + HUD.ToString();

	ReturnDesc += "\nHUD To Create: ";
	if (HUDToCreate != NULL && !HUDToCreate->GetDefaultObject()->GetClass()->GetFullName().IsEmpty())
	{
		ReturnDesc += HUDToCreate->GetDefaultObject()->GetClass()->GetName();
	}
	else
	{
		ReturnDesc += "None";
	}

	return ReturnDesc;
}

#if WITH_EDITOR
FName UBTTask_CreateHUD::GetNodeIconName() const
{
	// TODO import my own icon
	return FName("BTEditor.Graph.BTNode.Task.Wait.Icon");
}
#endif	  // WITH_EDITOR

#undef LOCTEXT_NAMESPACE
