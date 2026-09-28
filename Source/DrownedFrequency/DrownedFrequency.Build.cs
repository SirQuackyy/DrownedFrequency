using UnrealBuildTool;
public class DrownedFrequency : ModuleRules
{
    public DrownedFrequency(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicIncludePaths.Add(ModuleDirectory);
        PublicDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "GameplayTags" });
        PrivateDependencyModuleNames.AddRange(new[] { "UMG", "Slate", "SlateCore" });
        if (Target.Configuration != UnrealTargetConfiguration.Shipping) PrivateDependencyModuleNames.Add("AudioMixer");
        if (Target.bBuildEditor) PrivateDependencyModuleNames.Add("UnrealEd");
    }
}
