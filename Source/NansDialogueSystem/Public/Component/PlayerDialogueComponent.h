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
#include "PointSystemHelpers.h"
#include "GameplayEffectTypes.h"
#include "Step.h"
#include "Components/ActorComponent.h"
#include "Dialogue/DialogueHistory.h"
#include "Dialogue/DialogueHistorySearch.h"
#include "Misc/BPDelegateHandle.h"
#include "PlayerDialogueComponent.generated.h"

// struct FNDialogueHistorySearch;
struct FNansConditionOperator;
class UAbilitySystemComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FDialogueEvent, class UPlayerDialogueComponent*);
DECLARE_DYNAMIC_DELEGATE_OneParam(FEventForDialogueEvent, class UPlayerDialogueComponent*, PlayerDialogueComp);

/**
 * The main Goal of this Component is to save data from dialog session.
 * It is also used to retrieve data on previous or actual session to perform some checks on previously chosen responses. 
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NANSDIALOGUESYSTEM_API UPlayerDialogueComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerDialogueComponent();
	UFUNCTION(BlueprintCallable, Category="Dialogue|History")
	void AddSequence(const FDialogueSequence& Sequence);
	UFUNCTION(BlueprintCallable, Category="Dialogue|History")
	void AddResponse(const FDialogueResult& Result);
	UFUNCTION(BlueprintCallable, Category="Dialogue|History")
	bool HasResults(const FNDialogueHistorySearch& Search);
	UFUNCTION(BlueprintCallable, Category="Dialogue|History")
	bool HasResultsOnSearches(const TArray<FNDialogueHistorySearch> Searches,
		TArray<FNansConditionOperator> ConditionsOperators);
	UFUNCTION(BlueprintCallable, Category="Dialogue|Points")
	int32 GetDialoguePoints(FNDialogueCategory Category) const;
	UFUNCTION(BlueprintCallable, Category="Dialogue|Points")
	void AddPoints(FNPoint Point, int32 Position, FNStep Step);
	UFUNCTION(BlueprintCallable, Category="Dialogue|History")
	TArray<FDialogueResult> SearchResults(const FNDialogueHistorySearch& Search) const;
	UPROPERTY(EditAnywhere, Category="PlayerDialogueComponent")
	bool bDebugSearch = false;
	UPROPERTY(EditAnywhere, Category="PlayerDialogueComponent")
	bool bDebug = false;

	// BEGIN BP delegates management declaration for OnDialogueStart
	FDialogueEvent OnDialogueStartEvent;
	UFUNCTION(BlueprintNativeEvent, Category = "Dialogue")
	void OnDialogueStart();
	virtual FDialogueEvent& OnNativeDialogueStart();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Dialogue")
	FBPDelegateHandle BindToOnDialogueStart(const FEventForDialogueEvent& Functor);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Dialogue")
	void UnbindToOnDialogueStart(UPARAM(ref) FBPDelegateHandle& Handle);
	virtual void OnDialogueStart_Implementation();
	virtual FBPDelegateHandle BindToOnDialogueStart_Implementation(const FEventForDialogueEvent& Functor);
	virtual void UnbindToOnDialogueStart_Implementation(FBPDelegateHandle& Handle);
	// END BP delegates management declaration for OnDialogueStart

	// BEGIN BP delegates management declaration for OnDialogueEnd
	FDialogueEvent OnDialogueEndEvent;
	UFUNCTION(BlueprintNativeEvent, Category = "Dialogue")
	void OnDialogueEnd();
	virtual FDialogueEvent& OnNativeDialogueEnd();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Dialogue")
	FBPDelegateHandle BindToOnDialogueEnd(const FEventForDialogueEvent& Functor);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Dialogue")
	void UnbindToOnDialogueEnd(UPARAM(ref) FBPDelegateHandle& Handle);
	virtual void OnDialogueEnd_Implementation();
	virtual FBPDelegateHandle BindToOnDialogueEnd_Implementation(const FEventForDialogueEvent& Functor);
	virtual void UnbindToOnDialogueEnd_Implementation(FBPDelegateHandle& Handle);
	// END BP delegates management declaration for OnDialogueEnd

protected:
	void DialogueTagChange(const FGameplayTag CallbackTag, int32 NewCount);
	virtual void BeginPlay() override;
	UPROPERTY()
	UAbilitySystemComponent* ABSComp;
	UPROPERTY()
	TArray<FDialogueHistory> DialogueHistories;
	int32 CurrentSequenceIndex = -1;
	int32 CurrentHistoryIndex = -1;
};
