using UnrealBuildTool;
using System.Collections.Generic;

public class PresentationEditorTarget : TargetRules
{
	public PresentationEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V6;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("Presentation");
	}
}
