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

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "NDSAbilitySystemGlobals.generated.h"

/**
 * Thanks to https://github.com/tranek/GASShooter
 * Child class of UAbilitySystemGlobals.
 * Do not try to get a reference to this or call into it during constructors of other UObjects. It will crash in packaged games.
 */
UCLASS()
class NANSDIALOGSYSTEM_API UNDSAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()
public:
	UNDSAbilitySystemGlobals();

	static UNDSAbilitySystemGlobals& GSGet()
	{
		return dynamic_cast<UNDSAbilitySystemGlobals&>(Get());
	}

	/** Should allocate a project specific GameplayEffectContext struct. Caller is responsible for deallocation */
	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
};
