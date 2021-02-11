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

#pragma once

#include "BTDialogueTypes.h"
#include "GameplayEffectTypes.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "NDSGameplayEffectTypes.generated.h"

/**
 * Thanks to https://github.com/tranek/GASShooter
 * Data structure that stores an instigator and related data, such as positions and targets
 * Games can subclass this structure and add game-specific information
 * It is passed throughout effect execution so it is a great place to track transient information about an execution
 */
USTRUCT()
struct NANSDIALOGSYSTEM_API FNDSGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

	virtual void AddPointsData(FDialogueBlockResult Data);
	virtual FDialogueBlockResult GetPointData() const;
	virtual void AddExtraData(FGameplayTag Tag, const FString& Data);
	virtual void AddExtraData(TMap<FGameplayTag, FString> NewData);
	virtual TMap<FGameplayTag, FString> GetExtraData() const;

	/**
	* Functions that subclasses of FGameplayEffectContext need to override
	*/
	virtual UScriptStruct* GetScriptStruct() const override
	{
		return StaticStruct();
	}

	virtual FNDSGameplayEffectContext* Duplicate() const override
	{
		FNDSGameplayEffectContext* NewContext = new FNDSGameplayEffectContext();
		*NewContext = *this;
		NewContext->AddActors(Actors);
		if (GetHitResult())
		{
			// Does a deep copy of the hit result
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		if (ExtraData.Num() > 0 && NewContext->ExtraData.Num() <= 0)
		{
			NewContext->AddExtraData(ExtraData);
		}
		NewContext->PointsData = PointsData;
		return NewContext;
	}

	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;
protected:
	UPROPERTY()
	TMap<FGameplayTag, FString> ExtraData;
	UPROPERTY()
	FDialogueBlockResult PointsData;
};


template <>
struct TStructOpsTypeTraits<FNDSGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FNDSGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true // Necessary so that TSharedPtr<FHitResult> Data is copied around
	};
};
