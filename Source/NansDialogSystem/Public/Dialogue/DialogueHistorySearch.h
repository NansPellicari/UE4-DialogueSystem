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

#include "BTDialogueTypes.h"
#include "NansCommon/Public/Service/NansComparatorHelpers.h"
#include "NansUE4Utilities/public/Misc/TextLibrary.h"
#include "Service/NansComparator.h"

#include "DialogueHistorySearch.generated.h"

USTRUCT(BlueprintType)
struct NANSDIALOGSYSTEM_API FNCurrentOrAll
{
	GENERATED_BODY()

	FNCurrentOrAll() {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta=(EditCondition="!bIsCurrent"))
	bool bIsAll = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta=(EditCondition="!bIsAll"))
	bool bIsCurrent = true;

	FString ToString() const
	{
		return bIsAll ? TEXT("All") : TEXT("Current");
	}
};

USTRUCT(BlueprintType)
struct NANSDIALOGSYSTEM_API FNFullValue
{
	GENERATED_BODY()

	FNFullValue() { }
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta=(EditCondition="!bIsAll && !bIsCurrent"))
	FString Value;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta=(EditCondition="!bIsCurrent"))
	bool bIsAll = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta=(EditCondition="!bIsAll"))
	bool bIsCurrent = true;

	FString ToString() const
	{
		return bIsAll ? TEXT("All") : bIsCurrent ? TEXT("Current") : Value;
	}
};

USTRUCT(BlueprintType)
struct NANSDIALOGSYSTEM_API FNAllOrName
{
	GENERATED_BODY()

	FNAllOrName() { }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta=(EditCondition="!bIsAll"))
	FString Value;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta=(EditCondition="!Value"))
	bool bIsAll = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta=(EditCondition="!bIsAll"))
	bool bLastOnly = false;

	FString ToString() const
	{
		FString Str = bIsAll ? TEXT("All") : Value;
		if (bLastOnly)
		{
			Str = TEXT("(last Only) ") + Str;
		}
		return Str;
	}

	void SetValue(const FString& InValue)
	{
		bIsAll = false;
		Value = InValue;
	}
};

UENUM(BlueprintType)
enum class ENPropertyValue : uint8
{
	InitialPoints,
	CategoryName,
	PointsEarned,
	SentencePosition,
	BlockName,
	Difficulty,
	IsDone
};

/**
 * 
 */
USTRUCT(BlueprintType)
struct NANSDIALOGSYSTEM_API FNDialogueHistorySearch
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FNCurrentOrAll SaveName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FNFullValue LevelName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FNFullValue SequenceName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FNFullValue OwnerName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FNAllOrName DialogName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ENPropertyValue PropertyName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta=(EditCondition="PropertyName != ENPropertyValue::IsDone"))
	ENansConditionComparator Operator;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta=(EditCondition=
		"PropertyName == ENPropertyValue::Difficulty || PropertyName == ENPropertyValue::SentencePosition"))
	int32 IntValue = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta=(EditCondition="PropertyName == ENPropertyValue::BlockName"))
	FName NameValue = NAME_None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta=(EditCondition=
		"PropertyName == ENPropertyValue::InitialPoints || PropertyName == ENPropertyValue::PointsEarned"))
	float FloatValue = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta=(EditCondition="PropertyName == ENPropertyValue::CategoryName"))
	FNDialogueCategory CategoryValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bInversed = false;

	bool IsInt() const
	{
		return PropertyName == ENPropertyValue::Difficulty || PropertyName == ENPropertyValue::SentencePosition;
	}

	bool IsName() const
	{
		return PropertyName == ENPropertyValue::BlockName;
	}

	bool IsFloat() const
	{
		return PropertyName == ENPropertyValue::InitialPoints || PropertyName == ENPropertyValue::PointsEarned;
	}

	bool IsCategory() const
	{
		return PropertyName == ENPropertyValue::CategoryName;
	}

	FString ValueToString() const
	{
		if (IsInt())
		{
			return FString::FromInt(IntValue);
		}
		if (IsName())
		{
			return NameValue.ToString();
		}
		if (IsFloat())
		{
			return FString::SanitizeFloat(FloatValue);
		}
		return CategoryValue.Name.ToString();
	}

	FString SearchToString() const
	{
		FString Str;
		if (!SaveName.bIsCurrent)
		{
			Str += FString::Printf(
				TEXT("Save: %s, "),
				*SaveName.ToString()
			);
		}
		if (!LevelName.bIsCurrent)
		{
			Str += FString::Printf(
				TEXT("Level: %s, "),
				*LevelName.ToString()
			);
		}
		if (!SequenceName.bIsCurrent)
		{
			Str += FString::Printf(
				TEXT("Sequence: %s, "),
				*SequenceName.ToString()
			);
		}

		if (!OwnerName.bIsCurrent)
		{
			Str += FString::Printf(
				TEXT("Owner: %s, "),
				*OwnerName.ToString()
			);
		}
		if (!OwnerName.bIsAll)
		{
			Str += FString::Printf(
				TEXT("Dialog: %s, "),
				*DialogName.ToString()
			);
		}
		Str += ENUM_TO_STRING(ENPropertyValue, PropertyName);

		if (bInversed)
		{
			Str += FString::Printf(
				TEXT(" (Inversed)"),
				*DialogName.ToString()
			);
		}
		return Str;
	}

	FString ToString() const
	{
		return FString::Printf(
			TEXT("%s %s %s"),
			*SearchToString(),
			(PropertyName != ENPropertyValue::IsDone) ? *UNansComparator::ComparatorToString(Operator) : TEXT(""),
			(PropertyName != ENPropertyValue::IsDone) ? *ValueToString() : TEXT("")
		);
	}
};
