using UnrealBuildTool;
public class DrownedFrequencyEditorTarget : TargetRules
{
    public DrownedFrequencyEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.Add("DrownedFrequency");
    }
}
