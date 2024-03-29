﻿// Copyright 2020-present Nans Pellicari (nans.pellicari@gmail.com).
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
#include "DialogueResult.h"
#include "DialogueSequence.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct NANSDIALOGUESYSTEM_API FDialogueSequence
{
	GENERATED_BODY()

	FDialogueSequence() {}

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "DialogueSequence")
	FName Name;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "DialogueSequence")
	TArray<FDialogueResult> Results;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "DialogueSequence")
	FString Id;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "DialogueSequence")
	FString Owner;

	void AddResult(FDialogueResult Result)
	{
		Results.Add(Result);
	}

	FString ToString() const
	{
		return FString::Printf(TEXT("Name: %s, Owner: %s, Id: %s"), *Name.ToString(), *Owner, *Id);
	}
};
