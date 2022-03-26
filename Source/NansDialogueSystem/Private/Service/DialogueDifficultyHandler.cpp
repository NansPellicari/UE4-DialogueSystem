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

#include "Service/DialogueDifficultyHandler.h"

#include "AbilitySystemComponent.h"
#include "BTDialogueTypes.h"
#include "Kismet/GameplayStatics.h"
#include "NansUE4Utilities/public/Misc/ErrorUtils.h"
#include "Service/DialogueBTHelpers.h"

#define LOCTEXT_NAMESPACE "DialogueSystem"

NDialogueDifficultyHandler::NDialogueDifficultyHandler(const TWeakObjectPtr<UAbilitySystemComponent>& InPlayerAsc,
	bool bInDebug)
{
	UDialogueSystemSettings::Get()->GetDifficultyConfigs(Settings);
	PlayerAsc = InPlayerAsc;
	bDebug = bInDebug;
}

NDialogueDifficultyHandler::~NDialogueDifficultyHandler()
{
	PlayerAsc.Reset();
}

float NDialogueDifficultyHandler::GetDifficultyLevel(const FBTDialogueResponse& Response)
{
	verify(PlayerAsc.IsValid());
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
					"Missing AttributeValue for DialogueDifficultyMagnitudeFactorSettings"),
				PlayerAsc
			);
			bError = true;
		}
		if (!RespFactor.MaxAtttributeValue.IsValid())
		{
			EDITOR_ERROR(
				"DialogComponent",
				LOCTEXT("MissingMaxAttrForDiffFactor",
					"Missing MaxAtttributeValue for DialogueDifficultyMagnitudeFactorSettings"),
				PlayerAsc
			);
			bError = true;
		}
		if (bError)
		{
			continue;
		}
		const float Factor = PlayerAsc->GetNumericAttribute(RespFactor.AttributeValue)
							 / PlayerAsc->GetNumericAttribute(RespFactor.MaxAtttributeValue);

		for (const FNDialogueFactorSettings& Setting : Settings)
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
