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

#include "Service/BTDialogDifficultyHandler.h"


#include "BTDialogueTypes.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "NansUE4Utilities/public/Misc/ErrorUtils.h"
#include "Service/DialogBTHelpers.h"
#include "Setting/DialogSystemSettings.h"

#define LOCTEXT_NAMESPACE "DialogSystem"

UBTDialogDifficultyHandler::UBTDialogDifficultyHandler()
{
	UDialogSystemSettings::Get()->GetDifficultyConfigs(Settings);
}

void UBTDialogDifficultyHandler::Initialize(UBehaviorTreeComponent& OwnerComp)
{
	check(GetWorld());
	PlayerGASC = NDialogBTHelpers::GetGASC(OwnerComp);
}

float UBTDialogDifficultyHandler::GetDifficultyLevel(const FBTDialogueResponse& Response)
{
	TArray<FNDialogueDifficultyMagnitudeFactorSettings> RespFactors = FNDialogueCategory::GetDifficulties(
		Response.Category
	);

	if (RespFactors.Num() <= 0)
	{
		return Response.Difficulty;
	}
	const float BaseLevel = Response.Difficulty;
	float DifficultyLevel = BaseLevel;

	for (const auto& RespFactor : RespFactors)
	{
		bool bError = false;
		if (!RespFactor.AttributeValue.IsValid())
		{
			EDITOR_ERROR(
				"DialogComponent",
				LOCTEXT("MissingAttrForDiffFactor",
					"Missing AttributeValue for DialogueDifficultyMagnitudeFactorSettings")
			);
			bError = true;
		}
		if (!RespFactor.MaxAtttributeValue.IsValid())
		{
			EDITOR_ERROR(
				"DialogComponent",
				LOCTEXT("MissingMaxAttrForDiffFactor",
					"Missing MaxAtttributeValue for DialogueDifficultyMagnitudeFactorSettings")
			);
			bError = true;
		}
		if (bError)
		{
			continue;
		}
		const float Factor = PlayerGASC->GetNumericAttribute(RespFactor.AttributeValue)
							 / PlayerGASC->GetNumericAttribute(RespFactor.MaxAtttributeValue);

		for (const FNDialogFactorSettings& Setting : Settings)
		{
			if (Setting.Category.Name == Response.Category.Name && Setting.RangeFrom <= Factor && Setting.RangeTo >=
				Factor)
			{
				// TODO should be great to set rules like: Multiply with base difficulty, mutiply with last computed diff, etc...
				DifficultyLevel *= Setting.Multiplier;
			}
		}
	}
	return DifficultyLevel;
}

#undef LOCTEXT_NAMESPACE
