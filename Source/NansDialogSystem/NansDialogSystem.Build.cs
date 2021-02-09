// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
	public class NansDialogSystem : ModuleRules
	{
		public NansDialogSystem(ReadOnlyTargetRules Target) : base(Target)
		{
			PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

			PublicDependencyModuleNames.AddRange(
				new[]
				{
					"Core"
				}
			);

			PrivateDependencyModuleNames.AddRange(
				new[]
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
					"GameplayAbilities",
					"GameplayTags",
					"NansCoreHelpers",
					"NansUE4Utilities",
					"NansTimelineSystemUE4",
					"NansFactorsFactoryCore",
					"NansFactorsFactoryUE4",
					"NansBehaviorSteps",
					"NansUMGExtent",
					"NansCommon", // TODO should remove this dependencies
					"Interactive"
				}
			);

			PrivateIncludePathModuleNames.AddRange(
				new[]
				{
					"MessageLog"
				}
			);
		}
	}
}
