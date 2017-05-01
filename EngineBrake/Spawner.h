// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Spawner.generated.h"
/**
*	Class used to handle the logic of spawning other actors
*/
UCLASS()
class ENGINEBRAKE_API ASpawner : public AActor
{
	GENERATED_BODY()
	//! Pointer to the Track generator spawned by this actor
	class AEndlessTrackGenerator* TrackGenerator;

	//! Pointer to the player controlled vehicle spawned by this actor
	class AEngineBrakePawn* PlayerPawn;
	
	//! Pointer to the World in which to spawn the actors
	class UWorld* World;

	//! TimerHandle for AI Vehicle spawning timer
	FTimerHandle AISpawning_Timer;

	//! Spawns a new AI vehicle on the track, ahead of the player pawn
	void SpawnAIVehicle();
	
	//! Generates a spawning location in relation with the current player location
	/*! Helper function used to find a suitable spawning location along the spline */
	FVector GenerateSpawningLocation(float Offset);

	//! Computes the rotation of the Spline on which the track is based in a location
	/*! Used to properly rotate newly spawned elements */
	FRotator ComputeRotation(FVector Location);


public:	
	//! Sets default values for this actor's properties
	ASpawner();

	//! Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	//! Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	//! Called by the player pawn when receiving the specific input for this action, spawns a fuel pickup
	void SpawnFuelPickup();
};
