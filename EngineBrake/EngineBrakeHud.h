// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/HUD.h"
#include "EngineBrakeHud.generated.h"


UCLASS(config = Game)
class AEngineBrakeHud : public AHUD
{
	GENERATED_BODY()

public:
	AEngineBrakeHud();

	/** Font used to render the vehicle info */
	UPROPERTY()
	UFont* HUDFont;

	// Begin AHUD interface
	virtual void DrawHUD() override;
	// End AHUD interface
};
