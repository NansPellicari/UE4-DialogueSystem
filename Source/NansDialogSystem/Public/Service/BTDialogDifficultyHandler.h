#pragma once

#include "CoreMinimal.h"

#include "BTDialogDifficultyHandler.generated.h"

struct FNDialogDifficultySettings;

UCLASS() class NANSDIALOGSYSTEM_API UBTDialogDifficultyHandler : public UObject
{
	GENERATED_BODY()
public:
	UBTDialogDifficultyHandler();
	float GetDifficultyLevel(const FBTDialogueResponse& Response);

	UFUNCTION(BlueprintCallable)
	void SetFactor(FName Name, float Factor);

protected:
	/** Should change depending on the Character player has chosen */
	float GeneralDifficultyLevel = 1.f;
	TMap<FName, float> Factors;
	TArray<FNDialogDifficultySettings> Settings;
};
