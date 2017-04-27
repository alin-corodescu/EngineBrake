// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/**
 * 
 */
class USplineMeshComponent;
class UStaticMesh;
class ENGINEBRAKE_API SplineMeshesComponentsPool
{
private:
	//! Number of cached Components
	int NumberOfComponents;
	
	//! Vector caching created SplineMeshComponents of the Road Mesh
	TArray<USplineMeshComponent*> RoadMeshes;

	//! Vector caching created SplineMeshComponents of the Left Guard Rail Mesh
	TArray<USplineMeshComponent*> LeftGuardRailMeshes;

	//! Vector caching created SplineMeshComponents of the Right Guard Rail Mesh
	TArray<USplineMeshComponent*> RightGuardRailMeshes;

	//! Index of the first component (ordered by their position in the world
	int FirstIndex = 0;

	int LastIndex;

	USplineMeshComponent* CreateHiddenSplineMeshComponent(AActor* Creator, UStaticMesh* Mesh);
public:
	//! Function used to initialize
	void CreateComponents(class AActor* Creator,int Number, UStaticMesh* RoadMesh, UStaticMesh* LGuard, UStaticMesh* RGuard);
	
	//! Sets the initial positions of the SplineMeshes
	/**
	* Spawns the components at the desired locations, should be used only when initializing the spline
	*/
	void SetInitialPositions(const TArray<FVector>& Locations, const TArray<FVector>& Tangents);

	//! Updates both ends of the Road, moving the first SplineMesh to the end
	void UpdateEnds(const FVector& Location, const FVector& Tangent);
	SplineMeshesComponentsPool();
	~SplineMeshesComponentsPool();
};
