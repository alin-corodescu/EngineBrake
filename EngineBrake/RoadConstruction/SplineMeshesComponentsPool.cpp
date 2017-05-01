// Fill out your copyright notice in the Description page of Project Settings.

#include "EngineBrake.h"
#include "SplineMeshesComponentsPool.h"
#include "Classes/Components/SplineMeshComponent.h"
#include "Classes/Engine/StaticMesh.h"

USplineMeshComponent * SplineMeshesComponentsPool::CreateHiddenSplineMeshComponent(AActor* Creator, UStaticMesh * Mesh)
{
	// Creates a new Hidden spline mesh component
	USplineMeshComponent* SplineMesh = ConstructObject<USplineMeshComponent>(USplineMeshComponent::StaticClass(), Creator);
	SplineMesh->SetMobility(EComponentMobility::Movable);
	SplineMesh->SetCollisionProfileName(FName("BlockAll"));
	SplineMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SplineMesh->CreationMethod = EComponentCreationMethod::UserConstructionScript;
	SplineMesh->SetStaticMesh(Mesh);
	SplineMesh->SetHiddenInGame(true);

	return SplineMesh;
}

void SplineMeshesComponentsPool::CreateComponents(AActor* Creator,int Number, UStaticMesh * RoadMesh, UStaticMesh * LGuard, UStaticMesh * RGuard)
{
	NumberOfComponents = Number;
	
	// Resize the arrays
	RoadMeshes.SetNum(Number);
	LeftGuardRailMeshes.SetNum(Number);
	RightGuardRailMeshes.SetNum(Number);
	
	// Fill the arrays
	for (int i = 0; i < Number; i++)
	{
		RoadMeshes[i] = CreateHiddenSplineMeshComponent(Creator, RoadMesh);
		LeftGuardRailMeshes[i] = CreateHiddenSplineMeshComponent(Creator, LGuard);
		RightGuardRailMeshes[i] = CreateHiddenSplineMeshComponent(Creator, RGuard);
	}
}

void SplineMeshesComponentsPool::SetInitialPositions(const TArray<FVector>& Locations, const TArray<FVector>& Tangents)
{
	int Current;
	// Set up the locations and un-hide the SplineMeshes components
	for (Current = 0; Current < NumberOfComponents; Current++)
	{
		int Next = Current + 1;
		RoadMeshes[Current]->SetStartAndEnd(Locations[Current], Tangents[Current],
			Locations[Next], Tangents[Next], true);
		RoadMeshes[Current]->SetHiddenInGame(false);

		LeftGuardRailMeshes[Current]->SetStartAndEnd(Locations[Current], Tangents[Current],
			Locations[Next], Tangents[Next], true);
		LeftGuardRailMeshes[Current]->SetHiddenInGame(false);

		RightGuardRailMeshes[Current]->SetStartAndEnd(Locations[Current], Tangents[Current],
			Locations[Next], Tangents[Next], true);
		RightGuardRailMeshes[Current]->SetHiddenInGame(false);
	}
	// Store the first and last index
	FirstIndex = 0;
	LastIndex = NumberOfComponents - 1;
}

void SplineMeshesComponentsPool::UpdateEnds(const FVector & Location, const FVector & Tangent)
{
	FVector StartLocation, EndLocation, StartTangent, EndTangent;
	EndLocation = Location;
	EndTangent = Tangent;
	
	// SplineMeshComponents starts where the last one ends
	StartLocation = RoadMeshes[LastIndex]->GetEndPosition();
	StartTangent = RoadMeshes[LastIndex]->GetEndTangent();

	// Set up the parameters
	RoadMeshes[FirstIndex]->SetStartAndEnd(StartLocation, StartTangent, EndLocation, EndTangent, true);
	LeftGuardRailMeshes[FirstIndex]->SetStartAndEnd(StartLocation, StartTangent, EndLocation, EndTangent, true);
	RightGuardRailMeshes[FirstIndex]->SetStartAndEnd(StartLocation, StartTangent, EndLocation, EndTangent, true);

	// Update the indexes
	LastIndex = FirstIndex;

	FirstIndex++;
	FirstIndex %= NumberOfComponents;
	
}

SplineMeshesComponentsPool::SplineMeshesComponentsPool()
{
}

SplineMeshesComponentsPool::~SplineMeshesComponentsPool()
{
}
