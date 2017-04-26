// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/WheeledVehicle.h"
#include "VehicleAIPawn.generated.h"

/**
 * 
 */
UCLASS()
class ENGINEBRAKE_API AVehicleAIPawn : public AWheeledVehicle
{
	GENERATED_BODY()
public:
	AVehicleAIPawn();
	
	// Begin Actor interface
	virtual void Tick(float Delta) override;

	virtual void BeginPlay() override;
	// End Actor interface
	

};
