// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "EndlessTrackGenerator.generated.h"
/**
* Actor spawning and rebuilding the track as the game progresses, at an increasing rate.
*/
UCLASS()
class ENGINEBRAKE_API AEndlessTrackGenerator : public AActor
{
	GENERATED_BODY()
	friend class ASpawner;
private:
	//! Pool of cached SplineMeshes used by this Actor
	/*! Enables the reuse of the SplineMeshes by modifying their locations when necessary */
	class SplineMeshesComponentsPool* SplineMeshesPool;

	//! Spline component of the track generator
	UPROPERTY()
	class USplineComponent* SplineComponent;

	//! Mesh to be used by the road
	UPROPERTY(EditAnywhere)
	class UStaticMesh* RoadMesh;

	//! Mesh to be used for LeftGuardRails
	UPROPERTY(EditAnywhere)
	class UStaticMesh* LeftGuardRailMesh;

	//! Mesh to be used for RightGuardRails
	UPROPERTY(EditAnywhere)
	class UStaticMesh* RightGuardRailMesh;

	//! Number of spline points the track should have at any given time
	UPROPERTY(EditAnywhere)
	int NumberOfSplinePoints = 10;

	//! Distance between consecutive spline points
	UPROPERTY(EditAnywhere)
	int DistanceBetweenPoints = 2000;

	//! Rebuilding interval, default 2 seconds
	/*! Represents how fast the game should advance, is used to increase the rate at which the road de-spawns / spawns */
	UPROPERTY(EditAnywhere)
	float RebuildingInterval = 2;

	//! Angle at which to vary the track on the YZ plane
	UPROPERTY(EditAnywhere)
	float UpDownAngleLimit = 10;

	//! Angle at which to vary the track on the XY plane
	UPROPERTY(EditAnywhere)
	float LeftRightAngleLimit = 30;

	//! Time of the last rebuilding of the track
	float LastRebuild;


	//! Functions used to build the initial elements of the track, called when spawned
	void BuildTrack();

	//! Function used to rebuild the track
	/*!
	* Changes the position of the first SplineMesh to place it at the end, adding a new element at the end and removing one from the start
	*/
	void RebuildTrack();

	//! Randomly generates a new point for the Spline
	/*! The point is generated in concordance with the parameters of this object (DistanceBetweenPoints and Angle limits) */
	FVector GenerateNextPoint();

	//! Counters for directional turns, used to handle the problem of overlapping tracks
	int LeftTurns = 0, DownTurns = 0;
public:	
	//! Sets default values for this actor's properties
	AEndlessTrackGenerator();

	//! Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	//! Called every frame
	virtual void Tick( float DeltaSeconds ) override;	

	//! Getter for the spline component on which the track is based
	/*! Used by the AI to compute the steering and throttle inputs for the AI controlled pawns */
	USplineComponent* GetSplineComponent();
};
