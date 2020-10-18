#pragma once

#include "BTDialogueTypes.h"
#include "CoreMinimal.h"

#include "BTDialogDifficultyHandler.generated.h"

struct FNDialogFactorSettings;

UCLASS()
class NANSDIALOGSYSTEM_API UBTDialogDifficultyHandler : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	bool bDebug = false;

	UBTDialogDifficultyHandler();
	float GetDifficultyLevel(const FBTDialogueResponse& Response);

	UFUNCTION(BlueprintCallable)
	void SetFactor(FName Name, float Factor);

protected:
	/** Should change depending on the Character the player will choose */
	float GeneralDifficultyLevel = 1.f;
	TMap<FName, float> Factors;
	TArray<FNDialogFactorSettings> Settings;
};
