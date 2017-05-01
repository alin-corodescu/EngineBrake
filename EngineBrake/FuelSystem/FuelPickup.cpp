// Fill out your copyright notice in the Description page of Project Settings.

#include "EngineBrake.h"
#include "FuelPickup.h"
#include "../EngineBrakePawn.h"


void AFuelPickup::OnBeginOverlap(AActor * Overlapped, AActor * OtherActor)
{
	if (OtherActor->IsA(AEngineBrakePawn::StaticClass()))
		this->Destroy();
}

// Sets default values
AFuelPickup::AFuelPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a mesh component
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FuelPickupMesh"));
	// Set the mesh for the static mesh component.
	ConstructorHelpers::FObjectFinder<UStaticMesh> FuelPickupAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/FuelPickup.FuelPickup'"));
	MeshComponent->SetStaticMesh(FuelPickupAsset.Object);
	this->RootComponent = MeshComponent;

	// Set up overlapping
	MeshComponent->SetCollisionProfileName("OverlapAllDynamic");
	OnActorBeginOverlap.AddDynamic(this, &AFuelPickup::OnBeginOverlap);
}

// Called when the game starts or when spawned
void AFuelPickup::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AFuelPickup::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

