// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "FuelSystemComponent.generated.h"

class AEngineBrakePawn;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ENGINEBRAKE_API UFuelSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFuelSystemComponent();

	//! Called after the constructor to set the fuel consumption coefficient and provide a reference to the Parent of this ActorComponent
	/**
	*	The fuel consumption is a linear function of the engine RPM, where the
	* RPM value is multiplied by the coefficient and the time elpased since the last Tick()
	*/
	void SetupConsumptionParameters(AEngineBrakePawn* MovementComponent,float Coefficient);

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;
	
	//! Returns the precentage of fuel remaining in the tank of the vehicle
	float GetFuelPrecentage();

	//! Resets the fuel level to full (100%) if no other precentage is passed as parameter
	void ResetFuelLevel(float Precetange = 100.f);

private:
	//! The current level of fuel of the vehicle
	float FuelLevel;

	//! The coefficient to multiply the engine RPM to compute instant consumption
	float ConsumptionCoefficient;

	//! Pointer to the Parent of this component
	AEngineBrakePawn* Parent;
};
