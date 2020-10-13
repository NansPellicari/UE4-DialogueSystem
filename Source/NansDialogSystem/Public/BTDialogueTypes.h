#pragma once

#include "NansDialogSystem/Public/PointSystemHelpers.h"

#include "BTDialogueTypes.generated.h"

UENUM(BlueprintType)
enum class EResponseDirection : uint8
{
	UP,
	DOWN,
};

/**
 * Dialogue Reponse Struct
 */

USTRUCT(BlueprintType)
struct NANSDIALOGSYSTEM_API FBTDialogueResponse
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Response", meta = (MultiLine = true))
	FText Text;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Response")
	int32 Level = 0;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Response")
	float WhenReach = 0;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Response")
	EAlignment Alignment = EAlignment::Neutral;

	static FBTDialogueResponse CreateNullObject()
	{
		FBTDialogueResponse Response = FBTDialogueResponse();
		Response.Alignment = EAlignment::None;
		Response.Text = NSLOCTEXT("DialogSystem", "DefaultDialogueResponseText", "Euh...");
		return Response;
	}
};
