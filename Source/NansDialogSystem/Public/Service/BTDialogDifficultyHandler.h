#pragma once

#include "BTDialogueTypes.h"
#include "CoreMinimal.h"

#include "BTDialogDifficultyHandler.generated.h"

struct FNDialogFactorSettings;
class UNFactorsFactoryClientAdapter;

UCLASS()
class NANSDIALOGSYSTEM_API UBTDialogDifficultyHandler : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	bool bDebug = false;

	void Initialize();

	UBTDialogDifficultyHandler();
	float GetDifficultyLevel(const FBTDialogueResponse& Response);

protected:
	/** Should change depending on the Character the player will choose */
	float GeneralDifficultyLevel = 1.f;
	TMap<FName, float> Factors;
	TArray<FNDialogFactorSettings> Settings;

	UPROPERTY()
	UNFactorsFactoryClientAdapter* FactorsClient;
};
