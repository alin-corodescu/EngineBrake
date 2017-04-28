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
private:
	//! Pawn sensing component used to sense the player / other vehicles
	UPROPERTY(VisibleAnywhere, Category = "AI")
	class UPawnSensingComponent* PawnSensingComp;

	UPROPERTY(EditAnywhere)
	class USplineComponent* SplineToFollow;


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

	//! Getter for the SplineToFollow
	USplineComponent* GetFollowedPath();

	//! Setter for the spline path to folow
	void SetPathToFollow(USplineComponent* Spline);

	UPROPERTY(EditAnywhere)
	float LookAheadSeconds = 1;
};
