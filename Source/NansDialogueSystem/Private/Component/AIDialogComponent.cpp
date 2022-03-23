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

#include "Component/AIDialogComponent.h"

#include "BTStepsLibrary.h"
#include "NDialogueSubsystem.h"
#include "NDSFunctionLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Service/BTDialogueDifficultyHandler.h"
#include "Service/BTDialoguePointsHandler.h"
#include "Service/DialogueBTHelpers.h"
#include "Setting/DialogueSystemSettings.h"

// Sets default values for this component's properties
UAIDialogComponent::UAIDialogComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// TODO create a UBTStepsComponent and embed it here
	// A UBTStepsComponent should to the same listening on OnBehaviorTreeTaskAbort()
	// to remove the StepsHandler associated to the AI controller
}

// Called when the game starts
void UAIDialogComponent::BeginPlay()
{
	Super::BeginPlay();

	// TODO create a UBTStepsComponent and create a protected function to embed this logic:
	const APawn* Pawn = Cast<APawn>(GetOwner());
	check(IsValid(Pawn), TEXT("You can attached a UAIDialogComponent only to Pawn or Character"))
	const AAIController* Controller = Cast<AAIController>(Pawn->GetController());
	checkf(IsValid(Controller), TEXT("A UAIDialogComponent has to be attached to a AIController only"));
	UBehaviorTreeComponent* BTComp = Cast<UBehaviorTreeComponent>(Controller->GetBrainComponent());
	checkf(IsValid(BTComp), TEXT("The brain component has to be a behavior tree component"));
	BehaviorTreeComp = MakeWeakObjectPtr(BTComp);
	check(BehaviorTreeComp.IsValid());
}

void UAIDialogComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!BehaviorTreeComp.IsValid()) return;

	if (!bIsAborting && BehaviorTreeComp->IsAbortPending())
	{
		bIsAborting = true;
		OnBehaviorTreeTaskAbort(*BehaviorTreeComp.Get());
	}
	else
	{
		bIsAborting = false;
	}
}

void UAIDialogComponent::OnBehaviorTreeTaskAbort(UBehaviorTreeComponent& OwnerComp) const
{
	const AAIController* AIOwner = OwnerComp.GetAIOwner();
	check(IsValid(AIOwner));
	// TODO Move this in a UBTStepsComponent, see above 
	UBTStepsLibrary::GetStepsSubsystem()->RemoveStepsHandler(AIOwner);
	UNDialogueSubsystem* DialSys = UNDSFunctionLibrary::GetDialogSubsystem();
	check(IsValid(DialSys));
	DialSys->EndDialogSequence(AIOwner);

	const FDialogueBehaviorTreeSettings& Settings = UDialogueSystemSettings::Get()->BehaviorTreeSettings;
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	UBTDialogueDifficultyHandler* BTDialogDifficultyHandler =
		Cast<UBTDialogueDifficultyHandler>(BlackboardComp->GetValueAsObject(Settings.DifficultyHandlerKey));

	if (IsValid(BTDialogDifficultyHandler))
	{
		BTDialogDifficultyHandler->Clear();
	}
	NDialogueBTHelpers::RemoveUIFromBlackboard(OwnerComp, BlackboardComp);
}
