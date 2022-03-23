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

#include "AI/Service/BTService_PrepareDialogue.h"

#include "AIController.h"
#include "NDialogueSubsystem.h"
#include "NDSFunctionLibrary.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Misc/ErrorUtils.h"
#include "Service/BTDialoguePointsHandler.h"
#include "Service/DialogueBTHelpers.h"
#include "Setting/DialogueSystemSettings.h"
#include "UI/ResponseButtonWidget.h"

#define LOCTEXT_NAMESPACE "DialogueSystem"

UBTService_PrepareDialogue::UBTService_PrepareDialogue(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = "Prepare Dialogue";

	bNotifyTick = false;
	bTickIntervals = true;
	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = false;
}

void UBTService_PrepareDialogue::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);
	check(IsValid(AIOwner));
	UNDialogueSubsystem* DialSys = UNDSFunctionLibrary::GetDialogSubsystem();
	check(IsValid(DialSys));
	// False means the dialogue sequence with this AIOwner has been already started
	if (!DialSys->CreateDialogSequence(AIOwner)) return;

	const TSharedPtr<NBTDialoguePointsHandler>& PointsHandler = DialSys->GetPointsHandler(AIOwner);
	check(PointsHandler.IsValid());

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	const UBlackboardData* BBData = BlackboardComp->GetBlackboardAsset();

	if (!IsValid(BBData))
	{
		EDITOR_ERROR(
			"DialogSystem",
			LOCTEXT("NoBBDataSet", "No BB data set for dialog in"),
			static_cast<UObject*>(OwnerComp.GetCurrentTree())
		);
	}
	const FDialogueBehaviorTreeSettings Settings = UDialogueSystemSettings::Get()->BehaviorTreeSettings;
	const UStruct* StructSettings = FDialogueBehaviorTreeSettings::StaticStruct();
	TArray<FName> BBKeys;
	for (const auto& Key : BBData->Keys)
	{
		BBKeys.Add(Key.EntryName);
	}

	for (TFieldIterator<FProperty> It(StructSettings, EFieldIteratorFlags::ExcludeSuper); It; ++It)
	{
		FProperty* Property = *It;

		if (FNameProperty* NamedProperty = CastField<FNameProperty>(Property))
		{
			const FString& VariableName = NamedProperty->GetName();
			auto Category = NamedProperty->GetMetaData("Category");
			const void* ValueRaw = NamedProperty->ContainerPtrToValuePtr<uint8>(&Settings);
			const FName& BBKeyName = NamedProperty->GetPropertyValue(ValueRaw);
			if (Category.Contains(TEXT("BBKeys")) && !BBKeys.Contains(BBKeyName))
			{
				EDITOR_ERROR(
					"DialogSystem",
					FText::Format(LOCTEXT("MissingBBKey",
						"You have to set the key \"{0}\" in the Blackboard, see the config \"{1}\" in the dialog system settings"
					) , FText::FromName(BBKeyName), FText::FromString(VariableName)),
					static_cast<UObject*>(OwnerComp.GetCurrentTree())
				);
				return;
			}
		}
	}

	BBKeys.Empty();

	BTDialogDifficultyHandler = Cast<UBTDialogueDifficultyHandler>(
		BlackboardComp->GetValueAsObject(Settings.DifficultyHandlerKey)
	);

	if (!IsValid(BTDialogDifficultyHandler))
	{
		BTDialogDifficultyHandler = NewObject<UBTDialogueDifficultyHandler>(&OwnerComp);
		BTDialogDifficultyHandler->Initialize(OwnerComp);
		BlackboardComp->SetValueAsObject(Settings.DifficultyHandlerKey, BTDialogDifficultyHandler);
	}

	const auto PlayerHUD = NDialogueBTHelpers::GetPlayerHUD(OwnerComp, FString(__FUNCTION__));
	if (!IsValid(PlayerHUD.GetObject()))
	{
		return;
	}

	const FName PreviousUINameKey = Settings.PreviousUINameKey;
	const FName PreviousUIClassKey = Settings.PreviousUIClassKey;

	// Save previous UIPanel to reload it next time. 
	const FName PreviousName = BlackboardComp->GetValueAsName(PreviousUINameKey);
	if (PreviousName == NAME_None)
	{
		FName OldUIName;
		TSubclassOf<UUserWidget> OldUIClass;
		IDialogueHUD::Execute_GetFullCurrentUIPanel(PlayerHUD.GetObject(), OldUIClass, OldUIName);
		BlackboardComp->SetValueAsName(PreviousUINameKey, OldUIName);
		BlackboardComp->SetValueAsClass(PreviousUIClassKey, OldUIClass);
	}

	NDialogueBTHelpers::RemoveUIFromBlackboard(OwnerComp, BlackboardComp);
}

#if WITH_EDITOR
FName UBTService_PrepareDialogue::GetNodeIconName() const
{
	// TODO import my own icon
	return FName("BTEditor.Graph.BTNode.Task.PlaySound.Icon");
}
#endif	  // WITH_EDITOR

FString UBTService_PrepareDialogue::GetStaticDescription() const
{
	FString ReturnDesc;
	return ReturnDesc;
}
#undef LOCTEXT_NAMESPACE
