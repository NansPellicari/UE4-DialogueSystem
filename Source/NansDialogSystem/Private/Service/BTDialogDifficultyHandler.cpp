#include "Service/BTDialogDifficultyHandler.h"

#include "BTDialogueTypes.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "NansFactorsFactoryCore/Public/FactorStateInRange.h"
#include "NansFactorsFactoryUE4/Public/FactorsFactoryClientAdapter.h"
#include "NansFactorsFactoryUE4/Public/FactorsFactoryGameInstance.h"
#include "NansUE4Utilities/public/Misc/ErrorUtils.h"
#include "Setting/DialogSystemSettings.h"

#define LOCTEXT_NAMESPACE "DialogSystem"

UBTDialogDifficultyHandler::UBTDialogDifficultyHandler()
{
	UDialogSystemSettings::Get()->GetDifficultyConfigs(Settings);
}

void UBTDialogDifficultyHandler::Initialize()
{
	check(GetWorld());

	UGameInstance* GI = UGameplayStatics::GetGameInstance(this);
	checkf(GI->Implements<UNFactorsFactoryGameInstance>(), TEXT("The GameInstance should implements INFactorsFactoryGameInstance"));

	FactorsClient = INFactorsFactoryGameInstance::Execute_GetFactorsFactoryClient(GI);
}

float UBTDialogDifficultyHandler::GetDifficultyLevel(const FBTDialogueResponse& Response)
{
	checkf(FactorsClient != nullptr, TEXT("You should call initialize() before using %s"), ANSI_TO_TCHAR(__FUNCTION__));

#if WITH_EDITOR
	if (bDebug)
	{
		for (auto& Factor : Factors)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s - factor %s: %f"), ANSI_TO_TCHAR(__FUNCTION__), *Factor.Key.ToString(), Factor.Value);
		}
	}
#endif

	TArray<FNDialogFactorTypeSettings> RespFactors = Response.Category.GetFactors((int32) EFactorType::Difficulty);

	if (RespFactors.Num() <= 0)
	{
		return Response.DifficultyLevel;
	}
	const float BaseLevel = Response.DifficultyLevel;
	float DifficultyLevel = BaseLevel;

	for (const auto& RespFactor : RespFactors)
	{
		if (((int32) EFactorType::Difficulty & RespFactor.Type) == 0) continue;
		if (!FactorsClient->HasFactor(RespFactor.Factor.Name))
		{
			EDITOR_WARN("DialogSystem",
				FText::Format(LOCTEXT("FactorInexistant", "You should create the factor {0} to use it here"),
					FText::FromName(RespFactor.Factor.Name)));
			continue;
		}

		checkf(RespFactor.bHasRange,
			TEXT("When a factor (%s) is a difficulty, it has to be a configured with a range lh & rh"),
			*RespFactor.Factor.Name.ToString());

		NFactorStateInRange* State = new NFactorStateInRange(RespFactor.Range.Lh, RespFactor.Range.Rh);
		FactorsClient->GetState(RespFactor.Factor.Name, *State);
		float Factor = State->GetDecimalPercent();

		for (const FNDialogFactorSettings& Setting : Settings)
		{
			if (Setting.Category.Name == Response.Category.Name && Setting.RangeFrom <= Factor && Setting.RangeTo >= Factor)
			{
				// TODO should be great to set rules like: Multiply with base difficulty, mutiply with last computed diff, etc...
				DifficultyLevel *= Setting.Multiplier;
			}
		}
		delete State;
	}
	return DifficultyLevel;
}

#undef LOCTEXT_NAMESPACE
