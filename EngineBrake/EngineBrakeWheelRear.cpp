// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "EngineBrake.h"
#include "EngineBrakeWheelRear.h"

UEngineBrakeWheelRear::UEngineBrakeWheelRear()
{
	ShapeRadius = 35.f;
	ShapeWidth = 10.0f;
	bAffectedByHandbrake = true;
	SteerAngle = 0.f;
}
