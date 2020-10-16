// Copyright 2020-present Nans Pellicari (nans.pellicari@gmail.com).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "Attribute/ResponseCategory.h"

#include "Setting/DialogSystemSettings.h"

FLinearColor FNResponseCategory::GetColor() const
{
	FNDialogResponseCategorySettings* Config = FNResponseCategory::GetConfig(Name);

	return Config->Color;
}

FName FNResponseCategory::GetFactorName() const
{
	FNDialogResponseCategorySettings* Config = FNResponseCategory::GetConfig(Name);

	return Config->FactorName;
}

FNDialogResponseCategorySettings* FNResponseCategory::GetConfig(const FName& Name)
{
	TArray<FNDialogResponseCategorySettings> Settings;
	UDialogSystemSettings::GetResponseCategoryConfigs(Settings);
	for (auto& Setting : Settings)
	{
		if (Setting.Name == Name)
		{
			return &Setting;
		}
	}
	return new FNDialogResponseCategorySettings();
}
