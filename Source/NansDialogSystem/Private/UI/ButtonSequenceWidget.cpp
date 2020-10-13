// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/ButtonSequenceWidget.h"

void UButtonSequenceWidget::SetText(FString _Text)
{
	if (!ensure(_Text.Len() == 1)) return;

	Text = _Text;
}

FString UButtonSequenceWidget::GetText() const
{
	return Text;
}

void UButtonSequenceWidget::CallBTClicked()
{
	OnBTClicked.Broadcast(this);
}
