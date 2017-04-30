// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "EngineBrake.h"
#include "EngineBrakeGameMode.h"
#include "EngineBrakePawn.h"
#include "EngineBrakeHud.h"

AEngineBrakeGameMode::AEngineBrakeGameMode()
{
	DefaultPawnClass = AEngineBrakePawn::StaticClass();
	PlayerStateClass = APlayerState::StaticClass();
	HUDClass = AEngineBrakeHud::StaticClass();
}
