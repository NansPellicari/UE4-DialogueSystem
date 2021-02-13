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

#include "NansDialogSystem/Public/Component/PlayerDialogComponent.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
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
	auto ABSComp = Cast<UAbilitySystemComponent>(
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

TArray<FDialogueResult> UPlayerDialogComponent::SearchResults(const FNDialogueHistorySearch& Search)
{
	TArray<FDialogueResult> Results;
	FDialogueHistory* CurrentHistory = CurrentHistoryIndex > -1
										   ? &DialogueHistories[CurrentHistoryIndex]
										   : &DialogueHistories.Last();
	FDialogueSequence* CurrentSequence = CurrentSequenceIndex > -1
											 ? &CurrentHistory->Sequences[CurrentSequenceIndex]
											 : &CurrentHistory->Sequences.Last();
	for (const FDialogueHistory& History : DialogueHistories)
	{
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

		for (const FDialogueSequence& Sequence : History.Sequences)
		{
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
				continue;
			}
			if (
				!Search.OwnerName.bIsAll &&
				!Search.OwnerName.bIsCurrent &&
				Search.OwnerName.Value != Sequence.Owner
			)
			{
				continue;
			}
			if (
				Search.OwnerName.bIsCurrent &&
				Sequence.Owner != CurrentSequence->Owner
			)
			{
				continue;
			}

			int32 LastIdx = Sequence.Results.Num() - 1;
			int32 Idx = 0;
			for (const FDialogueResult& Block : Sequence.Results)
			{
				if (
					!Search.DialogName.bIsAll &&
					!Search.DialogName.Value.IsEmpty() &&
					Search.DialogName.Value != Block.BlockName.ToString()
				)
				{
					continue;
				}
				if (Search.DialogName.bLastOnly && LastIdx > Idx)
				{
					continue;
				}
				bool bSuccess = false;
				if (Search.PropertyName == ENPropertyValue::All)
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
				}
				if (!bSuccess && Search.IsName())
				{
					bSuccess = UNansComparator::EvaluateComparator<FString>(
						Search.Operator,
						Block.BlockName.ToString(),
						Search.NameValue.ToString()
					);
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
				}

				if (bSuccess)
				{
					Results.Add(Block);
				}
				Idx++;
			}
		}
	}
	return Results;
}


#undef LOCTEXT_NAMESPACE
