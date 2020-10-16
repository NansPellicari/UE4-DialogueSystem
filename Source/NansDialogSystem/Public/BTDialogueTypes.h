#pragma once

#include "Attribute/ResponseCategory.h"
#include "NansDialogSystem/Public/PointSystemHelpers.h"

#include "BTDialogueTypes.generated.h"

UENUM(BlueprintType)
enum class EResponseDirection : uint8
{
	UP,
	DOWN,
	NONE,
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
	float DifficultyLevel = 0;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Response")
	FNResponseCategory Category;

	static FBTDialogueResponse CreateNullObject()
	{
		FBTDialogueResponse Response = FBTDialogueResponse();
		Response.Text = NSLOCTEXT("DialogSystem", "DefaultDialogueResponseText", "Euh...");
		return Response;
	}
};
