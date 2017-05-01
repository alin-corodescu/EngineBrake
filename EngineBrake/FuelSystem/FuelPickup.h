// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "FuelPickup.generated.h"
/**
* Class representing Fuel pickups that spawn in the world
*/
UCLASS()
class ENGINEBRAKE_API AFuelPickup : public AActor
{
	GENERATED_BODY()

	//! Static Mesh used by this Actor
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* MeshComponent;

	//! Callback function when overlapping with another actor
	/*! Destroyes the actor after overlapping with the Player controlled vehicle */
	UFUNCTION()
	void OnBeginOverlap(AActor* Overlapped, AActor* OtherActor);
public:	
	//! Sets default values for this actor's properties
	AFuelPickup();

	//! Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	//! Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	
	
};
