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
#include "AI/BehaviorTree/Tasks/BTTask_Base.h"

#include "BTDialogueTypes.h"
#include "NansUE4Utilities/public/Misc/TextLibrary.h"

#include "BTTask_ButtonsSequence.generated.h"

class UBTDialogPointsHandler;
class UDialogHUD;
class UPanelWidget;
class UButtonSequenceWidget;
class NButtonSequenceMovementManager;
class UBTTask_Countdown;

/**
 * Dialogue Response Struct
 */
USTRUCT(BlueprintType)
struct NANSDIALOGSYSTEM_API FBTButtonSequenceResponse
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Response")
	int32 ForPoint = -1;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Response", meta = (MultiLine = true))
	FText Response;

	/** If true, tasks will use the Default GameplayEffect set in DialogSystemSettings. */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Response")
	bool bUsedDefaultEffect = true;


	/**
	* /!\ Note: Be aware that this effect is used to create a GameplayEffectSpec in Tasks to add:
	*
	* - some level magnitude (using SetByCaller)
	* - some extra data to pass to the PlayerDialogComponent (see FNDSGameplayEffectContext)
	* - asset tags to identify easily the gameplay effect as a dialog effect (see UDialogSystemSettings::TagToIdentifyDialogEffect)
	*
	* Take this in consideration when you build your GameplayEffect.
	*/
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Response", Meta=(EditCondition="!bUsedDefaultEffect"))
	TSubclassOf<UGameplayEffect> EffectOnEarned;

	/** this is given by FBTButtonSequence to easily use GetSpawnableEffectOnEarned() */
	FNDialogueCategory Category;
	TSubclassOf<UGameplayEffect> GetSpawnableEffectOnEarned() const;
};

/**
 * Dialogue Reponse Struct
 */
USTRUCT(BlueprintType)
struct NANSDIALOGSYSTEM_API FBTButtonSequence
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Response")
	FBTButtonSequenceResponse Default;

	/**
	 * Can be OSBD for ex:
	 *      fr: Observation Sentiment Besoin Demande
	 *      en: Observation Feeling Need Request
	 *      = 24 combinations
	 * Or IAJA:
	 *      fr: Interpretation Analyse Jugement Agression
	 *      en: Interpretation Analysis Judgment Aggression
	 *      Easier = only 12 combinations
	 */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Response")
	FText ButtonSequence;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Response")
	FNDialogueCategory Category;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Response")
	EResponseDirection Direction = EResponseDirection::NONE;

	// This allow to divide the number of found letters in order
	// with this coefficient to compute earned points.
	// So when all letters are in order, the max value earnable is this number.
	// eg.  if we set levelCoefficient of 3 for the sequence OSBD:
	//      if the player gives BDOS, so 2 letters in order, point earned is:
	//      floor((2/TotalLetters)*coeff) = floor((2/4)*3) = 1,
	//      For DBSO (no letters at all) floor((0/4)*3) = 0
	// TODO maybe player should earns more points if he sets 2*2letters in order, should not be the same as BDOS
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Response")
	int32 LevelCoefficient = 3;
	/**
	 * When you need to retrieved these, use GetResponsesForPoint() instead.
	 * It add the category set here for each button.
	 */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Response")
	TArray<FBTButtonSequenceResponse> ResponsesForPoint;

	// Takes every time the last defined response:
	// eg. I created this list responses:
	// - default response is "response default"
	// - for point 1 = "response 1"
	// - for point 3 = "response 2"
	// if points passed is 0, returned response is "response default"
	// if points passed is 2, returned response is "response 1"
	// if points passed is 3, returned response is "response 2"
	FBTButtonSequenceResponse GetResponseForPoints(const int32 Points)
	{
		Default.Category = Category;
		if (ResponsesForPoint.Num() <= 0) return Default;

		FBTButtonSequenceResponse Response = Default;

		TArray<FBTButtonSequenceResponse> Responses = GetResponsesForPoint();

		for (const FBTButtonSequenceResponse Resp : Responses)
		{
			if (Points < Resp.ForPoint)
			{
				return Response;
			}
			Response = Resp;
		}
		return Response;
	}

	TArray<FBTButtonSequenceResponse> GetResponsesForPoint()
	{
		TArray<FBTButtonSequenceResponse> AllResponses;
		AllResponses.Add(Default);
		for (FBTButtonSequenceResponse Response : ResponsesForPoint)
		{
			Response.Category = Category;
			AllResponses.Add(Response);
		}
		return AllResponses;
	}
};

