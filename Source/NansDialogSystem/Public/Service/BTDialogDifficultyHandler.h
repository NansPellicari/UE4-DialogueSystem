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
	UFUNCTION(BlueprintCallable)
	FNResponseCategory Test(FNResponseCategory Category)
	{
		// UE_LOG(LogTemp, Warning, TEXT("%s - Color: %s"), ANSI_TO_TCHAR(__FUNCTION__), *Category.GetColor().ToString());
		return Category;
	}

protected:
	/** Should change depending on the Character player has chosen */
	float GeneralDifficultyLevel = 1.f;
	TMap<FName, float> Factors;
	TArray<FNDialogFactorSettings> Settings;
};
