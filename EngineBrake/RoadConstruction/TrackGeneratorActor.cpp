// Fill out your copyright notice in the Description page of Project Settings.

#include "EngineBrake.h"
#include "TrackGeneratorActor.h"
#include "Classes/Components/PrimitiveComponent.h"
#include "Classes/Components/SplineComponent.h"
#include "Classes/Components/SplineMeshComponent.h"
#include "Classes/Engine/StaticMesh.h"


// Sets default values
ATrackGeneratorActor::ATrackGeneratorActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	RootComponent = SplineComponent;

	ConstructorHelpers::FObjectFinder<UStaticMesh> RoadMeshFinder(TEXT("StaticMesh'/Game/Props/RoadMesh.RoadMesh'"));
	if (RoadMeshFinder.Succeeded())
	{
		RoadMesh = RoadMeshFinder.Object;
		UE_LOG(LogTemp, Warning, TEXT("Mesh lookup succeeded"));
	}

	ConstructorHelpers::FObjectFinder<UStaticMesh> RightGuardRailMeshFinder(TEXT("StaticMesh'/Game/Props/R_GuardRail.R_GuardRail'"));
	RightGuardRailMesh = RoadMeshFinder.Object;
	ConstructorHelpers::FObjectFinder<UStaticMesh> LeftGuardRailMeshFinder(TEXT("StaticMesh'/Game/Props/L_GuardRail.L_GuardRail'"));
	LeftGuardRailMesh = RoadMeshFinder.Object;
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
	//Super::OnConstruction(Transform);

	UE_LOG(LogTemp, Warning, TEXT("On construction called"));
	BuildTrack();

}

void ATrackGeneratorActor::BuildTrack() 
{
	// Store number of spline points
	SplinePointsNumber = SplineComponent->GetNumberOfSplinePoints();

	// Resize the array to fit all the spline points number
	RoadDataArray.SetNum(SplinePointsNumber, true);

	for (int i = 0; i < SplinePointsNumber; i++)
	{
		BuildTrackElement(RoadMesh, i);
		if (RoadDataArray[i].LeftGuardRail)
			BuildTrackElement(LeftGuardRailMesh, i);
		if (RoadDataArray[i].RightGuardRail)
			BuildTrackElement(RightGuardRailMesh, i);
	}
}
void ATrackGeneratorActor::BuildTrackElement(UStaticMesh * TrackElementMesh, int SplinePointIndex)
{
	int NextSplinePointIndex = (SplinePointIndex + 1) % RoadDataArray.Num();
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
	
	UE_LOG(LogTemp, Warning, TEXT("Setting static mesh"));

	USplineMeshComponent* SplineMesh = ConstructObject<USplineMeshComponent>(USplineMeshComponent::StaticClass(), this);
	SplineMesh->SetMobility(EComponentMobility::Movable);
	SplineMesh->AttachParent = SplineComponent;
	if (WantCollision) 
	{
		SplineMesh->SetCollisionProfileName(FName("BlockAll"));
		SplineMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
	SplineMesh->CreationMethod = EComponentCreationMethod::UserConstructionScript;
	SplineMesh->SetStaticMesh(TrackElementMesh);
	UE_LOG(LogTemp, Warning, TEXT("Static mesh set"));
	//RegisterAllComponents();
	SplineMesh->SetStartAndEnd(StartLocation, StartTangent, EndLocation, EndTangent, true);

	SplineMesh->SetStartRoll(RoadDataArray[SplinePointIndex].TrackBank);

	FVector2D Scale;
	Scale.X = RoadDataArray[SplinePointIndex].TrackWidth;
	Scale.Y = RoadDataArray[SplinePointIndex].TrackThickness;

	SplineMesh->SetStartScale(Scale);

	SplineMesh->SetEndRoll(RoadDataArray[NextSplinePointIndex].TrackBank);

	Scale.X = RoadDataArray[NextSplinePointIndex].TrackWidth;
	Scale.Y = RoadDataArray[NextSplinePointIndex].TrackThickness;
	SplineMesh->SetEndScale(Scale);

	FinishAndRegisterComponent(SplineMesh);

}

#if WITH_EDITOR
void ATrackGeneratorActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	DestroyConstructedComponents();
	BuildTrack();
	
}

void ATrackGeneratorActor::PostEditMove(bool bFinished)
{
	DestroyConstructedComponents();
	BuildTrack();
}
#endif

