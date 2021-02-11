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
#include "NDSFunctionLibrary.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
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
	PlayerABS->CancelAbilities(&UDialogSystemSettings::Get()->TagsForDialogAbility);
}

bool UBTDialogPointsHandler::Initialize(
	TScriptInterface<IBTStepsHandler> InStepsHandler, UBehaviorTreeComponent& OwnerComp, FString InAIPawnPathName,
	UAbilitySystemComponent* InPlayerABS)
{
	StepsHandler = InStepsHandler;
	PlayerABS = InPlayerABS;
	BehaviorTreePathName = OwnerComp.GetPathName();
	AIPawnPathName = InAIPawnPathName;

	check(GetWorld());

	UGameInstance* GI = UGameplayStatics::GetGameInstance(this);
	checkf(
		GI->Implements<UNFactorsFactoryGameInstance>(),
		TEXT("The GameInstance should implements INFactorsFactoryGameInstance")
	);

	FactorsClient = INFactorsFactoryGameInstance::Execute_GetFactorsFactoryClient(GI);

	NFactorState* State = new NFactorState();
	FactorsClient->GetState(PointsCollector, *State);
	FactorsPointsAtStart = State->Compute();


	// ==================
	// Replace with that:
	// ==================
	// TODO retrieve index for splitted screen 
	verify(UNDSFunctionLibrary::IsPlayerCanDialogue(&OwnerComp, 0));

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
	if (!PointsMultipliers.Contains(Point.Category.Name))
	{
		FFormatNamedArguments RespArguments;
		RespArguments.Add(TEXT("category"), FText::FromString(Point.Category.Name.ToString()));
		EDITOR_WARN(
			"DialogSystem",
			FText::Format(
				LOCTEXT("CanNotAddPointsForACategory", "You must defined a points multiplier for {category} "),
				RespArguments)
		);
		return;
	}


	TArray<FNDialogFactorTypeSettings> FactorTypes = PointsMultipliers.FindChecked(Point.Category.Name);
	// TODO refacto: Rather than creates a child factor, creates a data field FArchive to pass what ever you want... ? test
	// + add possibility to  tag a Factor Unit: this way easier to find by tag (identified by family)
	UNDialogFactorUnit* FactorUnit =
		Cast<UNDialogFactorUnit>(FactorsClient->CreateFactorUnit(PointsCollector, UNDialogFactorUnit::StaticClass()));
	FString BaseReason = TEXT("Dialog");
	BaseReason.Append("_");
	BaseReason.Append(Point.Category.Name.ToString());
	FactorUnit->Reason = FName(*BaseReason);
	FactorUnit->Step = Step;
	FactorUnit->Position = Position;
	FactorUnit->CategoryName = Point.Category.Name;
	FactorUnit->InitialPoint = Point.Point;
	FactorUnit->BehaviorTreePathName = BehaviorTreePathName;
	FactorUnit->AIPawnPathName = AIPawnPathName;
	UNOperatorSimpleOperations* Provider = Cast<UNOperatorSimpleOperations>(
		FactorsClient->CreateOperatorProvider(PointsCollector, UNOperatorSimpleOperations::StaticClass())
	);
	Provider->Type = ENFactorSimpleOperation::Add;
	FactorUnit->SetOperatorProvider(Provider);

	NFactorState* State = new NFactorState();
	for (const auto& FactorType : FactorTypes)
	{
		FactorsClient->GetState(FactorType.Factor.Name, *State);
		FactorUnit->FactorUnitValue += Point.Point * State->Compute();
	}

	int32 Key = FactorsClient->AddFactorUnit(PointsCollector, FactorUnit);
	if (Key < 0)
	{
		EDITOR_ERROR(
			"DialogSystem",
			FText::Format(
				LOCTEXT("FactorUnitCanNotBeAdded",
					"the factor unit (reason: {0}) can not be added to the point collector"),
				FText::FromString(BaseReason))
		);
		return;
	}
	FactorUnitKeys.Add(Key);


	// ==================
	// Replace with that:
	// ==================
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
	FString BaseName = TEXT("DialogBlock");
	BaseName.Append("#");
	BaseName.AppendInt(Step);
	FGameplayEffectContextHandle FxContextHandle = PlayerABS->MakeEffectContext();
	FDialogueBlockResult DialogData;
	DialogData.Difficulty = Point.Difficulty;
	DialogData.Position = Position;
	DialogData.CategoryName = Point.Category.Name;
	DialogData.BlockName = FName(BaseName);
	DialogData.InitialPoint = Point.Point;
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

UNDialogFactorUnit* UBTDialogPointsHandler::GetLastResponse()
{
	// TODO change this with new ExtraData
	return Cast<UNDialogFactorUnit>(FactorsClient->GetFactorUnit(PointsCollector, FactorUnitKeys.Last()));
}

float UBTDialogPointsHandler::GetPoints() const
{
	int32 TotalPoints = 0;

	NFactorState* State = new NFactorState();
	FactorsClient->GetState(PointsCollector, *State);

	return FMath::RoundToInt(State->Compute() - FactorsPointsAtStart);
}

int32 UBTDialogPointsHandler::GetDialogPoints(FNResponseCategory Category) const
{
	int32 TotalPoints = 0;
	ensureMsgf(PointsMultipliers.Contains(Category.Name), TEXT("Category asked doesn't apply on dialog system"));
	int32 Points = 0;

	for (const int32& PointsKey : FactorUnitKeys)
	{
		// TODO change this with new ExtraData
		UNDialogFactorUnit* FactorUnit = Cast<UNDialogFactorUnit>(
			FactorsClient->GetFactorUnit(PointsCollector, PointsKey)
		);

		if (FactorUnit->CategoryName == Category.Name)
		{
			Points += FactorUnit->InitialPoint;
		}
	}
	return Points;
}

UNDialogFactorUnit* UBTDialogPointsHandler::GetLastResponseFromStep(const int32 SearchStep)
{
	UNDialogFactorUnit* FactorUnit = nullptr;
	for (int32 Index = FactorUnitKeys.Num() - 1; Index >= 0; --Index)
	{
		// TODO change this with new ExtraData
		FactorUnit = Cast<UNDialogFactorUnit>(FactorsClient->GetFactorUnit(PointsCollector, FactorUnitKeys[Index]));

		if (FactorUnit->Step == SearchStep)
		{
			break;
		}
	}
	return FactorUnit;
}
#undef LOCTEXT_NAMESPACE
