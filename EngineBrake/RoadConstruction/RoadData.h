// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "RoadData.generated.h"
/**
 * 
 */
USTRUCT(BlueprintType)
struct FRoadData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	bool LeftGuardRail;

	UPROPERTY(EditAnywhere)
	bool RightGuardRail;

	UPROPERTY(EditAnywhere)
	float TrackBank;

	UPROPERTY(EditAnywhere)
	float TrackWidth;

	UPROPERTY(EditAnywhere)
	float TrackThickness;

	FRoadData();
};
