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
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "DialogAttributeSets.generated.h"


#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
* 
*/
UCLASS()
class NANSDIALOGSYSTEM_API UDialogAttributeSets : public UAttributeSet
{
	GENERATED_BODY()
public:
	UDialogAttributeSets();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/**
	 * PointsEarned is a meta attribute used by the PointsEarnedExecution to calculate final point.
	 * Temporary value that only exists on the Server. Not replicated.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "DialogPoints", meta = (HideFromLevelInfos))
	FGameplayAttributeData PointsEarned;
	ATTRIBUTE_ACCESSORS(UDialogAttributeSets, PointsEarned);
};