static TMap<FString, TMap<int32, FString>> StaticButtonSequenceDescs;

struct StaticButtonSequenceDescriptions
{
public:
	static void SetDescription(const FBTButtonSequence& Sequence, FString Desc)
	{
		const FString SequenceStr = Sequence.ButtonSequence.ToString();
		TMap<int32, FString>& DescMap = StaticButtonSequenceDescs.FindOrAdd(SequenceStr);
		DescMap.Add(Sequence.LevelCoefficient, Desc);
	}

	static FString GetDescription(const FBTButtonSequence& Sequence)
	{
		FString* StaticDesc = nullptr;
		const FString SequenceStr = Sequence.ButtonSequence.ToString();
		TMap<int32, FString>* StaticLevelDesc = StaticButtonSequenceDescs.Find(SequenceStr);

		if (StaticLevelDesc == nullptr)
		{
			return "";
		}

		StaticDesc = StaticLevelDesc->Find(Sequence.LevelCoefficient);

		if (StaticDesc == nullptr)
		{
			return "";
		}

		return *StaticDesc;
	}
};

/**
 *
 */
UCLASS()
class NANSDIALOGSYSTEM_API UBTTask_ButtonsSequence : public UBTTask_Base
{
	GENERATED_BODY()

	UBTTask_ButtonsSequence(const FObjectInitializer& objectInitializer);

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
#if WITH_EDITOR
	virtual void PostLoad() override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif	  // WITH_EDITOR

	virtual FString GetStaticDescription() const override;
	virtual void BeginDestroy() override;

	UFUNCTION()
	void OnButtonClick(UButtonSequenceWidget* Button);

	UFUNCTION()
	void OnCountdownEnds(UBehaviorTreeComponent* OwnerComp);

	UFUNCTION()
	void OnEndDisplayResponse();

protected:
	/**
	 * HUD need to be composed with widget named
	 * "WheelButton" (UWheelButtonWidget) and "ProgressBar" (UWheelProgressBarWidget)
	 * to work properly.
	 */
	UPROPERTY(VisibleAnywhere, Category = "HUD")
	FName UINameKey = NAME_None;

	UPROPERTY(EditInstanceOnly, Category = "HUD")
	TSubclassOf<UButtonSequenceWidget> ButtonWidget;

	UPROPERTY(EditInstanceOnly, Category = "HUD")
	FName ButtonsSlotName = FName("ButtonsSlot");

	UPROPERTY(EditInstanceOnly, Category = "HUD")
	FName ButtonAfterName = FName("ButtonAfter");

	UPROPERTY(EditInstanceOnly, Category = "Service")
	FName PointsHandlerKeyName = FName("PointsHandler");

	UPROPERTY(EditInstanceOnly, Category = "Responses")
	FName ResponseContainerKey = FName("ResponseContainer");

	UPROPERTY(EditInstanceOnly, Category = "Responses")
	float DefaultVelocity = 1;

	UPROPERTY(EditInstanceOnly, Category = "Responses")
	TArray<FBTButtonSequence> Sequences;

	UPROPERTY(EditInstanceOnly, Category = "Countdown")
	int32 TimeToResponse = 0;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
		EBTNodeResult::Type TaskResult) override;

	UPROPERTY()
	UBehaviorTreeComponent* OwnerComponent;
	UPROPERTY()
	UBlackboardComponent* Blackboard;
	UPROPERTY()
	UDialogHUD* DialogHUD;
	UPROPERTY()
	UPanelWidget* ButtonsSlot;
	UPROPERTY()
	UBTTask_Countdown* CountDownTask;

private:
	TSharedPtr<NButtonSequenceMovementManager> BTSequenceManager;
	UPROPERTY()
	UBTDialogPointsHandler* PointsHandler;
	FString PlayerTries;
	int32 SequenceIndex = -1;
	EBTNodeResult::Type TriesStatus = EBTNodeResult::InProgress;
	int32 GetEarnedPoint();
	const int32 PointsComparedToSequence(const FString& Tries, const FBTButtonSequence& Sequence) const;
	void CreateButtons();
	void RemoveButtons(UBehaviorTreeComponent& OwnerComp);
	FString ShowPermutationsPoints(const FBTButtonSequence& Sequence) const;
};
