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
#include "NansFactorsFactoryUE4/Public/FactorUnit/FactorUnitView.h"

#include "DialogFactorUnit.generated.h"

UCLASS()
class NANSDIALOGSYSTEM_API UNDialogFactorUnit : public UNFactorUnitView
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, Category = "FactorsFactory|Unit")
	int32 Step;
	UPROPERTY(BlueprintReadWrite, Category = "FactorsFactory|Unit")
	int32 Position;
	UPROPERTY(BlueprintReadWrite, Category = "FactorsFactory|Unit")
	int32 InitialPoint;
	UPROPERTY(BlueprintReadWrite, Category = "FactorsFactory|Unit")
	FGameplayTag CategoryName;
	UPROPERTY(BlueprintReadWrite, Category = "FactorsFactory|Unit")
	FString BehaviorTreePathName;
	UPROPERTY(BlueprintReadWrite, Category = "FactorsFactory|Unit")
	FString AIPawnPathName;
};
