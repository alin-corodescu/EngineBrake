// Fill out your copyright notice in the Description page of Project Settings.

#include "EngineBrake.h"
#include "EndlessTrackGenerator.h"
#include "Classes/Components/PrimitiveComponent.h"
#include "Classes/Components/SplineComponent.h"
#include "Classes/Components/SplineMeshComponent.h"
#include "Classes/Engine/StaticMesh.h"
#include "SplineMeshesComponentsPool.h"

#define THRESHOLD 0.15f
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

USplineComponent * AEndlessTrackGenerator::GetSplineComponent()
{
	return SplineComponent;
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
	//UE_LOG(LogTemp, Warning, TEXT("Rebuild track called"));
	int NumberOfSplinePoints = SplineComponent->GetNumberOfSplinePoints();
	TArray<FVector> SplinePointsLocation;
	SplinePointsLocation.SetNum(NumberOfSplinePoints);

	for (int i = 0; i < NumberOfSplinePoints - 1; i++)
		SplinePointsLocation[i] = SplineComponent->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::Local);

	// Here handle the logic of generating the next point

	SplinePointsLocation[NumberOfSplinePoints - 1] = GenerateNextPoint();

	SplineComponent->SetSplineLocalPoints(SplinePointsLocation);
	FVector Location, Tangent;
	SplineComponent->GetLocalLocationAndTangentAtSplinePoint(NumberOfSplinePoints - 1, Location, Tangent);

	SplineMeshesPool->UpdateEnds(Location, Tangent);

	LastRebuild = GetWorld()->TimeSeconds;
}

FVector AEndlessTrackGenerator::GenerateNextPoint()
{
	static int tester = 0;
	tester++;
	enum Directions 
	{
		Straight, Left, Right, Up, Down
	};
	// Would be more ellegant with a map, but in a hurry
	float UpDownAngle[5] = { 0.0f, 0.0f, 0.0f, UpDownAngleLimit, -UpDownAngleLimit };
	float LeftRightAngle[5] = { 0.0f, LeftRightAngleLimit, -LeftRightAngleLimit, 0.0f, 0.0f };
	int Index = SplineComponent->GetNumberOfSplinePoints() - 1;
	FVector UpVector, RightVector, ForwardVector;
	UpVector = SplineComponent->GetUpVectorAtSplinePoint(Index, ESplineCoordinateSpace::Local);
	RightVector = SplineComponent->GetRightVectorAtSplinePoint(Index, ESplineCoordinateSpace::Local);
	// Forward vector is the cross product of up and right vectors
	ForwardVector = -FVector::CrossProduct(UpVector, RightVector);


	// Scale up the forward vector
	ForwardVector *= DistanceBetweenPoints;


	float AngleOnZ = 0, AngleOnX = 0;

	// Decide if we go up, down or straight on the Z axis
	float RandomNumber = FMath::FRand();
	if (RandomNumber < THRESHOLD)
	{
		// Don't allow more than 2 up turns
		if (DownTurns > -2) 
		{
			// Go up
			UE_LOG(LogTemp, Warning, TEXT("UP"));
			AngleOnZ = UpDownAngleLimit;
			DownTurns--;
		}
	}
	else if (RandomNumber < 2.0f * THRESHOLD)
	{
		if (DownTurns < 2)
		{
			UE_LOG(LogTemp, Warning, TEXT("DOWN"));
			AngleOnZ = -UpDownAngleLimit;
			DownTurns++;
		}
	}


	// Decide if we go left or right
	RandomNumber = FMath::FRand();
	if (RandomNumber < THRESHOLD)
	{
		// Don't allow more than 2 turns
		if (LeftTurns > -2)
		{
			// Go Right
			UE_LOG(LogTemp, Warning, TEXT("Right"));
			AngleOnX = LeftRightAngleLimit;
			LeftTurns--;
		}
	}
	else if (RandomNumber < 2.0f * THRESHOLD)
	{
		if (LeftTurns < 2)
		{
			// Go left
			UE_LOG(LogTemp, Warning, TEXT("Left"));
			AngleOnX = -LeftRightAngleLimit;
			LeftTurns++;
		}
	}


	// Rotate around the Right vector for up/ down orientation
	ForwardVector = ForwardVector.RotateAngleAxis(AngleOnZ, RightVector);
	
	// Rotate around the Up vector for left right orinetation
	ForwardVector = ForwardVector.RotateAngleAxis(AngleOnX , UpVector);

	return SplineComponent->GetLocationAtSplinePoint(Index, ESplineCoordinateSpace::Local) + ForwardVector;
}
