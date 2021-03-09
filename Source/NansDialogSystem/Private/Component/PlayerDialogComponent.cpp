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

#include "NansDialogSystem/Public/Component/PlayerDialogComponent.h"


#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "NansDialogSystemLog.h"
#include "NDSFunctionLibrary.h"
#include "PointSystemHelpers.h"
#include "Ability/NDSGameplayEffectTypes.h"
#include "Dialogue/DialogueHistorySearch.h"
#include "Kismet/GameplayStatics.h"
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
	ABSComp = Cast<UAbilitySystemComponent>(
		OwnerActor->GetComponentByClass(UAbilitySystemComponent::StaticClass())
	);
	if (!IsValid(ABSComp))
	{
		EDITOR_ERROR("DialogSystem", LOCTEXT("MissingABS", "Need the UAbilitySystemComponent to work"));
	}
}

void UPlayerDialogComponent::AddSequence(const FDialogueSequence& Sequence)
{
	auto CurrentLevelName = UGameplayStatics::GetCurrentLevelName(this);
	// TODO improve test, only test on LevelName is not sufficient...
	if (DialogueHistories.Num() <= 0 || CurrentLevelName != DialogueHistories.Last().LevelName)
	{
		FDialogueHistory History;
		History.LevelName = CurrentLevelName;
		// TODO maybe a timestamp instead?
		// History.LevelId = UGameplayStatics::GetCurrentLevelName(
		CurrentHistoryIndex = DialogueHistories.Add(History);
	}
	CurrentSequenceIndex = DialogueHistories.Last().Sequences.Add(Sequence);
}

void UPlayerDialogComponent::AddResponse(const FDialogueResult& Result)
{
	if (DialogueHistories.Last().Sequences.Num() <= 0)
	{
		EDITOR_ERROR(
			"DialogSystem",
			LOCTEXT("NoSequencesInDialogHistory",
				"You should first create a new DialogueSequence before saving a Result")
		);
		return;
	}
	DialogueHistories.Last().Sequences.Last().Results.Add(Result);
}

bool UPlayerDialogComponent::HasResults(const FNDialogueHistorySearch& Search)
{
	const TArray<FDialogueResult> Results = SearchResults(Search);
	return Search.bInversed ? Results.Num() <= 0 : Results.Num() > 0;
}

bool UPlayerDialogComponent::HasResultsOnSearches(const TArray<FNDialogueHistorySearch> Searches,
	TArray<FNansConditionOperator> ConditionsOperators)
{
	const bool HasConditionsOperator = ConditionsOperators.Num() > 0;
	TMap<FString, BoolStruct*> ConditionsResults;
	int32 Idx = 0;
	if (Searches.Num() > 0)
	{
		for (auto& Search : Searches)
		{
			FString SearchKey = UNansComparator::BuildKeyFromIndex(Idx);
			const bool bResult = HasResults(Search);
			if (bDebugSearch)
			{
				UE_LOG(
					LogDialogSystem,
					Display,
					TEXT("%s search: %s - results: %i"),
					*SearchKey,
					*Search.ToString(),
					bResult
				);
			}
			ConditionsResults.Add(SearchKey, new BoolStruct(bResult));
			++Idx;
			if (HasConditionsOperator == false && bResult == false)
			{
				break;
			}
		}
	}
	if (ConditionsResults.Num() <= 0)
	{
		return false;
	}
	if (HasConditionsOperator == false)
	{
		const FString Key = UNansComparator::BuildKeyFromIndex(ConditionsResults.Num() - 1);
		return ConditionsResults.FindRef(Key)->value;
	}

	return UNansComparator::EvaluateOperators(ConditionsOperators, ConditionsResults, bDebugSearch);
}

int32 UPlayerDialogComponent::GetDialogPoints(FNDialogueCategory Category) const
{
	int32 Points = 0;
	FNDialogueHistorySearch Search;
	Search.DialogName.bIsAll = true;
	Search.PropertyName = ENPropertyValue::IsDone;
	const TArray<FDialogueResult> Results = SearchResults(Search);

	for (const FDialogueResult& Result : Results)
	{
		if (Result.CategoryName == Category.Name)
		{
			Points += Result.InitialPoints;
		}
	}
	return Points;
}

