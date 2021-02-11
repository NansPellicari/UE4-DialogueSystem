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

#include "NansDialogSystem/Public/PlayerDialogComponent.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "Ability/NDSGameplayEffectTypes.h"
#include "Misc/ErrorUtils.h"
#include "Setting/DialogSystemSettings.h"

#define LOCTEXT_NAMESPACE "DialogSystem"


UPlayerDialogComponent::UPlayerDialogComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPlayerDialogComponent::BeginPlay()
{
	Super::BeginPlay();
	if (!IsValid(GetOwner())) return;
	AActor* OwnerActor = Cast<AActor>(GetOwner());
	auto ABSComp = Cast<UAbilitySystemComponent>(
		OwnerActor->GetComponentByClass(UAbilitySystemComponent::StaticClass())
	);
	if (!IsValid(ABSComp))
	{
		EDITOR_ERROR("DialogSystem", LOCTEXT("MissingABS", "Need the UAbilitySystemComponent to work"));
	}
}

#undef LOCTEXT_NAMESPACE
