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
	UPROPERTY()
	FCanvasIcon EngineOnIcon; //!< Icon used to indicate the engine is running

	UPROPERTY()
	FCanvasIcon EngineOffIcon; //!< Icon used to indicate the engine is NOT running

	UPROPERTY()
	FCanvasIcon RPMMeterIcon; //!< Icon used to display the RPM meter
private:
	void DrawScore();
	void DrawPopupMessage();
	void DrawFuelLevel();
	void DrawEngineIndicator();
	void DrawGear();
	void DrawSpeed();
	void DrawRPM();
};
