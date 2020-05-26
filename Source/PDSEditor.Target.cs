// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class PDSEditorTarget : TargetRules
{
	public PDSEditorTarget(TargetInfo Target) : base(Target)
	{
		DefaultBuildSettings = BuildSettingsVersion.V2;
		Type = TargetType.Editor;

		ExtraModuleNames.AddRange( new string[] { "PDS" } );
	}
}
