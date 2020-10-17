#include "Service/BTDialogDifficultyHandler.h"

#include "BTDialogueTypes.h"
#include "Setting/DialogSystemSettings.h"

UBTDialogDifficultyHandler::UBTDialogDifficultyHandler()
{
	UDialogSystemSettings::Get()->GetDifficultyConfigs(Settings);
}

float UBTDialogDifficultyHandler::GetDifficultyLevel(const FBTDialogueResponse& Response)
{
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
		if (!Factors.Contains(RespFactor.FactorName)) continue;

		float Factor = *Factors.Find(RespFactor.FactorName);

		for (const FNDialogFactorSettings& Setting : Settings)
		{
			if (Setting.Category.Name == Response.Category.Name && Setting.RangeFrom <= Factor && Setting.RangeTo >= Factor)
			{
				// TODO should be great to set rules like: Multiply with base difficulty, mutiply with last computed diff, etc...
				DifficultyLevel *= Setting.Multiplier;
			}
		}
	}
	return DifficultyLevel;
}

void UBTDialogDifficultyHandler::SetFactor(FName Name, float Factor)
{
	Factors.Add(Name, Factor);
}