void UPlayerDialogComponent::AddPoints(FNPoint Point, int32 Position, FBTStep Step)
{
	TSubclassOf<UGameplayEffect> GEffect = Point.EffectOnEarned;
	if (!IsValid(GEffect))
	{
		EDITOR_ERROR(
			"DialogSystem",
			FText::Format(
				LOCTEXT("EffectOnEarnedMissing",
					"No effect on earned is set for point type {0}"),
				FText::FromString(Point.Category.Name.ToString()))
		);
		return;
	}
	FGameplayEffectContextHandle FxContextHandle = ABSComp->MakeEffectContext();
	FDialogueResult DialogData;
	DialogData.Difficulty = Point.Difficulty;
	DialogData.Position = Position;
	DialogData.CategoryName = Point.Category.Name;
	DialogData.BlockName = FBlockName(Step.Id, Step.GetLabel());
	DialogData.InitialPoints = Point.Point;
	DialogData.Response = Point.Response;
	UNDSFunctionLibrary::EffectContextAddPointsData(FxContextHandle, DialogData);
	FGameplayEffectSpecHandle SpecHandle = ABSComp->MakeOutgoingSpec(GEffect, Point.Difficulty, FxContextHandle);
	UAbilitySystemBlueprintLibrary::AddAssetTag(SpecHandle, Point.Category.Name);
	UAbilitySystemBlueprintLibrary::AddAssetTag(SpecHandle, UDialogSystemSettings::Get()->TagToIdentifyDialogEffect);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle,
		UDialogSystemSettings::Get()->PointMagnitudeTag,
		Point.Point
	);
	ABSComp->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

