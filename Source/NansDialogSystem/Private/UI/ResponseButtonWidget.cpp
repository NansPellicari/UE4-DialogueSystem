// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/ResponseButtonWidget.h"

#include "BTDialogueResponseContainer.h"
#include "Components/Button.h"

void UResponseButtonWidget::SetResponse(UBTDialogueResponseContainer* DialogueResponse)
{
	Response = DialogueResponse;
}

void UResponseButtonWidget::ComputeColor(int32 MaxLevel)
{
	FLinearColor BtColor = Response->GetResponse().Category.GetColor();

	FVector vBaseColor = FVector(BaseColor.R, BaseColor.G, BaseColor.B);
	FVector vBtColor = FVector(BtColor.R, BtColor.G, BtColor.B);
	FVector DiffColor = vBtColor - vBaseColor * ((float) Response->GetResponse().Point / (float) (MaxLevel <= 0 ? 1 : MaxLevel));
	vBaseColor += DiffColor;
	FinalColor.R = vBaseColor.X;
	FinalColor.G = vBaseColor.Y;
	FinalColor.B = vBaseColor.Z;
	FinalColor.A = BtColor.A;
}

UBTDialogueResponseContainer* UResponseButtonWidget::GetResponse()
{
	return Response;
}

void UResponseButtonWidget::CallBTClicked(UResponseButtonWidget* Button)
{
	OnBTClicked.Broadcast(this);
}
