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

#include "BTDialogueTypes.h"

#include "NansCoreHelpers/Public/Misc/NansAssertionMacros.h"
#include "Setting/DialogSystemSettings.h"

FLinearColor FNResponseCategory::GetColor() const
{
	const FNDialogResponseCategorySettings& Config = GetConfig();

	return Config.Color;
}

TArray<FNDialogFactorTypeSettings> FNResponseCategory::GetFactors(const int32 Type) const
{
	const FNDialogResponseCategorySettings& Config = GetConfig();
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

const FNDialogResponseCategorySettings& FNResponseCategory::GetConfig() const
{
	ensureMsgf(Name.IsValid(), TEXT("You should set a name before getting the config of a FNResponseCategory."));
	static TMap<FGameplayTag, FNDialogResponseCategorySettings> SettingsByName;
	FNDialogResponseCategorySettings& OutSetting = *FNDialogResponseCategorySettings::CreateNullInstance();
	if (SettingsByName.Contains(Name))
	{
		OutSetting = SettingsByName.FindRef(Name);
		return OutSetting;
	}
	TArray<FNDialogResponseCategorySettings> Settings;
	UDialogSystemSettings::Get()->GetResponseCategoryConfigs(Settings);
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