TArray<FDialogueResult> UPlayerDialogComponent::SearchResults(const FNDialogueHistorySearch& Search) const
{
	TArray<FDialogueResult> Results;
	const FDialogueHistory* CurrentHistory = CurrentHistoryIndex > -1
												 ? &DialogueHistories[CurrentHistoryIndex]
												 : &DialogueHistories.Last();
	const FDialogueSequence* CurrentSequence = CurrentSequenceIndex > -1
												   ? &CurrentHistory->Sequences[CurrentSequenceIndex]
												   : &CurrentHistory->Sequences.Last();
	FString Decals = "";
	if (bDebugSearch)
	{
		UE_LOG(LogDialogSystem, Display, TEXT(">>>>> START DEBUG: Search results >>>>>"));
		UE_LOG(LogDialogSystem, Display, TEXT("Search: %s"), *Search.ToString());
	}
	Decals += "  ";
	for (const FDialogueHistory& History : DialogueHistories)
	{
		if (bDebugSearch) UE_LOG(LogDialogSystem, Display, TEXT("%s|History| %s"), *Decals, *History.ToString());
		if (
			!Search.LevelName.bIsAll &&
			!Search.LevelName.bIsCurrent &&
			Search.LevelName.Value != History.LevelName
		)
		{
			continue;
		}
		// TODO see how to manage that
		// if (
		// 	Search.LevelName.bIsCurrent &&
		// 	History.LevelId != CurrentLevelId
		// )
		// {
		// 	continue;
		// }

		const int32 LastSequenceIdx = History.Sequences.Num() - 1;
		int32 SequenceIdx = -1;
		Decals += "  ";
		for (const FDialogueSequence& Sequence : History.Sequences)
		{
			SequenceIdx++;
			if (bDebugSearch)
			{
				UE_LOG(LogDialogSystem, Display, TEXT("%s|Sequence|"), *Decals);
				UE_LOG(LogDialogSystem, Display, TEXT("%s  - %s"), *Decals, *Sequence.ToString());
				UE_LOG(
					LogDialogSystem,
					Display,
					TEXT("%s  - Index: %i, Last Index: %i"),
					*Decals,
					SequenceIdx,
					LastSequenceIdx
				)
			}
			if (
				!Search.SequenceName.bIsAll &&
				!Search.SequenceName.bIsCurrent &&
				Search.SequenceName.Value != Sequence.Name.ToString()
			)
			{
				continue;
			}
			if (
				Search.SequenceName.bIsCurrent &&
				Sequence.Name != CurrentSequence->Name
			)
			{
				if (bDebugSearch) UE_LOG(LogDialogSystem, Display, TEXT("%s>has not the same name"), *Decals);
				continue;
			}
			if (
				Search.SequenceName.bIsCurrent &&
				Sequence.Name == CurrentSequence->Name &&
				LastSequenceIdx > SequenceIdx
			)
			{
				if (bDebugSearch) UE_LOG(LogDialogSystem, Display, TEXT("%s>Is not the last"), *Decals);
				continue;
			}
			if (
				!Search.OwnerName.bIsAll &&
				!Search.OwnerName.bIsCurrent &&
				Search.OwnerName.Value != Sequence.Owner
			)
			{
				if (bDebugSearch) UE_LOG(LogDialogSystem, Display, TEXT("%s>Do not belong to the same user"), *Decals);
				continue;
			}
			if (
				Search.OwnerName.bIsCurrent &&
				Sequence.Owner != CurrentSequence->Owner
			)
			{
				if (bDebugSearch) UE_LOG(LogDialogSystem, Display, TEXT("%s>Do not belong to the same user"), *Decals);
				continue;
			}

			Decals += "  ";
			const int32 LastBlockIdx = Sequence.Results.Num() - 1;
			int32 BlockIdx = -1;
			for (const FDialogueResult& Block : Sequence.Results)
			{
				++BlockIdx;
				if (bDebugSearch)
				{
					UE_LOG(LogDialogSystem, Display, TEXT("%s|Block|"), *Decals);
					UE_LOG(LogDialogSystem, Display, TEXT("%s  - %s"), *Decals, *Block.ToString());
					UE_LOG(
						LogDialogSystem,
						Display,
						TEXT("%s  - Index: %i, Last Index: %i"),
						*Decals,
						BlockIdx,
						LastBlockIdx
					)
				}

				if (
					!Search.DialogName.bIsAll &&
					!Search.DialogName.Value.IsEmpty() &&
					Block.BlockName != Search.DialogName.Value
				)
				{
					if (bDebugSearch) UE_LOG(LogDialogSystem, Display, TEXT("%s>Name is not compatible"), *Decals);
					continue;
				}
				if (Search.DialogName.bLastOnly && LastBlockIdx > BlockIdx)
				{
					if (bDebugSearch) UE_LOG(LogDialogSystem, Display, TEXT("%s>Is not the last one"), *Decals);
					continue;
				}
				bool bSuccess = false;
				if (!bSuccess && Search.PropertyName == ENPropertyValue::IsDone)
				{
					bSuccess = true;
				}
				if (!bSuccess && Search.IsCategory())
				{
					bSuccess = UNansComparator::EvaluateComparator<FString>(
						Search.Operator,
						Block.CategoryName.ToString(),
						Search.CategoryValue.Name.ToString()
					);
					if (bDebugSearch) UE_LOG(
						LogDialogSystem,
						Display,
						TEXT("%s>Is %s category: %i"),
						*Decals,
						*ENUM_TO_STRING(ENansConditionComparator, Search.Operator),
						bSuccess
					)
				}
				if (!bSuccess && Search.IsName())
				{
					bSuccess = UNansComparator::EvaluateComparator<FString>(
						Search.Operator,
						Block.BlockName.GetNameFromId().ToString(),
						Search.NameValue.ToString()
					);
					bSuccess = bSuccess || UNansComparator::EvaluateComparator<FString>(
								   Search.Operator,
								   Block.BlockName.GetName().ToString(),
								   Search.NameValue.ToString()
							   );
					if (bDebugSearch) UE_LOG(
						LogDialogSystem,
						Display,
						TEXT("%s>Is %s name: %i"),
						*Decals,
						*ENUM_TO_STRING(ENansConditionComparator, Search.Operator),
						bSuccess
					)
				}
				if (!bSuccess && Search.IsFloat())
				{
					bSuccess = UNansComparator::EvaluateComparator<float>(
						Search.Operator,
						Search.PropertyName == ENPropertyValue::InitialPoints
							? Block.InitialPoints
							: Block.MitigatedPointsEarned,
						Search.FloatValue
					);
					if (bDebugSearch) UE_LOG(
						LogDialogSystem,
						Display,
						TEXT("%s>Is %s %s: %i"),
						*Decals,
						*ENUM_TO_STRING(ENansConditionComparator, Search.Operator),
						Search.PropertyName == ENPropertyValue::InitialPoints
						? TEXT("InitialPoints")
						: TEXT("MitigatedPointsEarned"),
						bSuccess
					)
				}
				if (!bSuccess && Search.IsInt())
				{
					bSuccess = UNansComparator::EvaluateComparator<int32>(
						Search.Operator,
						Search.PropertyName == ENPropertyValue::Difficulty
							? Block.Difficulty
							: Block.Position,
						Search.IntValue
					);
					if (bDebugSearch) UE_LOG(
						LogDialogSystem,
						Display,
						TEXT("%s>Is %s %s: %i"),
						*Decals,
						*ENUM_TO_STRING(ENansConditionComparator, Search.Operator),
						Search.PropertyName == ENPropertyValue::Difficulty
						? TEXT("Difficulty")
						: TEXT("Position"),
						bSuccess
					)
				}

				if (bSuccess)
				{
					if (bDebugSearch) UE_LOG(LogDialogSystem, Display, TEXT(">>Is success"));
					Results.Add(Block);
				}
			}
		}
	}
	if (bDebugSearch)
	{
		UE_LOG(LogDialogSystem, Display, TEXT("<<<<< END <<<<<<"),);
	}
	return Results;
}


#undef LOCTEXT_NAMESPACE
