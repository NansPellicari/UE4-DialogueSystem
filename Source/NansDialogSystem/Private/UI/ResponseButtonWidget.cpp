// Copyright 2020-present Nans Pellicari (nans.pellicari@gmail.com).
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "UI/ResponseButtonWidget.h"

void UResponseButtonWidget::SetResponse(FBTDialogueResponse DialogueResponse)
{
	Response = DialogueResponse;
}

void UResponseButtonWidget::ComputeColor(int32 MaxLevel)
{
	FLinearColor BtColor = FNDialogueCategory::GetColorFromSettings(Response.Category);

	FVector vBaseColor = FVector(BaseColor.R, BaseColor.G, BaseColor.B);
	FVector vBtColor = FVector(BtColor.R, BtColor.G, BtColor.B);
	FVector DiffColor = vBtColor - vBaseColor * (
							static_cast<float>(Response.Point) / static_cast<float>(MaxLevel <= 0 ? 1 : MaxLevel));
	vBaseColor += DiffColor;
	FinalColor.R = vBaseColor.X;
	FinalColor.G = vBaseColor.Y;
	FinalColor.B = vBaseColor.Z;
	FinalColor.A = BtColor.A;
}

FBTDialogueResponse UResponseButtonWidget::GetResponse()
{
	return Response;
}

void UResponseButtonWidget::CallBTClicked()
{
	OnBTClicked.Broadcast(this);
}

void UResponseButtonWidget::SetText(FString InText)
{
	if (!ensure(InText.Len() == 1)) return;

	Text = InText;
}

FString UResponseButtonWidget::GetText() const
{
	return Text;
}
