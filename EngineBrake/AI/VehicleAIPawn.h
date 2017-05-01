// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/WheeledVehicle.h"
#include "VehicleAIPawn.generated.h"

/**
 * Class for AI controlled vehicles in the game
 */
UCLASS()
class ENGINEBRAKE_API AVehicleAIPawn : public AWheeledVehicle
{
	GENERATED_BODY()
private:
	//! Pawn sensing component used to sense the player / other vehicles
	UPROPERTY(VisibleAnywhere, Category = "AI")
	class UPawnSensingComponent* PawnSensingComp;

	//! Reference to the Actor generating the Track, required for AI calculations of steering and throttle
	UPROPERTY(EditAnywhere)
	class AEndlessTrackGenerator* RoadToFollow;

	//! Audio component for the engine sound
	UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* EngineSoundComponent;

	//! Function to be called when colliding with another object
	UFUNCTION()
	void OnHit(UPrimitiveComponent * HitComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit);

	//! Sound to play when colliding with another vehicle
	USoundBase* ExplosionSound = NULL;

	//! Particle System to be used when colliding with another vehicle
	UParticleSystem* Explosion = NULL;

public:
	AVehicleAIPawn();

	// Begin Actor interface
	virtual void Tick(float Delta) override;

	virtual void BeginPlay() override;
	// End Actor interface
	
	//! Function called when another vehicle has been detected
	UFUNCTION()
	void OnDetectVehicle(APawn* Vehicle);

	//! Reference to the behaviour tree used by this vehicle
	UPROPERTY(VisibleAnywhere, Category = "AI")
	class UBehaviorTree* BehaviorTree;

	//! Getter for the Actor generating the track
	AEndlessTrackGenerator* GetFollowedPath();

	//! Setter for the Actor generating the track
	void SetPathToFollow(AEndlessTrackGenerator* RoadGenerator);

	//! How far ahead (in time) to look ahead when predicting the future location for steering and throttle input.
	UPROPERTY(EditAnywhere)
	float LookAheadMiliSeconds = 1000;
};
