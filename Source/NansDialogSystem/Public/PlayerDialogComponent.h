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

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerDialogComponent.generated.h"


/**
 * The main Goal of this Component is to save data from dialog session.
 * It is also used to retrieve data on previous or actual session to perform some checks on previously chosen responses. 
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NANSDIALOGSYSTEM_API UPlayerDialogComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerDialogComponent();

protected:
	virtual void BeginPlay() override;
	;
};
