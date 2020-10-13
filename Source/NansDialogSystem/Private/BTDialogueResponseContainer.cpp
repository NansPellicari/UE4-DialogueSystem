// Fill out your copyright notice in the Description page of Project Settings.

#include "BTDialogueResponseContainer.h"

FBTDialogueResponse UBTDialogueResponseContainer::GetResponse() {
    return Response;
}

void UBTDialogueResponseContainer::SetResponse(FBTDialogueResponse _Response) {
    Response = _Response;
}

UBTDialogueResponseContainer * UBTDialogueResponseContainer::CreateNullObject(UObject* Outer)
{
    UBTDialogueResponseContainer* Container = NewObject<UBTDialogueResponseContainer>(Outer, StaticClass());
    FBTDialogueResponse Response = FBTDialogueResponse::CreateNullObject();
    Container->SetResponse(Response);
    return Container;
}

