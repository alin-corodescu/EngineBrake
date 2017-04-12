// Fill out your copyright notice in the Description page of Project Settings.

#include "EngineBrake.h"
#include "RoadData.h"

FRoadData::FRoadData()
{
	LeftGuardRail = RightGuardRail = false;
	TrackWidth = TrackThickness = 1.0f;
	TrackBank = 0.0f;
}
