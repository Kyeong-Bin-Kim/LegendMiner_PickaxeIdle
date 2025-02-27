// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class LegendMiner_PickIdle : ModuleRules
{
	public LegendMiner_PickIdle(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(new string[]
         {
            Path.Combine(ModuleDirectory, "Public"),
            Path.Combine(ModuleDirectory, "Public/Characters"),
            Path.Combine(ModuleDirectory, "Public/Components"),
            Path.Combine(ModuleDirectory, "Public/Data"),
            Path.Combine(ModuleDirectory, "Public/GameModes"),
            Path.Combine(ModuleDirectory, "Public/Objects"),
            Path.Combine(ModuleDirectory, "Public/SaveSystem"),
            Path.Combine(ModuleDirectory, "Public/UserWidget")
        });

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "Niagara", "AIModule", "NavigationSystem", "UMG" });

        PrivateDependencyModuleNames.AddRange(new string[] {    });

        // Uncomment if you are using Slate UI
        //PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore"});

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
