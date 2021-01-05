// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
	public class NansDialogSystem : ModuleRules
	{
		public NansDialogSystem(ReadOnlyTargetRules Target) : base(Target)
		{
			PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

			PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
				}
			);

			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"CoreUObject",
					"Engine", // TODO This one should be removed when NansCommon be removed too
					"AIModule",
					"GameplayTasks",
					"UMG",
					"Slate",
					"SlateCore",
					"InputCore",
					"DeveloperSettings",
					"NansCoreHelpers",
					"NansUE4Utilities",
					"NansTimelineSystemUE4",
					"NansFactorsFactoryCore",
					"NansFactorsFactoryUE4",
					"NansBehaviorSteps",
					"NansUMGExtent",
					"NansCommon", // TODO should remove this dependencies
					"Interactive",
				}
			);

			PrivateIncludePathModuleNames.AddRange(
				new string[]
				{
					"MessageLog",
				}
			);
		}
	}
}
