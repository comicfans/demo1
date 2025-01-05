// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/HUD.h"
#include "demo1Hud.generated.h"


UCLASS(config = Game)
class Ademo1Hud : public AHUD
{
	GENERATED_BODY()

public:
	Ademo1Hud();

	/** Font used to render the vehicle info */
	UPROPERTY()
	UFont* HUDFont;

	// Begin AHUD interface
	virtual void DrawHUD() override;
	// End AHUD interface
};
