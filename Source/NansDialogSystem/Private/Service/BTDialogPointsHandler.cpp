#include "Service/BTDialogPointsHandler.h"

#include "BTDialogueTypes.h"
#include "Engine/GameInstance.h"
#include "Factor/DialogFactorUnit.h"
#include "Kismet/GameplayStatics.h"
#include "NansBehaviorSteps/Public/BTStepsHandler.h"
#include "NansCoreHelpers/Public/Misc/NansAssertionMacros.h"
#include "NansFactorsFactoryCore/Public/FactorState.h"
#include "NansFactorsFactoryUE4/Public/FactorUnit/FactorUnitAdapter.h"
#include "NansFactorsFactoryUE4/Public/FactorUnit/UnrealFactorUnitProxy.h"
#include "NansFactorsFactoryUE4/Public/FactorsFactoryClientAdapter.h"
#include "NansFactorsFactoryUE4/Public/FactorsFactoryGameInstance.h"
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
	checkf(GI->Implements<UNFactorsFactoryGameInstance>(), TEXT("The GameInstance should implements INFactorsFactoryGameInstance"));

	FactorsClient = INFactorsFactoryGameInstance::Execute_GetFactorsFactoryClient(GI);

	NFactorState* State = new NFactorState();
	FactorsClient->GetState(PointsCollector, *State);
	FactorsPointsAtStart = State->Compute();
}

void UBTDialogPointsHandler::AddPoints(FPoint Point, int32 Position)
{
	int32 Step = IBTStepsHandler::Execute_GetCurrentStep(StepsHandler.GetObject());
	if (!PointsMultipliers.Contains(Point.Category.Name))
	{
		FFormatNamedArguments RespArguments;
		RespArguments.Add(TEXT("category"), FText::FromName(Point.Category.Name));
		EDITOR_WARN("DialogSystem",
			FText::Format(
				LOCTEXT("CanNotAddPointsForACategory", "You must defined a points multiplier for {category} "), RespArguments));
		return;
	}
	TArray<FNDialogFactorTypeSettings> FactorTypes = PointsMultipliers.FindChecked(Point.Category.Name);
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
		FactorsClient->CreateOperatorProvider(PointsCollector, UNOperatorSimpleOperations::StaticClass()));
	Provider->Type = ENFactorSimpleOperation::Add;
	FactorUnit->SetOperatorProvider(Provider);

	NFactorState* State = new NFactorState();
	for (const auto& FactorType : FactorTypes)
	{
		FactorsClient->GetState(FactorType.Factor.Name, *State);
		FactorUnit->FactorUnitValue += Point.Point * State->Compute();
	}

	int32 Key = FactorsClient->AddFactorUnit(PointsCollector, MakeShareable(new NUnrealFactorUnitProxy(FactorUnit)));
	if (Key < 0)
	{
		EDITOR_ERROR("DialogSystem",
			FText::Format(
				LOCTEXT("FactorUnitCanNotBeAdded", "the factor unit (reason: {0}) can not be added to the point collector"),
				FText::FromString(BaseReason)));
		return;
	}
	FactorUnitKeys.Add(Key);
}

UNDialogFactorUnit* UBTDialogPointsHandler::GetLastResponse()
{
	NUnrealFactorUnitProxy* FactorUnitProxy =
		dynamic_cast<NUnrealFactorUnitProxy*>(FactorsClient->GetFactorUnit(PointsCollector, FactorUnitKeys.Last()).Get());
	return Cast<UNDialogFactorUnit>(FactorUnitProxy->GetUnrealObject());
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
		NUnrealFactorUnitProxy* FactorUnitProxy =
			dynamic_cast<NUnrealFactorUnitProxy*>(FactorsClient->GetFactorUnit(PointsCollector, PointsKey).Get());
		UNDialogFactorUnit* FactorUnit = FactorUnit = Cast<UNDialogFactorUnit>(FactorUnitProxy->GetUnrealObject());

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
		NUnrealFactorUnitProxy* FactorUnitProxy =
			dynamic_cast<NUnrealFactorUnitProxy*>(FactorsClient->GetFactorUnit(PointsCollector, FactorUnitKeys[Index]).Get());
		FactorUnit = Cast<UNDialogFactorUnit>(FactorUnitProxy->GetUnrealObject());

		if (FactorUnit->Step == SearchStep)
		{
			break;
		}
	}
	return FactorUnit;
}
#undef LOCTEXT_NAMESPACE
