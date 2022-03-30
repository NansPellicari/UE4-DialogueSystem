// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

// ReSharper disable All

namespace UnrealBuildTool.Rules
{
	public class NansDialogueSystem : ModuleRules
	{
		public NansDialogueSystem(ReadOnlyTargetRules Target) : base(Target)
		{
			PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

			PublicDependencyModuleNames.AddRange(
				new[]
				{
					"Core", "AIModule"
				}
			);

			PrivateDependencyModuleNames.AddRange(
				new[]
				{
					"CoreUObject",
					"Engine",
					"AIModule",
					"UMG",
					"Slate",
					"SlateCore",
					"InputCore",
					"DeveloperSettings",
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