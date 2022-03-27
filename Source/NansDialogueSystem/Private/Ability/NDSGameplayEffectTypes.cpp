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

#include "Ability/NDSGameplayEffectTypes.h"

void FNDSGameplayEffectContext::AddPointsData(const FDialogueResult& Data)
{
	PointsData = Data;
}

FDialogueResult FNDSGameplayEffectContext::GetPointData() const
{
	return PointsData;
}

void FNDSGameplayEffectContext::AddExtraData(const FGameplayTag& Tag, const FString& Data)
{
	ExtraData.Add(Tag, Data);
}

void FNDSGameplayEffectContext::AddExtraData(const TMap<FGameplayTag, FString>& NewData)
{
	ExtraData.Append(NewData);
}

TMap<FGameplayTag, FString> FNDSGameplayEffectContext::GetExtraData() const
{
	return ExtraData;
}

bool FNDSGameplayEffectContext::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), ANSI_TO_TCHAR(__FUNCTION__));
	// TODO serialize the ExtraData TMap 
	return Super::NetSerialize(Ar, Map, bOutSuccess);
}
