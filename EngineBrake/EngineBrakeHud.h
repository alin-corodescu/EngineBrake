// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/HUD.h"
#include "EngineBrakeHud.generated.h"

/**
* Default HUD of the game
*/
UCLASS(config = Game)
class AEngineBrakeHud : public AHUD
{
	GENERATED_BODY()

public:
	AEngineBrakeHud();

	/*! Font used to render the vehicle info */
	UPROPERTY()
	UFont* HUDFont;

	UPROPERTY()
	FCanvasIcon EngineOnIcon; //!< Icon used to indicate the engine is running

	UPROPERTY()
	FCanvasIcon EngineOffIcon; //!< Icon used to indicate the engine is NOT running

	UPROPERTY()
	FCanvasIcon RPMMeterIcon; //!< Icon used to display the RPM meter

	// Begin AHUD interface
	virtual void DrawHUD() override;
	// End AHUD interface

private:
	//! Function used to draw the score on the screen
	void DrawScore();
	
	//! Functions used to draw the current Popup message on the screen
	void DrawPopupMessage();
	
	//! Function used to draw the current fuel level on the screen
	void DrawFuelLevel();

	//! Function used to draw the indicator whether the engine is running or not
	void DrawEngineIndicator();
	
	//! Function used to draw the current gear
	void DrawGear();

	//! Function used to draw the current speed of the vehicle
	void DrawSpeed();

	//! Function used to represent the RPM of the engine
	/*! Uses an icon asset for the RPM metter and draws a line to serve as indicator */
	void DrawRPM();
};
