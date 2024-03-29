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

#include "AI/Task/BTTask_SimpleResponses.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "UI/ResponseButtonWidget.h"

#define LOCTEXT_NAMESPACE "DialogueSystem"

UBTTask_SimpleResponses::UBTTask_SimpleResponses(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	NodeName = "Simple Responses";
	bNotifyTick = true;
	bNotifyTaskFinished = true;
}

void UBTTask_SimpleResponses::ReceiveOnTick(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	// ...
}

#undef LOCTEXT_NAMESPACE
