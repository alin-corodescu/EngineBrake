// Fill out your copyright notice in the Description page of Project Settings.

#include "EngineBrake.h"
#include "EndlessTrackGenerator.h"
#include "Classes/Components/PrimitiveComponent.h"
#include "Classes/Components/SplineComponent.h"
#include "Classes/Components/SplineMeshComponent.h"
#include "Classes/Engine/StaticMesh.h"




// Sets default values
AEndlessTrackGenerator::AEndlessTrackGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	RootComponent = SplineComponent;

	ConstructorHelpers::FObjectFinder<UStaticMesh> RoadMeshFinder(TEXT("StaticMesh'/Game/Props/RoadMesh.RoadMesh'"));

	ConstructorHelpers::FObjectFinder<UStaticMesh> RightGuardRailMeshFinder(TEXT("StaticMesh'/Game/Props/R_GuardRail.R_GuardRail'"));
	RightGuardRailMesh = RoadMeshFinder.Object;
	ConstructorHelpers::FObjectFinder<UStaticMesh> LeftGuardRailMeshFinder(TEXT("StaticMesh'/Game/Props/L_GuardRail.L_GuardRail'"));
	LeftGuardRailMesh = RoadMeshFinder.Object;
}

// Called when the game starts or when spawned
void AEndlessTrackGenerator::BeginPlay()
{
	Super::BeginPlay();

	SplineComponent->SetDrawDebug(true);
	TArray<FVector> SplinePointsLocation;
	SplinePointsLocation.SetNum(InitialSplinePoints);
	SplinePointsLocation[0] = FVector(0, 0, 0);
	for (int i = 1; i < InitialSplinePoints; i++)
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

	for (int i = 0; i < SplinePointsNumber - 1; i++)
	{
		BuildTrackElement(RoadMesh, i);
		BuildTrackElement(LeftGuardRailMesh, i);
		BuildTrackElement(RightGuardRailMesh, i);
	}
}

void AEndlessTrackGenerator::BuildTrackElement(UStaticMesh * TrackElementMesh, int SplinePointIndex)
{
	int NextSplinePointIndex = SplinePointIndex + 1;
	FVector StartLocation, EndLocation, StartTangent, EndTangent;
	SplineComponent->GetLocalLocationAndTangentAtSplinePoint
	(
		SplinePointIndex,
		StartLocation,
		StartTangent
	);

	SplineComponent->GetLocalLocationAndTangentAtSplinePoint
	(
		NextSplinePointIndex,
		EndLocation,
		EndTangent
	);

	//UE_LOG(LogTemp, Warning, TEXT("Setting static mesh"));

	USplineMeshComponent* SplineMesh = ConstructObject<USplineMeshComponent>(USplineMeshComponent::StaticClass(), this);
	SplineMesh->SetMobility(EComponentMobility::Movable);
	SplineMesh->AttachParent = SplineComponent;
	SplineMesh->SetCollisionProfileName(FName("BlockAll"));
	SplineMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SplineMesh->CreationMethod = EComponentCreationMethod::UserConstructionScript;
	SplineMesh->SetStaticMesh(TrackElementMesh);
	SplineMesh->SetStartAndEnd(StartLocation, StartTangent, EndLocation, EndTangent, true);

	FinishAndRegisterComponent(SplineMesh);
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

	BuildTrack();
}