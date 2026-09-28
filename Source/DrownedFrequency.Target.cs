using UnrealBuildTool;
public class DrownedFrequencyTarget : TargetRules
{
    public DrownedFrequencyTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.Add("DrownedFrequency");
    }
}
