// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/WheeledVehicle.h"
#include "EngineBrakePawn.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UTextRenderComponent;
class UInputComponent;
UCLASS(config=Game)
class AEngineBrakePawn : public AWheeledVehicle
{
	friend class ASpawner;
	friend class AVehicleAIPawn;
	friend class UFuelSystemComponent;
	friend class AEngineBrakeHud;
	GENERATED_BODY()

	//! Spring arm that will offset the camera
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArm;

	//! Camera component that will be our viewpoint
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;

	//! Audio component for the engine sound
	UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UAudioComponent* EngineSoundComponent;

	//! Pointer to the fuel system component used by this vehicle
	class UFuelSystemComponent* FuelSystem;

public:
	AEngineBrakePawn();

	//! The current speed as a string eg 10 km/h
	UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly)
	FText SpeedDisplayString;

	//! The current gear as a string (R,N, 1,2 etc)
	UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly)
	FText GearDisplayString;

	//! The current fuel precentage
	UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly)
	FText FuelPrecentageDisplayString;

	//! String of the current score
	UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly)
	FText ScoreDisplayString;

	//! The current pop-up to display
	UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly)
	FText PopupString;

	// Begin Pawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End Pawn interface

	// Begin Actor interface
	virtual void Tick(float Delta) override;
	virtual void BeginPlay() override;
	// End Actor interface

	/** Handle pressing forwards */
	void MoveForward(float Val);
	/** Handle pressing right */
	void MoveRight(float Val);
	/** Handle handbrake pressed */
	void OnHandbrakePressed();
	/** Handle handbrake released */
	void OnHandbrakeReleased();
	//! Called when the player requests fuel
	void OnFuelRequest();
	//! Function used to handle upshifting
	void OnUpShift();
	//! Function used to handle downshifting
	void OnDownShift();
	//! Function used to disengage any gear and put the car in neutral
	void OnNeutralGear();
	//! Function used to enter reverse gear
	void OnReverseGear();

	//! Callback functions used by the Fuel Component of this actor to signal lack of fuel
	void OutOfFuel();

	void SetSpawner(ASpawner* Spawner);

private:
	//! Float value which influences how fast the fuel is consumed
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float ConsumptionCoefficient = 0.00002f;

	//! Minimum threshold speeds for different gears
	/**!
	*	If the vehicle has is moving below the threshold of the gear engaged,
	*	the engine will stall, and the player will have to restart it manually
	*/
	int MinGearSpeeds[7] = { -1, -1, -1, 15, 25, 40, 70 };

	//! Flag specifying whether or not the engine is running
	/**!
	* Used to simulate real life stalling of the engine when the RPM is too low
	*/
	bool bRunningEngine;

	//! Flag specifying whether or not the vehicle is out of fuel
	bool bOutOfFuel;

	//! Is the vehicle in reverse gear?
	bool bInReverseGear;

	//! Reference to the Spawner Actor that spawned this class
	/*! Used to call the SpawnFuelPickup() from the input handler function */
	class ASpawner* SpawnerRef;

	//! Sound to be used when colliding with another vehicle
	USoundBase* ExplosionSound;

	//! Particle system to be used when colliding with antoher vehicle
	UParticleSystem* Explosion;

	//! Timer handle used to set a timer to clean up the pop up message after some time
	FTimerHandle PopupCleanerTimerHandle;

	//! Handles Collision logic, destroying the vehicle if it collides with another vehicle
	UFUNCTION()
	void OnCollision(UPrimitiveComponent * HitComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit);

	//! Handles Overlapping logic, refilling the fuel level when overlapping with a fuel pickup
	UFUNCTION()
	void OnOverlap(AActor* OverlappedActor, AActor* OtherActor);

	//! Updates the strings used by the HUD
	void UpdateHUDStrings();

	//! Sets the pop-up message to an empty string
	void ClearPopupMessage();

	//! Checks if the speed is below the current gear's threshold 
	/*!
	* Returns true if the speed is below the current gear's threshold, false otherwise
	* It only returns false if the vehicle is moving forward, to make the game feel more natural in case 
	* of a collision or when sliding down a slope
	*/
	bool CheckLowSpeedThreshold();

	//! Stalls the engine
	/*! Function called when the speed of the vehicle is too low for the current gear, to simulate real life behaviour */
	void StallEngine();

	//! Starts the engine
	void StartEngine();
};
