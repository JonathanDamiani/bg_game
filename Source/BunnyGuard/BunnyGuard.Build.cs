// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BunnyGuard : ModuleRules
{
	public BunnyGuard(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "UMG", "OnlineSubsystem", "OnlineSubsystemSteam", "NavigationSystem", "AIModule", "Http", "Json", "JsonUtilities", "AkAudio" });

		PrivateDependencyModuleNames.AddRange(new string[] { "GameplayAbilities", "GameplayTags", "GameplayTasks", "Slate", "SlateCore", "Niagara" });

	}
}
