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
	FName CategoryName;
	UPROPERTY(BlueprintReadWrite, Category = "FactorsFactory|Unit")
	FString BehaviorTreePathName;
	UPROPERTY(BlueprintReadWrite, Category = "FactorsFactory|Unit")
	FString AIPawnPathName;
};
