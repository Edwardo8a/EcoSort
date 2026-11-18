// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class EcoSort : ModuleRules
{
	public EcoSort(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate",
			"SlateCore"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"EcoSort",
			"EcoSort/Variant_Platforming",
			"EcoSort/Variant_Platforming/Animation",
			"EcoSort/Variant_Combat",
			"EcoSort/Variant_Combat/AI",
			"EcoSort/Variant_Combat/Animation",
			"EcoSort/Variant_Combat/Gameplay",
			"EcoSort/Variant_Combat/Interfaces",
			"EcoSort/Variant_Combat/UI",
			"EcoSort/Variant_SideScrolling",
			"EcoSort/Variant_SideScrolling/AI",
			"EcoSort/Variant_SideScrolling/Gameplay",
			"EcoSort/Variant_SideScrolling/Interfaces",
			"EcoSort/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
