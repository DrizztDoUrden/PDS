// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class PDSTarget : TargetRules
{
	public PDSTarget(TargetInfo Target) : base(Target)
	{
		DefaultBuildSettings = BuildSettingsVersion.V2;
		Type = TargetType.Game;

		ExtraModuleNames.AddRange( new string[] { "PDS" } );
	}
}
