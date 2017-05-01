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
	GENERATED_BODY()

	/** Spring arm that will offset the camera */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArm;

	/** Camera component that will be our viewpoint */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;

	/** SCene component for the In-Car view origin */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* InternalCameraBase;

	/** Camera component for the In-Car view */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* InternalCamera;

	/** Text component for the In-Car speed */
	UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UTextRenderComponent* InCarSpeed;

	/** Text component for the In-Car gear */
	UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UTextRenderComponent* InCarGear;

	/** For some reason modifications to this in the editor aren't reflected in the game */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float ConsumptionCoefficient = 0.00002f;

	//! Pointer to the fuel system component used by this
	class UFuelSystemComponent* FuelSystem;



	friend class UFuelSystemComponent;

	class ASpawner* SpawnerRef;

	USoundBase* ExplosionSound;

	UParticleSystem* Explosion;
	
public:
	AEngineBrakePawn();

	/** The current speed as a string eg 10 km/h */
	UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly)
	FText SpeedDisplayString;

	/** The current gear as a string (R,N, 1,2 etc) */
	UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly)
	FText GearDisplayString;

	/** The current RPM as a string */
	UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly)
	FText RPMDisplayString;

	/** The current fuel precentage */
	UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly)
	FText FuelPrecentageDisplayString;

	/** The current score */
	UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly)
	FText ScoreDisplayString;

	/** The current pop-up to display */
	UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly)
	FText PopupString;

	UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly)
	/** The color of the incar gear text in forward gears */
	FColor	GearDisplayColor;

	/** The color of the incar gear text when in reverse */
	UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly)
	FColor	GearDisplayReverseColor;


	/** Are we using incar camera */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly)
	bool bInCarCameraActive;

	/** Are we in reverse gear */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly)
	bool bInReverseGear;

	/** Initial offset of incar camera */
	FVector InternalCameraOrigin;

	FTimerHandle PopupCleanerTimerHandle;

	bool bHasThrottleInput;
	// Begin Pawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End Pawn interface

	// Begin Actor interface
	virtual void Tick(float Delta) override;
	virtual void BeginPlay() override;
	// End Actor interface

	/** Handle pressing forwards */
	void MoveForward(float Val);
	/** Setup the strings used on the hud */
	void SetupInCarHUD();

	/** Handle pressing right */
	void MoveRight(float Val);
	/** Handle handbrake pressed */
	void OnHandbrakePressed();
	/** Handle handbrake released */
	void OnHandbrakeReleased();
	/** Switch between cameras */
	void OnToggleCamera();

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

	static const FName LookUpBinding;
	static const FName LookRightBinding;

	void SetSpawner(ASpawner* Spawner);

	//! Flag specifying whether or not the engine is running
	/**
	* Used to simulate real life stalling of the engine when the RPM is too low
	*/
	bool bRunningEngine;

private:
	//! Minimum threshold speeds for different gears
	/**
	*	If the vehicle has is moving below the threshold of the gear engaged,
	*	the engine will stall, and the player will have to restart it manually
	*/
	int MinGearSpeeds[7] = { -1, -1, -1, 15, 25, 40, 70 };
	/** 
	 * Activate In-Car camera. Enable camera and sets visibility of incar hud display
	 *
	 * @param	bState true will enable in car view and set visibility of various if its doesnt match new state
	 * @param	bForce true will force to always change state
	 */
	void EnableIncarView( const bool bState, const bool bForce = false );

	/** Update the gear and speed strings */
	void UpdateHUDStrings();

	/* Are we on a 'slippery' surface */
	bool bIsLowFriction;

	void ClearPopupMessage();

	//! Checks if the speed is below the current gear's threshold 
	/*
	* Returns true if the speed is below the current gear's threshold, false otherwise
	*/
	bool CheckLowSpeedThreshold();

	void StallEngine();

	void StartEngine();

	bool bOutOfFuel;

	UFUNCTION()
	void OnCollision(UPrimitiveComponent * HitComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit);

	UFUNCTION()
	void OnOverlap(AActor* OverlappedActor, AActor* OtherActor);

	/** Audio component for the engine sound */
	UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UAudioComponent* EngineSoundComponent;

	bool bOnTrack;


public:
	/** Returns SpringArm subobject **/
	FORCEINLINE USpringArmComponent* GetSpringArm() const { return SpringArm; }
	/** Returns Camera subobject **/
	FORCEINLINE UCameraComponent* GetCamera() const { return Camera; }
	/** Returns InternalCamera subobject **/
	FORCEINLINE UCameraComponent* GetInternalCamera() const { return InternalCamera; }
	/** Returns InCarSpeed subobject **/
	FORCEINLINE UTextRenderComponent* GetInCarSpeed() const { return InCarSpeed; }
	/** Returns InCarGear subobject **/
	FORCEINLINE UTextRenderComponent* GetInCarGear() const { return InCarGear; }
};
