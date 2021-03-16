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

#include "Ability/PointsEarnedExecCalculation.h"

#include "Ability/NDSAbilitySystemComponent.h"
#include "Ability/NDSGameplayEffectTypes.h"
#include "Ability/AttributeSets/DialogAttributeSets.h"
#include "Setting/DialogSystemSettings.h"

// Declare the attributes to capture and define how we want to capture them from the Source and Target.
struct NPointsEarnedStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(PointsEarned);

	NPointsEarnedStatics()
	{
		// Snapshot happens at time of GESpec creation

		// We're not capturing anything from the Source in this example, but there could be like AttackPower attributes that you might want.

		// Capture optional Damage set on the damage GE as a CalculationModifier under the ExecutionCalculation
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDialogAttributeSets, PointsEarned, Source, true);
	}
};

static const NPointsEarnedStatics& PointsEarnedStatics()
{
	static NPointsEarnedStatics PStatics;
	return PStatics;
}

UPointsEarnedExecCalculation::UPointsEarnedExecCalculation()
{
	RelevantAttributesToCapture.Add(PointsEarnedStatics().PointsEarnedDef);
}

void UPointsEarnedExecCalculation::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	UAbilitySystemComponent* TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();
	UAbilitySystemComponent* SourceAbilitySystemComponent = ExecutionParams.GetSourceAbilitySystemComponent();

	AActor* SourceActor = SourceAbilitySystemComponent ? SourceAbilitySystemComponent->GetAvatarActor() : nullptr;
	AActor* TargetActor = TargetAbilitySystemComponent ? TargetAbilitySystemComponent->GetAvatarActor() : nullptr;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FNDSGameplayEffectContext* ContextHandle = static_cast<FNDSGameplayEffectContext*>(Spec.GetContext().Get());
	checkf(
		ContextHandle != nullptr,
		TEXT("FGameplayEffectContext should be overrided with FNDSGameplayEffectContext to work with %s"),
		ANSI_TO_TCHAR(__FUNCTION__)
	);

	FDialogueResult XtraData = ContextHandle->GetPointData();

	// Gather the tags from the source and target as that can affect which buffs should be used
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float PointsEarned = 0.0f;
	// Capture optional damage value set on the damage GE as a CalculationModifier under the ExecutionCalculation
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		PointsEarnedStatics().PointsEarnedDef,
		EvaluationParameters,
		PointsEarned
	);
	// Add SetByCaller Points Earned if it exists
	PointsEarned += FMath::Max<float>(
		Spec.GetSetByCallerMagnitude(UDialogSystemSettings::Get()->PointMagnitudeTag, false, 1.0f),
		0.0f
	);

	float UnmitigatedPointsEarned = PointsEarned; // Can multiply any points boosters here

	float MitigatedPointsEarned = UnmitigatedPointsEarned; // TODO check if not possible to directly add Mitigator here 

	if (MitigatedPointsEarned > 0.f)
	{
		// Set the Target's points meta attribute
		OutExecutionOutput.AddOutputModifier(
			FGameplayModifierEvaluatedData(
				PointsEarnedStatics().PointsEarnedProperty,
				EGameplayModOp::Additive,
				MitigatedPointsEarned
			)
		);
	}

	// Broadcast damages to Target ASC
	UNDSAbilitySystemComponent* TargetASC = Cast<UNDSAbilitySystemComponent>(TargetAbilitySystemComponent);
	if (TargetASC)
	{
		XtraData.UnmitigatedPointsEarned = UnmitigatedPointsEarned;
		XtraData.MitigatedPointsEarned = MitigatedPointsEarned;
		UNDSAbilitySystemComponent* SourceASC = Cast<UNDSAbilitySystemComponent>(SourceAbilitySystemComponent);
		TargetASC->ReceivePoints(SourceASC, UnmitigatedPointsEarned, MitigatedPointsEarned, XtraData);
	}
}
