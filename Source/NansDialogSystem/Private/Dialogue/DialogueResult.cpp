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

#include "Dialogue/DialogueResult.h"

FString FDialogueResult::ToString() const
{
	FStringFormatOrderedArguments Args;
	Args.Add(Position);
	Args.Add(BlockName.ToString());
	Args.Add(Difficulty);
	Args.Add(InitialPoints);
	Args.Add(CategoryName.ToString());
	return FString::Format(
		TEXT(" Position: {0}, BlockName: [{1}], Difficulty: {2}, InitialPoints: {3}, CategoryName: {4}"),
		Args
	);
}
