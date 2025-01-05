// Copyright Epic Games, Inc. All Rights Reserved.

#include "demo1GameMode.h"
#include "demo1Pawn.h"
#include "demo1Hud.h"

Ademo1GameMode::Ademo1GameMode()
{
	DefaultPawnClass = Ademo1Pawn::StaticClass();
	HUDClass = Ademo1Hud::StaticClass();
}
