// Fill out your copyright notice in the Description page of Project Settings.

#include "EngineBrake.h"
#include "TrackGeneratorActor.h"
#include "Classes/Components/PrimitiveComponent.h"
#include "Classes/Components/SplineComponent.h"


// Sets default values
ATrackGeneratorActor::ATrackGeneratorActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	
}

// Called when the game starts or when spawned
void ATrackGeneratorActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATrackGeneratorActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ATrackGeneratorActor::OnConstruction(const FTransform & Transform)
{
	Super::OnConstruction(Transform);

	// Store number of spline points
	SplinePointsNumber = SplineComponent->GetNumberOfSplinePoints();

	// Resize the array to fit all the spline points number
	RoadDataArray.SetNum(SplinePointsNumber, true);
}

