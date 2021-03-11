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
					"UMG",
					"Slate",
					"SlateCore",
					"InputCore",
					"DeveloperSettings",
					"NansBehaviorStepsCore",
					"NansBehaviorSteps",
					"GameplayTasks",
					"GameplayAbilities",
					"GameplayTags",
					"NansCoreHelpers",
					"NansUE4Utilities",
					"NansUMGExtent"
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
