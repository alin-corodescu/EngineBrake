// Fill out your copyright notice in the Description page of Project Settings.

#include "EngineBrake.h"
#include "EngineBrakePawn.h"
#include "Vehicles/WheeledVehicleMovementComponent4W.h"
#include "FuelSystemComponent.h"

#define INITIAL_FUEL 1000.f

// Sets default values for this component's properties
UFuelSystemComponent::UFuelSystemComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UFuelSystemComponent::SetupConsumptionParameters(AEngineBrakePawn* Parent,float Coefficient)
{
	this->ConsumptionCoefficient = Coefficient;
	this->Parent = Parent;
}

// Called when the game starts
void UFuelSystemComponent::BeginPlay()
{
	Super::BeginPlay();
	UActorComponent::RegisterComponent();
	FuelLevel = INITIAL_FUEL;
	// ...
	
}


// Called every frame
void UFuelSystemComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if (Parent->bRunningEngine && FuelLevel > 0 && Parent->bHasThrottleInput)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Component Tick is actually being called"));

		UWheeledVehicleMovementComponent* MovementComponent = Parent->GetVehicleMovement();
		float InstantConsumption = ConsumptionCoefficient * MovementComponent->GetEngineRotationSpeed();

		FuelLevel -= InstantConsumption;

		if (FuelLevel < 0.0f)
			Parent->OutOfFuel();
	}
	// ...
}

float UFuelSystemComponent::GetFuelPrecentage()
{
	if (FuelLevel < 0)
		return 0.0f;
	return (FuelLevel / INITIAL_FUEL) * 100.f;
}

void UFuelSystemComponent::ResetFuelLevel(float Precetange)
{
	FuelLevel = (Precetange / 100.f) * INITIAL_FUEL;
}

