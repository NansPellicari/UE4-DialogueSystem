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

FLinearColor FNDialogueCategory::GetColor() const
{
	const FNDialogueCategorySettings& Config = GetConfig();

	return Config.Color;
}

TArray<FNDialogFactorTypeSettings> FNDialogueCategory::GetFactors(const int32 Type) const
{
	const FNDialogueCategorySettings& Config = GetConfig();
	if (static_cast<int32>(Type) == 0) return Config.Factors;

	TArray<FNDialogFactorTypeSettings> OutFactors;
	for (const auto& Factor : Config.Factors)
	{
		if (Type & Factor.Type)
		{
			OutFactors.Add(Factor);
		}
	}
	return OutFactors;
}

const FNDialogueCategorySettings& FNDialogueCategory::GetConfig() const
{
	ensureMsgf(Name.IsValid(), TEXT("You should set a name before getting the config of a FNDialogueCategory."));
	static TMap<FGameplayTag, FNDialogueCategorySettings> SettingsByName;
	FNDialogueCategorySettings& OutSetting = *FNDialogueCategorySettings::CreateNullInstance();
	if (SettingsByName.Contains(Name))
	{
		OutSetting = SettingsByName.FindRef(Name);
		return OutSetting;
	}
	TArray<FNDialogueCategorySettings> Settings;
	UDialogSystemSettings::Get()->GetDialogueCategoryConfigs(Settings);
	for (auto& Setting : Settings)
	{
		if (Setting.Name == Name)
		{
			SettingsByName.Add(Name, Setting);
			OutSetting = SettingsByName.FindRef(Name);
			break;
		}
	}
	return OutSetting;
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
