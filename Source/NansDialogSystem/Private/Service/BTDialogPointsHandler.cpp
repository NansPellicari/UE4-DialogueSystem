#include "Service/BTDialogPointsHandler.h"

#include "BTDialogueTypes.h"
#include "NansBehaviorSteps/Public/BTStepsHandler.h"
#include "Setting/DialogSystemSettings.h"


void UBTDialogPointsHandler::BeginDestroy()
{
	EarnedPoints.Empty();
	HeapResponses.Empty();
	Super::BeginDestroy();
}

UBTDialogPointsHandler::UBTDialogPointsHandler()
{
	UDialogSystemSettings::Get()->GetPointsEarnerConfigs(Settings);
}

void UBTDialogPointsHandler::Initialize(TScriptInterface<IBTStepsHandler> _StepsHandler)
{
	StepsHandler = _StepsHandler;
}

void UBTDialogPointsHandler::AddPoints(FPoint Point, int32 Position)
{
	int32 Step = IBTStepsHandler::Execute_GetCurrentStep(StepsHandler.GetObject());
	HeapResponses.Emplace(FBTPointInStep(Step, Point, Position));
	int32& PointNumber = EarnedPoints.FindOrAdd(Point.Category.Name);
	PointNumber += Point.Point;
}

void UBTDialogPointsHandler::getLastResponse(FBTPointInStep& PointInStep)
{
	PointInStep = HeapResponses.Last();
}
int32 UBTDialogPointsHandler::GetPoints(FNResponseCategory Category) const
{
	int32 TotalPoints = 0;
	for (FBTPointInStep Point : HeapResponses)
	{
		TotalPoints += Point.Point.Category.Name == Category.Name ? Point.Point.Point : 0;
	}

	return TotalPoints;
}

void UBTDialogPointsHandler::getLastResponseFromStep(const int32 SearchStep, FBTPointInStep& PointInStep)
{
	for (int32 i = HeapResponses.Num() - 1; i >= 0; --i)
	{
		if (HeapResponses[i].Step == SearchStep)
		{
			PointInStep = HeapResponses[i];
			break;
		}
	}
}
