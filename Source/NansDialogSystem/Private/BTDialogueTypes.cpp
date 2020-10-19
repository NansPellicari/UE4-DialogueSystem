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
	if ((int32) Type == 0) return Config.Factors;

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
	ensureMsgf(Name != NAME_None, TEXT("You should set a name before getting the config of a FNResponseCategory."));
	static TMap<FName, FNDialogResponseCategorySettings> SettingsByName;
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
