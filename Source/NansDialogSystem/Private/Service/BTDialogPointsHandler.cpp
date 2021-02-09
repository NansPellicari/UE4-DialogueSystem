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

#include "BTDialogueTypes.h"
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
}

void UBTDialogPointsHandler::Initialize(
	TScriptInterface<IBTStepsHandler> _StepsHandler, FString _BehaviorTreePathName, FString _AIPawnPathName)
{
	StepsHandler = _StepsHandler;

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
}

void UBTDialogPointsHandler::AddPoints(FNPoint Point, int32 Position)
{
	int32 Step = IBTStepsHandler::Execute_GetCurrentStep(StepsHandler.GetObject());
	if (!PointsMultipliers.Contains(Point.Category.Name))
	{
		FFormatNamedArguments RespArguments;
		RespArguments.Add(TEXT("category"), FText::FromName(Point.Category.Name));
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
