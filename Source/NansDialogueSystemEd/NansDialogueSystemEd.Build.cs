// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
	public class NansDialogueSystemEd : ModuleRules
	{
		public NansDialogueSystemEd(ReadOnlyTargetRules Target) : base(Target)
		{
			PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

			PrivateDependencyModuleNames.AddRange(
				new[]
				{
					"Core",
					"CoreUObject",
					"Engine",
					"UnrealEd",
					"BlueprintGraph",
					"GraphEditor",
					"PropertyEditor",
					"UMG",
					"Slate",
					"SlateCore",
					"InputCore",
					"Kismet",
					"KismetCompiler",
					"GameplayAbilities",
					"GameplayTags",
					"EditorStyle",
					"NansCoreHelpers",
					"NansUE4Utilities",
					"NansBehaviorSteps",
					"NansUMGExtent",
					"NansDialogueSystem"
				});
			PrivateIncludePaths.AddRange(
				new[]
				{
					"NansDialogueSystemEd/Private"
				});
		}
	}
}
