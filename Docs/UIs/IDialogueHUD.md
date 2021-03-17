# IDialogueHUD

This interface allows to communicate with your HUD in the Dialogue System.  
I didn't create a basic implementation class here cause I think it's too much dependant on the project settings and I don't want to overload the editor with a **_never used Blueprint_**.  
But if you need some tips, below you'll find an example of a basic implementation.

## Implementation's example

```cpp
// ====================================
// Header file: MyHUD.h
// ====================================
#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"
#include "GameFramework/HUD.h"
#include "HUD/MessageableHUD.h"
#include "UI/DialogueHUD.h"
#include "UObject/Object.h"
#include "Widgets/Layout/Anchors.h"

#include "MyHUD.generated.h"

/**
 * This is absolutly not required.
 * But it can be useful...
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnHUDChangeUIPanelEvent,
	const FName&,
	UIName,
	bool,
	bIsDisplayedOrHide
);

/**
 * /!\ The AMessageableHUD overrides is required if you used the task "NoteToPlayer".
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class MYGAME_API AMyHUD : public AMessageableHUD, public IDialogueHUD
{
	GENERATED_BODY()

public:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual FORCEINLINE bool IsDisplayed_Implementation(const FName Name) const override
	{
		return Name == CurrentUIPanel.Name;
	}

	virtual FORCEINLINE UUserWidget* GetCurrentUIPanel_Implementation() const override
	{
		return CurrentUIPanel.Widget;
	}

	virtual FORCEINLINE UUserWidget* GetFullCurrentUIPanel_Implementation(TSubclassOf<UUserWidget>& OutWidgetClass,
		FName& OutName) const override
	{
		OutName = CurrentUIPanel.Name;
		OutWidgetClass = CurrentUIPanel.Widget->GetClass();
		return CurrentUIPanel.Widget;
	}

	virtual UUserWidget* DisplayUIPanel_Implementation(TSubclassOf<UUserWidget> WidgetClass, FName Name) override;

	virtual void HideUIPanel_Implementation(FName Name) override;

	UPROPERTY(BlueprintAssignable, Category = "Event")
	FOnHUDChangeUIPanelEvent OnHUDChangeUIPanel;

protected:

	struct FCurrentWidget
	{
		FName Name;
		UUserWidget* Widget;
	};

	FCurrentWidget CurrentUIPanel;
	virtual void BeginPlay() override;
};

```

```cpp
// ====================================
// Body file: MyHUD.cpp
// ====================================

#include "MyHUD.h"
#include "Blueprint/UserWidget.h"


void AMyHUD::BeginPlay()
{
	Super::BeginPlay();
	// If you need a default UI when HUD start to play
	// Execute_DisplayUIPanel(this, UMyUIPanel::StaticClass(), FName("DefaultUI"));
}

void AMyHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	CurrentUIPanel.Widget = nullptr;
}

UUserWidget* AMyHUD::DisplayUIPanel_Implementation(TSubclassOf<UUserWidget> WidgetClass, FName Name)
{
	if (CurrentUIPanel.Name == Name && CurrentUIPanel.Widget->GetClass() == WidgetClass)
	{
		if (bDebug)
		{
			UE_LOG(
				LogTemp,
				Warning,
				TEXT("%s The UI panel %s is already displayed"),
				ANSI_TO_TCHAR(__FUNCTION__),
				*Name.ToString()
			);
		}
		return CurrentUIPanel.Widget;
	}

	if (IsValid(CurrentUIPanel.Widget))
	{
		Execute_HideUIPanel(this, CurrentUIPanel.Name);
	}
	CurrentUIPanel.Name = Name;
	CurrentUIPanel.Widget = CreateWidget(GetOwningPlayerController(), WidgetClass);
	// Rather than this, you can also have a main Parent widget in which every new UIs will be added to.
	// This way you can manage Z order between UIs for example.
	CurrentUIPanel.Widget->AddToViewport();
	OnHUDChangeUIPanel.Broadcast(Name, true);
	return CurrentUIPanel.Widget;
}

void AMyHUD::HideUIPanel_Implementation(FName Name)
{
	if (CurrentUIPanel.Name == Name && IsValid(CurrentUIPanel.Widget))
	{
		CurrentUIPanel.Widget->StopAllAnimations();
		CurrentUIPanel.Widget->RemoveFromParent();
		CurrentUIPanel.Widget = nullptr;
		CurrentUIPanel.Name = NAME_None;
		OnHUDChangeUIPanel.Broadcast(Name, false);
	}
}

```
