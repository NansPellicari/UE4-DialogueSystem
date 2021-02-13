//  Copyright 2020-present Nans Pellicari (nans.pellicari@gmail.com).
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

#include "Service/BTDialogPointsHandler.h"


#include "AbilitySystemBlueprintLibrary.h"
#include "AIController.h"
#include "BTDialogueTypes.h"
#include "NansDialogSystemLog.h"
#include "NDSFunctionLibrary.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Dialogue/DialogueSequence.h"
#include "Engine/GameInstance.h"
#include "Factor/DialogFactorUnit.h"
#include "Kismet/GameplayStatics.h"
#include "NansBehaviorSteps/Public/BTStepsHandler.h"
#include "NansFactorsFactoryCore/Public/FactorState.h"
#include "NansFactorsFactoryUE4/Public/FactorsFactoryClientAdapter.h"
#include "NansFactorsFactoryUE4/Public/FactorsFactoryGameInstance.h"
#include "NansFactorsFactoryUE4/Public/FactorUnit/FactorUnitView.h"
#include "NansFactorsFactoryUE4/Public/Operator/OperatorProviders.h"
#include "NansUE4Utilities/public/Misc/ErrorUtils.h"
#include "Service/DialogBTHelpers.h"
#include "Setting/DialogSystemSettings.h"

#define LOCTEXT_NAMESPACE "DialogSystem"

UBTDialogPointsHandler::UBTDialogPointsHandler()
{
	UDialogSystemSettings::Get()->GetPointsMultipliersConfigs(PointsMultipliers);
	PointsCollector = UDialogSystemSettings::Get()->PointsCollector.Name;
}

void UBTDialogPointsHandler::BeginDestroy()
{
	HeapResponses.Empty();
	FactorsPointsAtStart = 0;
	FactorUnitKeys.Empty();
	Super::BeginDestroy();
}

void UBTDialogPointsHandler::Clear()
{
	HeapResponses.Empty();
	FactorsPointsAtStart = 0;
	FactorUnitKeys.Empty();
	bDebug = false;
	PlayerABS->CancelAbilities(&UDialogSystemSettings::Get()->TagsForDialogAbility);
}

bool UBTDialogPointsHandler::HasResults(const TArray<FNDialogueHistorySearch> Searches,
	TArray<FNansConditionOperator> ConditionsOperators)
{
	verify(IsValid(DialogComp));
	const bool HasConditionsOperator = ConditionsOperators.Num() > 0;
	TMap<FString, BoolStruct*> ConditionsResults;
	int32 Idx = 0;
	if (Searches.Num() > 0)
	{
		for (auto& Search : Searches)
		{
			FString SearchKey = UNansComparator::BuildKeyFromIndex(Idx);
			const bool bResult = HasResults(Search);
			if (bDebug)
			{
				UE_LOG(
					LogDialogSystem,
					Display,
					TEXT("%s search: %s - results: %i"),
					*SearchKey,
					*Search.ToString(),
					bResult
				);
			}
			ConditionsResults.Add(SearchKey, new BoolStruct(bResult));
			++Idx;
			if (HasConditionsOperator == false && bResult == false)
			{
				break;
			}
		}
	}
	if (ConditionsResults.Num() <= 0)
	{
		return false;
	}
	if (HasConditionsOperator == false)
	{
		FString Key = UNansComparator::BuildKeyFromIndex(ConditionsResults.Num() - 1);
		return ConditionsResults.FindRef(Key)->value;
	}

	return UNansComparator::EvaluateOperators(ConditionsOperators, ConditionsResults, bDebug);
}

bool UBTDialogPointsHandler::HasResults(const FNDialogueHistorySearch& Search)
{
	verify(IsValid(DialogComp));
	const TArray<FDialogueResult> Results = DialogComp->SearchResults(Search);
	return Results.Num() > 0;
}

bool UBTDialogPointsHandler::Initialize(
	TScriptInterface<IBTStepsHandler> InStepsHandler, UBehaviorTreeComponent& OwnerComp,
	FDialogueSequence DialogueSequence)
{
	StepsHandler = InStepsHandler;
	PlayerABS = NDialogBTHelpers::GetABS(OwnerComp);
	BehaviorTreePathName = DialogueSequence.Name.ToString();
	AIPawnPathName = DialogueSequence.Owner;

	check(GetWorld());

	// TODO retrieve index for splitted screen 
	verify(UNDSFunctionLibrary::IsPlayerCanDialogue(&OwnerComp, 0));

	DialogComp = NDialogBTHelpers::GetDialogComponent(OwnerComp);
	verify(IsValid(DialogComp));

	DialogComp->AddSequence(DialogueSequence);

	// Activating Dialog Gameplay Activity
	FGameplayEventData Payload;
	// TODO Check: need more data ?
	Payload.Instigator = OwnerComp.GetAIOwner()->GetPawn();
	Payload.Target = PlayerABS->GetOwnerActor();
	Payload.EventTag = UDialogSystemSettings::Get()->TriggerAbilityTag;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		PlayerABS->GetOwnerActor(),
		UDialogSystemSettings::Get()->TriggerAbilityTag,
		Payload
	);
	return true;
}

void UBTDialogPointsHandler::AddPoints(FNPoint Point, int32 Position)
{
	int32 Step = IBTStepsHandler::Execute_GetCurrentStep(StepsHandler.GetObject());

	TSubclassOf<UGameplayEffect> GEffect = Point.EffectOnEarned;
	if (!IsValid(GEffect))
	{
		EDITOR_ERROR(
			"DialogSystem",
			FText::Format(
				LOCTEXT("EffectOnEarnedMissing",
					"No effect on earned is set for point type {0}"),
				FText::FromString(Point.Category.Name.ToString()))
		);
		return;
	}
	FString BaseName = TEXT("Step");
	BaseName.AppendInt(Step);
	FGameplayEffectContextHandle FxContextHandle = PlayerABS->MakeEffectContext();
	FDialogueResult DialogData;
	DialogData.Difficulty = Point.Difficulty;
	DialogData.Position = Position;
	DialogData.CategoryName = Point.Category.Name;
	DialogData.BlockName = FName(BaseName);
	DialogData.InitialPoints = Point.Point;
	UNDSFunctionLibrary::EffectContextAddPointsData(FxContextHandle, DialogData);
	FGameplayEffectSpecHandle SpecHandle = PlayerABS->MakeOutgoingSpec(GEffect, Point.Difficulty, FxContextHandle);
	UAbilitySystemBlueprintLibrary::AddAssetTag(SpecHandle, Point.Category.Name);
	UAbilitySystemBlueprintLibrary::AddAssetTag(SpecHandle, UDialogSystemSettings::Get()->TagToIdentifyDialogEffect);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle,
		UDialogSystemSettings::Get()->PointMagnitudeTag,
		Point.Point
	);
	PlayerABS->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

int32 UBTDialogPointsHandler::GetDialogPoints(FNResponseCategory Category) const
{
	verify(IsValid(DialogComp));
	int32 TotalPoints = 0;
	int32 Points = 0;
	FNDialogueHistorySearch Search;
	Search.DialogName.bIsAll = true;
	Search.PropertyName = ENPropertyValue::All;
	const TArray<FDialogueResult> Results = DialogComp->SearchResults(Search);

	for (const FDialogueResult& Result : Results)
	{
		if (Result.CategoryName == Category.Name)
		{
			Points += Result.InitialPoints;
		}
	}
	return Points;
}

#undef LOCTEXT_NAMESPACE
