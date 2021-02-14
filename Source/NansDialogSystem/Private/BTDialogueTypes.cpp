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

#include "BTDialogueTypes.h"


#include "Misc/ErrorUtils.h"
#include "Setting/DialogSystemSettings.h"

#define LOCTEXT_NAMESPACE "DialogSystem"

FLinearColor FNDialogueCategory::GetColorFromSettings(const FNDialogueCategory& DialogueCategory)
{
	static TMap<FGameplayTag, FLinearColor> ColorsCached;
	FLinearColor Color;

	if (!ColorsCached.Contains(DialogueCategory.Name))
	{
		TArray<FNDialogueCategorySettings> Settings;
		UDialogSystemSettings::Get()->GetDialogueCategoryConfigs(Settings);
		for (auto Set : Settings)
		{
			if (Set.Name == DialogueCategory.Name)
			{
				ColorsCached.Add(Set.Name, Set.Color);
				break;
			}
		}
	}
	if (!ColorsCached.Contains(DialogueCategory.Name)) return {};

	return ColorsCached.FindRef(DialogueCategory.Name);
}

TArray<FNDialogueDifficultyMagnitudeFactorSettings> FNDialogueCategory::GetDifficulties(const FNDialogueCategory& DialogueCategory)
{
	const FGameplayTag Name = DialogueCategory.Name;
	ensureMsgf(Name.IsValid(), TEXT("You should set a name before getting the config of a FNDialogueCategory."));
	static TMap<FGameplayTag, FNDialogueDifficultyMagnitudeSettings> SettingsByName;

	if (!SettingsByName.Contains(Name))
	{
		TArray<FNDialogueDifficultyMagnitudeSettings> Settings;
		UDialogSystemSettings::Get()->GetDialogueDifficultyMagnitudeConfigs(Settings);
		for (auto& Setting : Settings)
		{
			if (Setting.Category.Name == Name)
			{
				SettingsByName.Add(Name, Setting);
				break;
			}
		}
	}
	if (!SettingsByName.Contains(Name)) return {};

	return SettingsByName.FindRef(DialogueCategory.Name).Attributes;
}


TSubclassOf<UGameplayEffect> FBTDialogueResponse::GetSpawnableEffectOnEarned() const
{
	auto GEffect = EffectOnEarned;
	if (!IsValid(GEffect))
	{
		for (const auto& CatSet : UDialogSystemSettings::Get()->ResponseCategorySettings)
		{
			if (CatSet.Name == Category.Name)
			{
				GEffect = CatSet.DefaultEffect;
				break;
			}
		}
		if (!IsValid(GEffect))
		{
			return nullptr;
		}
	}
	return GEffect;
}

#undef LOCTEXT_NAMESPACE
