// Copyright 2020-present Nans Pellicari (nans.pellicari@gmail.com).
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

#include "AI/Task/BTTask_NoteToPlayer.h"

#include "AIController.h"
#include "Messageable.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Pawn.h"
#include "NansUE4Utilities/Public/MessageTypes.h"
#include "NansUE4Utilities/public/Misc/ErrorUtils.h"
#include "NansUE4Utilities/public/Misc/TextLibrary.h"
#include "NansUMGExtent/Public/HUD/MessageableHUD.h"
#include "Service/DialogBTHelpers.h"

#define LOCTEXT_NAMESPACE "DialogSystem"

EBTNodeResult::Type UBTTask_NoteToPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	OwnerComponent = &OwnerComp;

	AAIController* Controller = OwnerComp.GetAIOwner();
	if (!ensure(Controller != nullptr)) return EBTNodeResult::Aborted;

	APawn* Pawn = Controller->GetPawn();
	if (!ensure(Pawn != nullptr)) return EBTNodeResult::Aborted;

	if (MessageDuration <= 0)
	{
		EDITOR_ERROR(
			"DialogSystem",
			LOCTEXT("NoteToPlayerWrongDisplayMessageDuration", "DisplayMessageDuration need to be superior to 0 in ")
		);
		return EBTNodeResult::Aborted;
	}
	if (!ensure(GEngine != nullptr && GetWorld() != nullptr))
	{
		return EBTNodeResult::Aborted;
	}

	FNFlashMessage SentMessage;

	SentMessage.Message = Message;
	SentMessage.TimeDuration = MessageDuration;

	TArray<UActorComponent*> PawnComps = Pawn->GetComponentsByInterface(UNMessageable::StaticClass());

	if (WhereToDisplay == EMessageDisplayOn::PlayerHUD || PawnComps.Num() <= 0)
	{
		const TScriptInterface<IDialogueHUD> DialogHUD = NDialogBTHelpers::GetPlayerHUD(OwnerComp, FString(__FUNCTION__));
		AMessageableHUD* HUD = Cast<AMessageableHUD>(DialogHUD.GetObject());

		if (!IsValid(HUD))
		{
			EDITOR_ERROR(
				"DialogSystem",
				LOCTEXT("NoteToPlayerNoCorresspondingPlayerHUD",
					"Player HUD need to be an instance of AMessageableHUD in ")
			);
			return EBTNodeResult::Aborted;
		}

		TScriptInterface<INMessageable> MessageWidget;
		MessageWidget.SetInterface(Cast<INMessageable>(HUD));
		MessageWidget.SetObject(HUD);
		MessageWidgets.Add(MessageWidget);
	}

	if (WhereToDisplay == EMessageDisplayOn::NPC && PawnComps.Num() > 0)
	{
		for (auto PawnComp : PawnComps)
		{
			TScriptInterface<INMessageable> MessageWidget;
			MessageWidget.SetInterface(Cast<INMessageable>(PawnComp));
			MessageWidget.SetObject(PawnComp);
			MessageWidgets.Add(MessageWidget);
		}
	}

	for (auto MessageWidget : MessageWidgets)
	{
		INMessageable::Execute_SetMessage(MessageWidget.GetObject(), SentMessage);
		if (WaitAfterMessage)
			MessageWidget->OnNativeEndDisplayMessage().AddUObject(
				this,
				&UBTTask_NoteToPlayer::OnMessageEnd
			);
	}

	return WaitAfterMessage ? EBTNodeResult::InProgress : EBTNodeResult::Succeeded;
}

void UBTTask_NoteToPlayer::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);

	if (MessageWidgets.Num() > 0)
	{
		for (auto MessageWidget : MessageWidgets)
		{
			if (WaitAfterMessage) MessageWidget->OnNativeEndDisplayMessage().RemoveAll(this);
		}
		MessageWidgets.Empty();
	}

	OwnerComponent = nullptr;
}

void UBTTask_NoteToPlayer::OnMessageEnd()
{
	if (WaitAfterMessage == false) return;

	MessageFinished++;

	if (MessageFinished == MessageWidgets.Num())
	{
		MessageFinished = 0;
		FinishLatentTask(*OwnerComponent, EBTNodeResult::Succeeded);
	}
}

FString UBTTask_NoteToPlayer::GetStaticDescription() const
{
	FString ReturnDesc = Super::GetStaticDescription();
	if (Message.IsEmptyOrWhitespace())
	{
		ReturnDesc += "\n\nNo Message\n";
	}
	else
	{
		ReturnDesc += "\n\n" + UNTextLibrary::StringToLines("\"" + Message.ToString() + "\"", 50, "\t");
	}

	ReturnDesc += "\n\nMessage Duration: " + FString::SanitizeFloat(MessageDuration);
	ReturnDesc += "\nWhere To Display: " + ENUM_TO_STRING(EMessageDisplayOn, WhereToDisplay);
	ReturnDesc += "\nWait After Message: ";
	ReturnDesc += WaitAfterMessage ? "X" : "0";

	return ReturnDesc;
}

#if WITH_EDITOR
FName UBTTask_NoteToPlayer::GetNodeIconName() const
{
	// TODO import my own icon
	return FName("BTEditor.Graph.BTNode.Task.PlaySound.Icon");
}
#endif	  // WITH_EDITOR

#undef LOCTEXT_NAMESPACE
