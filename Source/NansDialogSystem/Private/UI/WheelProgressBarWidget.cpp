// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/WheelProgressBarWidget.h"

void UWheelProgressBarWidget::SetPercent(float _Percent)
{
    Percent = _Percent;
    ReceiveSetPercent(Percent);
}

void UWheelProgressBarWidget::SetProgress(float _Percent)
{
    SetPercent(_Percent);
    OnProgress.Broadcast(_Percent);
    ReceiveOnSetProgress(_Percent);
}
