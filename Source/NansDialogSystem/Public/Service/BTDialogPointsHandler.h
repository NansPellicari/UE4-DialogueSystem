#pragma once

#include "BTDialogueTypes.h"
#include "CoreMinimal.h"

#include "BTDialogPointsHandler.generated.h"

struct FNDialogFactorSettings;
class IBTStepsHandler;

/**
 * Points Associated to step
 * TODO remove this and replace by factor
 */
USTRUCT(BlueprintType)
struct NANSDIALOGSYSTEM_API FBTPointInStep
{
	GENERATED_USTRUCT_BODY()

	FBTPointInStep(int32 _Step = 0, FPoint _Point = FPoint(), int32 _Position = 0)
		: Step(_Step), Point(_Point), Position(_Position){};
	int32 Step;
	FPoint Point;
	int32 Position;
};

UCLASS(BlueprintType)
class NANSDIALOGSYSTEM_API UBTDialogPointsHandler : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	bool bDebug = false;

	UBTDialogPointsHandler();
	void Initialize(TScriptInterface<IBTStepsHandler> _StepsHandler);

	virtual void BeginDestroy() override;

	UFUNCTION(BlueprintCallable, Category = "PointsHandler")
	void AddPoints(FPoint Point, int32 Position);

	UFUNCTION(BlueprintCallable, Category = "PointsHandler")
	void getLastResponse(FBTPointInStep& PointInStep);

	UFUNCTION(BlueprintCallable, Category = "PointsHandler")
	void getLastResponseFromStep(const int32 SearchStep, FBTPointInStep& PointInStep);

	UFUNCTION(BlueprintCallable, Category = "PointsHandler")
	int32 GetPoints(FNResponseCategory Category) const;

protected:
	UPROPERTY()
	TScriptInterface<IBTStepsHandler> StepsHandler;
	TMap<FName, TArray<FNDialogFactorTypeSettings>> Settings;

	/** A HEAP on point by step, the HEAP allow to keep trace of action's chronologicaly */
	TArray<FBTPointInStep> HeapResponses;

	UPROPERTY(BlueprintReadOnly, Category = "PointsHandler")
	TMap<FName, int32> EarnedPoints;
};
