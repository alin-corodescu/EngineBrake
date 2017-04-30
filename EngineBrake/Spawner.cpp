// Fill out your copyright notice in the Description page of Project Settings.

#include "EngineBrake.h"
#include "Spawner.h"
#include "RoadConstruction/EndlessTrackGenerator.h"
#include "EngineBrakePawn.h"
#include "FuelSystem/FuelPickup.h"
#include "Classes/Components/SplineComponent.h"
#include "AI/VehicleAIPawn.h"

#define FUEL_OFFSET 10000.0f
#define VEHICLE_OFFSET 20000.0f
#define VEHICLE_Z_OFFSET 200
void ASpawner::SpawnFuelPickup()
{
	// Spawn a Fuel pickup 
	FVector Location = GenerateSpawningLocation(FUEL_OFFSET);

	World->SpawnActor<AFuelPickup>(Location, FRotator::ZeroRotator);
}

void ASpawner::SpawnAIVehicle()
{
	FVector Location = GenerateSpawningLocation(VEHICLE_OFFSET);
	// Spawn it higher so it doesn't spawn inside the road mesh
	Location.Z += VEHICLE_Z_OFFSET;
	int RandomRotationChooser = FMath::RandHelper(2);
	FRotator Rotation(0, RandomRotationChooser * 180, 0);

	AVehicleAIPawn* AIVehicle = World->SpawnActor<AVehicleAIPawn>(Location, Rotation);

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

// Sets default values
ASpawner::ASpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

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

	PlayerPawn->SetSpawner(this);

	//UE_LOG(LogTemp, Warning, TEXT("PlayerPawn was spawned"));

	World->GetTimerManager().SetTimer(AISpawning_Timer, this, &ASpawner::SpawnAIVehicle, FMath::RandRange(2, 3));

}

// Called every frame
void ASpawner::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

