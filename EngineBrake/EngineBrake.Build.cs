// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class EngineBrake : ModuleRules
{
	public EngineBrake(TargetInfo Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "AIModule",
        "GameplayTasks"/* Added to solve a linkage error for 4.12 version of the engine */});
	}
}
