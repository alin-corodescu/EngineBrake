// Fill out your copyright notice in the Description page of Project Settings.

#include "EngineBrake.h"
#include "SplineMeshesComponentsPool.h"
#include "Classes/Components/SplineMeshComponent.h"
#include "Classes/Engine/StaticMesh.h"

USplineMeshComponent * SplineMeshesComponentsPool::CreateHiddenSplineMeshComponent(AActor* Creator, UStaticMesh * Mesh)
{
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
	RoadMeshes.SetNum(Number);
	LeftGuardRailMeshes.SetNum(Number);
	RightGuardRailMeshes.SetNum(Number);
	
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
	FirstIndex = 0;
	LastIndex = NumberOfComponents - 1;
}

void SplineMeshesComponentsPool::UpdateEnds(const FVector & Location, const FVector & Tangent)
{
	FVector StartLocation, EndLocation, StartTangent, EndTangent;
	EndLocation = Location;
	EndTangent = Tangent;
	StartLocation = RoadMeshes[LastIndex]->GetEndPosition();
	StartTangent = RoadMeshes[LastIndex]->GetEndTangent();

	RoadMeshes[FirstIndex]->SetStartAndEnd(StartLocation, StartTangent, EndLocation, EndTangent, true);

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
