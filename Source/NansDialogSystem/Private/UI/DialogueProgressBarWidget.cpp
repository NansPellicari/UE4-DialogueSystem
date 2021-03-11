// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/DialogueProgressBarWidget.h"

void UDialogueProgressBarWidget::SetPercent(float _Percent)
{
    Percent = _Percent;
    ReceiveSetPercent(Percent);
}

void UDialogueProgressBarWidget::SetProgress(float _Percent)
{
    SetPercent(_Percent);
    OnProgress.Broadcast(_Percent);
    ReceiveOnSetProgress(_Percent);
}
