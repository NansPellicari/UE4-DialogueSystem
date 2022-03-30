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

#include "NDialogueSubsystem.h"
#include "NDSFunctionLibrary.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Service/DialoguePointsHandler.h"
#include "Service/DialogueBTHelpers.h"

// Sets default values for this component's properties
UAIDialogComponent::UAIDialogComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

void UAIDialogComponent::GetBehaviorTreeComponent()
{
	const APawn* Pawn = Cast<APawn>(GetOwner());
	checkf(IsValid(Pawn), TEXT("You can attached a UAIDialogComponent only to Pawn or Character"));
	const AAIController* Controller = Cast<AAIController>(Pawn->GetController());
	checkf(IsValid(Controller), TEXT("A UAIDialogComponent has to be attached to a AIController only"));
	UBrainComponent* Brain = Controller->GetBrainComponent();
	if (Brain)
	{
		UBehaviorTreeComponent* BTComp = Cast<UBehaviorTreeComponent>(Brain);
		checkf(BTComp, TEXT("The brain component has to be a behavior tree component"));
		BehaviorTreeComp = MakeWeakObjectPtr(BTComp);
		check(BehaviorTreeComp.IsValid());
	}
}

// Called when the game starts
void UAIDialogComponent::BeginPlay()
{
	Super::BeginPlay();

	GetBehaviorTreeComponent();
}

void UAIDialogComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                       FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!BehaviorTreeComp.IsValid())
	{
		GetBehaviorTreeComponent();
	}
	if (!BehaviorTreeComp.IsValid())
	{
		return;
	}

	if (!bIsAborting && BehaviorTreeComp->IsAbortPending())
	{
		bIsAborting = true;
		OnBehaviorTreeTaskAbort();
	}
	else
	{
		bIsAborting = false;
	}
}

void UAIDialogComponent::OnBehaviorTreeTaskAbort() const
{
	if (BehaviorTreeComp.Get())
	{
		const AAIController* AIOwner = BehaviorTreeComp->GetAIOwner();
		check(IsValid(AIOwner));
		UNDialogueSubsystem* DialSys = UNDSFunctionLibrary::GetDialogSubsystem();
		check(IsValid(DialSys));
		DialSys->EndDialogSequence(AIOwner);
		UBlackboardComponent* BlackboardComp = BehaviorTreeComp->GetBlackboardComponent();
		NDialogueBTHelpers::RemoveUIFromBlackboard(*BehaviorTreeComp.Get(), BlackboardComp);
	}
}
