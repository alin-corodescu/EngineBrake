// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "EndlessTrackGenerator.generated.h"

UCLASS()
class ENGINEBRAKE_API AEndlessTrackGenerator : public AActor
{
	GENERATED_BODY()
	friend class ASpawner;
private:

	class SplineMeshesComponentsPool* SplineMeshesPool;

	//! Spline component of the track generator
	UPROPERTY()
	class USplineComponent* SplineComponent;

	//! Mesh to be used by the road
	UPROPERTY(EditAnywhere)
	class UStaticMesh* RoadMesh;

	UPROPERTY(EditAnywhere)
	class UStaticMesh* LeftGuardRailMesh;

	UPROPERTY(EditAnywhere)
	class UStaticMesh* RightGuardRailMesh;

	UPROPERTY(EditAnywhere)
	int NumberOfSplinePoints = 10;

	UPROPERTY(EditAnywhere)
	int DistanceBetweenPoints = 2000;

	//! Time of the last rebuilding of the track
	float LastRebuild;

	//! Rebuilding interval, default 2 seconds
	UPROPERTY(EditAnywhere)
	float RebuildingInterval = 2;

	UPROPERTY(EditAnywhere)
	float UpDownAngleLimit = 10;

	UPROPERTY(EditAnywhere)
	float LeftRightAngleLimit = 30;

	void BuildTrack();

	//! Function used to rebuild the track
	/**
	*	Deletes the first element of the track and adds another to the end
	*/
	void RebuildTrack();

	FVector GenerateNextPoint();

	//! Counters for directional turns, used to handle the problem of overlapping tracks
	int LeftTurns = 0, DownTurns = 0;
public:	
	// Sets default values for this actor's properties
	AEndlessTrackGenerator();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;	

	USplineComponent* GetSplineComponent();
};
