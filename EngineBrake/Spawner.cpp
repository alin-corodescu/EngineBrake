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
#define VEHICLE_Z_OFFSET 200
#define DISTANCE_THRESHOLD 5000.0f
void ASpawner::SpawnFuelPickup()
{
	// Spawn a Fuel pickup 
	FVector Location = GenerateSpawningLocation(FUEL_OFFSET);
	//
	ScoreCalculator* Calculator = ScoreCalculator::GetInstance();

	Calculator->ScoreMultiplier += 0.3f;
	TrackGenerator->RebuildingInterval -= 0.5f;
	if (TrackGenerator->RebuildingInterval < 0.5f)
		TrackGenerator->RebuildingInterval = 0.5f;

	World->SpawnActor<AFuelPickup>(Location, ComputeRotation(Location));
}

void ASpawner::SpawnAIVehicle()
{
	FVector Location = GenerateSpawningLocation(VEHICLE_OFFSET);
	// Spawn it higher so it doesn't spawn inside the road mesh
	Location.Z += VEHICLE_Z_OFFSET;
	//int RandomRotationChooser = FMath::RandHelper(2);
	//FRotator Rotation(0, RandomRotationChooser * 180, 0);

	Location.Z += 1500;

	AVehicleAIPawn* AIVehicle = World->SpawnActor<AVehicleAIPawn>(Location, ComputeRotation(Location));

	AIVehicle->SetPathToFollow(TrackGenerator);

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
	// Spawn a new Endless Road Generator
	World = GetWorld();
	if (World)
	{
		UE_LOG(LogTemp, Warning, TEXT("World is set up correctly"));
	}
	TrackGenerator = World->SpawnActor<AEndlessTrackGenerator>(
		FVector(0, 0, 0), FRotator(0, 0, 0));

	/*World->SpawnActor<APlayerStart>
		(FVector(2000, 0, 200), FRotator(0, 0, 0));*/
 
	PlayerPawn = Cast<AEngineBrakePawn>(UGameplayStatics::GetPlayerPawn(World, 0));
	PlayerPawn->SetActorLocation(FVector(5000, 0, 200), false);
	PlayerPawn->SetActorRotation(FRotator::ZeroRotator);

	APawnFollowingKillZVolume* KillZVolume = World->SpawnActor<APawnFollowingKillZVolume>(
		FVector(0, 0, -10000), FRotator::ZeroRotator);

	PlayerPawn->SetSpawner(this);

	//UE_LOG(LogTemp, Warning, TEXT("PlayerPawn was spawned"));

	World->GetTimerManager().SetTimer(AISpawning_Timer, this, &ASpawner::SpawnAIVehicle, FMath::RandRange(2, 3));

}

// Called every frame
void ASpawner::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	// Check if the pawn is still on track
	FVector SplineLocation = TrackGenerator->GetSplineComponent()->FindLocationClosestToWorldLocation(PlayerPawn->GetActorLocation(),ESplineCoordinateSpace::World);
	float Distance = FVector::Dist(SplineLocation, PlayerPawn->GetActorLocation());
	//UE_LOG(LogTemp, Warning, TEXT("Distance is %f:"),Distance);
	if (Distance > DISTANCE_THRESHOLD)
	{
		// Initiate a game over sequence
		AEngineBrakeGameMode* GameMode = Cast<AEngineBrakeGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		static FTimerHandle GameOverDelayer;
		GetWorld()->GetTimerManager().SetTimer(GameOverDelayer, GameMode, &AEngineBrakeGameMode::GameOver, 2.0f);
	}
}

