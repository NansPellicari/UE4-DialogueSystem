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

#pragma once

#include "CoreMinimal.h"
#include "GameplayTags.h"
#include "DialogueResult.generated.h"

USTRUCT(BlueprintType)
struct NANSDIALOGSYSTEM_API FDialogueResult
{
	GENERATED_USTRUCT_BODY()

	FDialogueResult() {}

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "DialogueResult")
	int32 Position = -100;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "DialogueResult")
	FName BlockName = NAME_None;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "DialogueResult")
	FText Response;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "DialogueResult")
	int32 Difficulty = 0;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "DialogueResult")
	float InitialPoints = -1.f;
	/** This is filled by the PointEarnedExecCalculation() */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "DialogueResult")
	float UnmitigatedPointsEarned = -1.f;
	/** This is filled by the PointEarnedExecCalculation() */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "DialogueResult")
	float MitigatedPointsEarned = -1.f;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "DialogueResult")
	FGameplayTag CategoryName = FGameplayTag::EmptyTag;

	FString ToString() const;
};
