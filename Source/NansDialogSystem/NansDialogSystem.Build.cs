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
					"Engine",
					"UnrealEd",
					"BlueprintGraph",
					"GraphEditor",
					"PropertyEditor",
					"AIModule",
					"GameplayTasks",
					"UMG",
					"Slate",
					"SlateCore",
					"InputCore",
					"Kismet",
					"KismetCompiler",
					"NansCoreHelpers",
					"NansUE4Utilities",
					"NansFactorsFactoryCore",
					"NansFactorsFactoryUE4",
					"NansBehaviorSteps",
					"NansExtension",
					"NansCommon",
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
