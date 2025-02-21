// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class LegendMiner_PickIdle : ModuleRules
{
	public LegendMiner_PickIdle(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(new string[]
        {
            "LegendMiner_PickIdle/Public",
            "LegendMiner_PickIdle/Public/Characters",
            "LegendMiner_PickIdle/Public/Components",
            "LegendMiner_PickIdle/Public/Data",
            "LegendMiner_PickIdle/Public/GameModes"
        });

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "Niagara", "AIModule", "NavigationSystem" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
