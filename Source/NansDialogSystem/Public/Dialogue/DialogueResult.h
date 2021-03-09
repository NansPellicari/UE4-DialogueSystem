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
struct FBlockName
{
	GENERATED_BODY()
	FBlockName() {}
	FBlockName(const int32& InId, const FName& InName) : Name(InName), Id(InId) {}
	FName Name = NAME_None;
	int32 Id = 0;

	bool operator==(const FString& RHS) const { return Name.ToString() == RHS || GetNameFromId().ToString() == RHS; }
	bool operator!=(const FString& RHS) const { return !(*this == RHS); }

	FName GetName() const
	{
		return Name;
	}

	FName GetNameFromId() const
	{
		FString Str = FString("Step");
		Str += FString::FromInt(Id);
		return FName(Str);
	}

	FString ToString() const
	{
		return FString::Printf(TEXT("id: %d, name: %s"), Id, *Name.ToString());
	}
};

USTRUCT(BlueprintType)
struct NANSDIALOGSYSTEM_API FDialogueResult
{
	GENERATED_USTRUCT_BODY()

	FDialogueResult() {}

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "DialogueResult")
	int32 Position = -100;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "DialogueResult")
	FBlockName BlockName;
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
