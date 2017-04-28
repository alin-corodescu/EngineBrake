// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "RoadData.h"
#include "TrackGeneratorActor.generated.h"

UCLASS()
class ENGINEBRAKE_API ATrackGeneratorActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATrackGeneratorActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	virtual void OnConstruction(const FTransform& Transform) override;

private:
	//! Spline component of the track generator
	UPROPERTY(EditAnywhere)
	class USplineComponent* SplineComponent;
	
	//! Number of spline points on the spline
	int SplinePointsNumber;

	//! Array containing the data to be used in all the spline points of the spline
	UPROPERTY(EditAnywhere)
	TArray<FRoadData> RoadDataArray;

	//! Mesh to be used by the road
	UPROPERTY(EditAnywhere)
	class UStaticMesh* RoadMesh;

	UPROPERTY(EditAnywhere)
	class UStaticMesh* LeftGuardRailMesh;

	UPROPERTY(EditAnywhere)
	class UStaticMesh* RightGuardRailMesh;


	UPROPERTY(EditAnywhere)
	bool WantCollision;

	void BuildTrackElement(UStaticMesh* TrackElementMesh, int SplinePointIndex);

	void BuildTrack();

	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	void PostEditMove(bool bFinished) override;
};
