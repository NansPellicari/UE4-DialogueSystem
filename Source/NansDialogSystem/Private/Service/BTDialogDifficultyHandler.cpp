#include "Service/BTDialogDifficultyHandler.h"

#include "BTDialogueTypes.h"
#include "Setting/DialogSystemSettings.h"

UBTDialogDifficultyHandler::UBTDialogDifficultyHandler()
{
	UDialogSystemSettings::GetDifficultyConfigs(Settings);
}

float UBTDialogDifficultyHandler::GetDifficultyLevel(const FBTDialogueResponse& Response)
{
	for (auto& Factor : Factors)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s - factor %s: %f"), ANSI_TO_TCHAR(__FUNCTION__), *Factor.Key.ToString(), Factor.Value);
	}
	if (Response.Alignment == EAlignment::Neutral)
	{
		return Response.DifficultyLevel;
	}

	float Factor = Response.Alignment == EAlignment::CSV ? Factors.FindChecked("Malevolence") : Factors.FindChecked("Benevolence");
	float DifficultyLevel = Response.DifficultyLevel;

	for (const FNDialogDifficultySettings& Setting : Settings)
	{
		if (Setting.Alignment == Response.Alignment && Setting.RangeFrom < Factor && Setting.RangeTo > Factor)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s Setting.Multiplier: %f"), ANSI_TO_TCHAR(__FUNCTION__), Setting.Multiplier);
			DifficultyLevel *= Setting.Multiplier;
			break;
		}
	}
	return DifficultyLevel;
}

void UBTDialogDifficultyHandler::SetFactor(FName Name, float Factor)
{
	Factors.Add(Name, Factor);
}
