// Fill out your copyright notice in the Description page of Project Settings.

#include "EngineBrake.h"
#include "Spawner.h"
#include "EngineBrakeGameMode.h"
#include "RoadConstruction/EndlessTrackGenerator.h"
#include "EngineBrakePawn.h"
#include "FuelSystem/FuelPickup.h"
#include "Classes/Components/SplineComponent.h"
#include "AI/VehicleAIPawn.h"
#include "Score/ScoreCalculator.h"
#include "PawnFollowingKillZVolume.h"

#define FUEL_OFFSET 10000.0f
#define VEHICLE_OFFSET 20000.0f
#define VEHICLE_Z_OFFSET 1500.0f
#define DISTANCE_THRESHOLD 1000.0f

void ASpawner::SpawnFuelPickup()
{
	// Generate a new Spawning Location
	FVector Location = GenerateSpawningLocation(FUEL_OFFSET);

	ScoreCalculator* Calculator = ScoreCalculator::GetInstance();

	// Updates the Score Multiplier and game speed
	Calculator->ScoreMultiplier += 0.3f;
	TrackGenerator->RebuildingInterval -= 0.5f;
	if (TrackGenerator->RebuildingInterval < 0.5f)
		TrackGenerator->RebuildingInterval = 0.5f;

	// Spawn the new Fuel pickup
	World->SpawnActor<AFuelPickup>(Location, ComputeRotation(Location));
}

void ASpawner::SpawnAIVehicle()
{

	FVector Location = GenerateSpawningLocation(VEHICLE_OFFSET);

	// Spawn it higher above the ground
	Location.Z += VEHICLE_Z_OFFSET;

	// Spawn the new actor
	AVehicleAIPawn* AIVehicle = World->SpawnActor<AVehicleAIPawn>(Location, ComputeRotation(Location));

	// Set it's path to be followed
	AIVehicle->SetPathToFollow(TrackGenerator);

	// Set a timer to spawn a new Vehicle 
	World->GetTimerManager().ClearTimer(AISpawning_Timer);
	World->GetTimerManager().SetTimer(AISpawning_Timer, this, &ASpawner::SpawnAIVehicle, FMath::RandRange(20, 40));
}


FVector ASpawner::GenerateSpawningLocation(float Offset)
{
	FVector Location;
	FVector PlayerLocation = PlayerPawn->GetActorLocation();
	USplineComponent* Spline = TrackGenerator->GetSplineComponent();
	FVector ForwardVector = PlayerPawn->GetActorForwardVector();

	Location = PlayerLocation + ForwardVector * Offset;

	Location = Spline->FindLocationClosestToWorldLocation(Location, ESplineCoordinateSpace::World);

	return Location;
}

FRotator ASpawner::ComputeRotation(FVector Location)
{
	FRotator result = TrackGenerator->GetSplineComponent()->FindRotationClosestToWorldLocation(Location, ESplineCoordinateSpace::World);
	return result;
}


// Sets default values
ASpawner::ASpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASpawner::BeginPlay()
{
	Super::BeginPlay();

	World = GetWorld();

	// Spawn the Track generator
	TrackGenerator = World->SpawnActor<AEndlessTrackGenerator>(
		FVector(0, 0, 0), FRotator(0, 0, 0));
 
	// Spawn a new Player controlled vehicle
	PlayerPawn = Cast<AEngineBrakePawn>(UGameplayStatics::GetPlayerPawn(World, 0));
	PlayerPawn->SetActorLocation(FVector(5000, 0, 200), false);
	PlayerPawn->SetActorRotation(FRotator::ZeroRotator);

	PlayerPawn->SetSpawner(this);

	// Spawn the KillZVolume
	APawnFollowingKillZVolume* KillZVolume = World->SpawnActor<APawnFollowingKillZVolume>(
		FVector(0, 0, -10000), FRotator::ZeroRotator);

	// Spawn the first AI vehicle
	World->GetTimerManager().SetTimer(AISpawning_Timer, this, &ASpawner::SpawnAIVehicle, FMath::RandRange(2, 3));

}

// Called every frame
void ASpawner::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	// Check if the pawn is still on track
	FVector SplineLocation = TrackGenerator->GetSplineComponent()->FindLocationClosestToWorldLocation(PlayerPawn->GetActorLocation(),ESplineCoordinateSpace::World);
	float Distance = FVector::Dist(SplineLocation, PlayerPawn->GetActorLocation());

	// If the pawn is too far away from the spline
	if (Distance > DISTANCE_THRESHOLD)
	{
		// Initiate a game over sequence
		AEngineBrakeGameMode* GameMode = Cast<AEngineBrakeGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		GameMode->GameOver();
	}
}

