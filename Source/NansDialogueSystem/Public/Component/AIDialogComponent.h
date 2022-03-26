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

#include "Components/ActorComponent.h"
#include "AIDialogComponent.generated.h"

class UBehaviorTreeComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NANSDIALOGUESYSTEM_API UAIDialogComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAIDialogComponent();
	void GetBehaviorTreeComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	bool bIsAborting = false;
	void OnBehaviorTreeTaskAbort() const;
	TWeakObjectPtr<UBehaviorTreeComponent> BehaviorTreeComp;
};
