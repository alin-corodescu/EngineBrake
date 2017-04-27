// Fill out your copyright notice in the Description page of Project Settings.

#include "EngineBrake.h"
#include "EndlessTrackGenerator.h"
#include "Classes/Components/PrimitiveComponent.h"
#include "Classes/Components/SplineComponent.h"
#include "Classes/Components/SplineMeshComponent.h"
#include "Classes/Engine/StaticMesh.h"
#include "SplineMeshesComponentsPool.h"



// Sets default values
AEndlessTrackGenerator::AEndlessTrackGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	RootComponent = SplineComponent;

	ConstructorHelpers::FObjectFinder<UStaticMesh> RoadMeshFinder(TEXT("StaticMesh'/Game/Props/RoadMesh.RoadMesh'"));
	RoadMesh = RoadMeshFinder.Object;
	ConstructorHelpers::FObjectFinder<UStaticMesh> RightGuardRailMeshFinder(TEXT("StaticMesh'/Game/Props/R_GuardRail.R_GuardRail'"));
	RightGuardRailMesh = RightGuardRailMeshFinder.Object;
	ConstructorHelpers::FObjectFinder<UStaticMesh> LeftGuardRailMeshFinder(TEXT("StaticMesh'/Game/Props/L_GuardRail.L_GuardRail'"));
	LeftGuardRailMesh = LeftGuardRailMeshFinder.Object;

	SplineMeshesPool = new SplineMeshesComponentsPool();
	SplineMeshesPool->CreateComponents(this, NumberOfSplinePoints - 1, RoadMesh, RightGuardRailMesh, LeftGuardRailMesh);
}

// Called when the game starts or when spawned
void AEndlessTrackGenerator::BeginPlay()
{
	Super::BeginPlay();

	SplineComponent->SetDrawDebug(true);
	TArray<FVector> SplinePointsLocation;
	SplinePointsLocation.SetNum(NumberOfSplinePoints);
	SplinePointsLocation[0] = FVector(0, 0, 0);
	for (int i = 1; i < NumberOfSplinePoints; i++)
	{
		SplinePointsLocation[i] = SplinePointsLocation[i - 1];
		SplinePointsLocation[i].X += DistanceBetweenPoints;
	}

	// Spawn the inital spline points
	SplineComponent->SetSplineLocalPoints(SplinePointsLocation);
	
	BuildTrack();
}

// Called every frame
void AEndlessTrackGenerator::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	
	if (GetWorld()->TimeSince(LastRebuild) > RebuildingInterval)
	{
		RebuildTrack();
	}
}

void AEndlessTrackGenerator::BuildTrack()
{	
	LastRebuild = GetWorld()->TimeSeconds;
	// Store number of spline points
	int SplinePointsNumber = SplineComponent->GetNumberOfSplinePoints();
	TArray<FVector> Locations, Tangents;
	Locations.SetNum(NumberOfSplinePoints);
	Tangents.SetNum(NumberOfSplinePoints);
	for (int i = 0; i < SplinePointsNumber; i++)
		// Construct the TArray with initial positions and tangents
		SplineComponent->GetLocalLocationAndTangentAtSplinePoint(i, Locations[i], Tangents[i]);

	SplineMeshesPool->SetInitialPositions(Locations, Tangents);
}

void AEndlessTrackGenerator::RebuildTrack()
{
	UE_LOG(LogTemp, Warning, TEXT("Rebuild track called"));
	int NumberOfSplinePoints = SplineComponent->GetNumberOfSplinePoints();
	TArray<FVector> SplinePointsLocation;
	SplinePointsLocation.SetNum(NumberOfSplinePoints);

	for (int i = 0; i < NumberOfSplinePoints - 1; i++)
		SplinePointsLocation[i] = SplineComponent->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::Local);

	SplinePointsLocation[NumberOfSplinePoints - 1] = SplinePointsLocation[NumberOfSplinePoints - 2];
	SplinePointsLocation[NumberOfSplinePoints - 1].X += DistanceBetweenPoints;

	SplineComponent->SetSplineLocalPoints(SplinePointsLocation);
	FVector Location, Tangent;
	SplineComponent->GetLocalLocationAndTangentAtSplinePoint(NumberOfSplinePoints - 1, Location, Tangent);

	SplineMeshesPool->UpdateEnds(Location, Tangent);

	LastRebuild = GetWorld()->TimeSeconds;
}