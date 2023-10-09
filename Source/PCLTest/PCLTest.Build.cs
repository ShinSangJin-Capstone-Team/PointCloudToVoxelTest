// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PCLTest : ModuleRules
{
	public PCLTest(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "Open3DUE5", "Voxel" , "Open3D"});

		PrivateDependencyModuleNames.AddRange(new string[] {  });

        bUseRTTI = true;

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
