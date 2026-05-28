using UnrealBuildTool;
using System.Collections.Generic;

public class PresentationTarget : TargetRules
{
	public PresentationTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V6;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("Presentation");
	}
}
