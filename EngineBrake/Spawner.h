// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Spawner.generated.h"
/**
*	Class used to handle the logic of spawning AI vehicles
*/
UCLASS()
class ENGINEBRAKE_API ASpawner : public AActor
{
	GENERATED_BODY()
	class AEndlessTrackGenerator* TrackGenerator;

	class AEngineBrakePawn* PlayerPawn;

	void SpawnAIVehicle();
	
	//! Generates a spawning location in relation with the current player location
	FVector GenerateSpawningLocation(float Offset);

	class UWorld* World;	

	//! TimerHandle for AI Vehicle spawning timer
	FTimerHandle AISpawning_Timer;
public:	
	// Sets default values for this actor's properties
	ASpawner();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	void SpawnFuelPickup();
};
