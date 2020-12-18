// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
	public class NansDialogSystemEd : ModuleRules
	{
		public NansDialogSystemEd(ReadOnlyTargetRules Target) : base(Target)
		{
			PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

			PrivateDependencyModuleNames.AddRange(
				new string[]
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
					"EditorStyle",
					"NansCoreHelpers",
					"NansUE4Utilities",
					"NansFactorsFactoryCore",
					"NansFactorsFactoryUE4",
					"NansBehaviorSteps",
					"NansUMGExtent",
					"NansCommon",
					"Interactive",
					"NansDialogSystem",
				});
			PrivateIncludePaths.AddRange(
				new string[]
				{
					"NansDialogSystemEd/Private"
				});
		}
	}
}
