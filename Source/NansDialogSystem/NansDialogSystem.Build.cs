// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
	public class NansDialogSystem : ModuleRules
	{
		public NansDialogSystem(ReadOnlyTargetRules Target) : base(Target)
		{
			// This to allow dynamic_cast
			// https://answers.unrealengine.com/questions/477792/how-do-i-cast-between-polymorphic-classes-that-don.html?sort=oldest
			bUseRTTI = true;

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
					"NansCoreHelpers",
					"NansUE4Utilities",
					"NansFactorsFactoryCore",
					"NansFactorsFactoryUE4",
					"NansBehaviorSteps",
					"NansExtension",
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
