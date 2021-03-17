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

#include "AI/Decorator/BTDecorator_CheckDialogueResults.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Misc/NansComparator.h"
#include "NansUE4Utilities/public/Misc/ErrorUtils.h"
#include "Service/BTDialoguePointsHandler.h"
#include "Service/DialogueBTHelpers.h"
#include "Setting/DialogueSystemSettings.h"

#define LOCTEXT_NAMESPACE "DialogSystem"

UBTDecorator_CheckDialogueResults::UBTDecorator_CheckDialogueResults(const FObjectInitializer& ObjectInitializer) :
	Super(
		ObjectInitializer
	)
{
	NodeName = "Check: Dialogue Results";
}

bool UBTDecorator_CheckDialogueResults::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory) const
{
	const UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	const auto Settings = UDialogueSystemSettings::Get()->BehaviorTreeSettings;

	UBTDialoguePointsHandler* PointsHandler = Cast<UBTDialoguePointsHandler>(
		BlackboardComp->GetValueAsObject(Settings.PointsHandlerKey)
	);

	if (PointsHandler == nullptr)
	{
		EDITOR_ERROR(
			"DialogSystem",
			LOCTEXT("InvalidPointsHandlerKey", "Invalid key for PointsHandler in "),
			reinterpret_cast<UObject*>(OwnerComp.GetCurrentTree())
		);
		return false;
	}

	return PointsHandler->HasResults(DialogueHistorySearches, ConditionsOperators);
}

FString UBTDecorator_CheckDialogueResults::GetStaticDescription() const
{
	FString ReturnDesc;

	ReturnDesc += FString("\nConditions:");
	for (int32 Index = 0; Index != DialogueHistorySearches.Num(); ++Index)
	{
		FNDialogueHistorySearch Condition = DialogueHistorySearches[Index];

		ReturnDesc += FString::Printf(
			TEXT("\n%s %s"),
			*UNansComparator::BuildKeyFromIndex(Index),
			*Condition.ToString()
		);
	}

	if (ConditionsOperators.Num() <= 0) return ReturnDesc;

	ReturnDesc += FString("\n\nConditions Operators:");
	ReturnDesc += UNansComparator::OperatorsToString(ConditionsOperators);

	return ReturnDesc;
}
#undef LOCTEXT_NAMESPACE
