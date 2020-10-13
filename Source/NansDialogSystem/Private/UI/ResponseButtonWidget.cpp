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
	FLinearColor* AlignmentColor = nullptr;

	if (Response->GetResponse().Alignment == EAlignment::CNV)
	{
		AlignmentColor = &ColorCNV;
	}
	else if (Response->GetResponse().Alignment == EAlignment::CSV)
	{
		AlignmentColor = &ColorCSV;
	}
	else
	{
		FinalColor = ColorNeutral;
	}

	if (AlignmentColor != nullptr)
	{
		FVector vNeutralColor = FVector(ColorNeutral.R, ColorNeutral.G, ColorNeutral.B);
		FVector vAlignmentColor = FVector(AlignmentColor->R, AlignmentColor->G, AlignmentColor->B);
		FVector DiffColor = vAlignmentColor - vNeutralColor * ((float) Response->GetResponse().Level / (float) MaxLevel);
		vNeutralColor += DiffColor;
		FinalColor.R = vNeutralColor.X;
		FinalColor.G = vNeutralColor.Y;
		FinalColor.B = vNeutralColor.Z;
		FinalColor.A = AlignmentColor->A;
	}
}

UBTDialogueResponseContainer* UResponseButtonWidget::GetResponse()
{
	return Response;
}

void UResponseButtonWidget::CallBTClicked(UResponseButtonWidget* Button)
{
	OnBTClicked.Broadcast(this);
}
