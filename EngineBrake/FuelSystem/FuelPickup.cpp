// Fill out your copyright notice in the Description page of Project Settings.

#include "EngineBrake.h"
#include "FuelPickup.h"
#include "../EngineBrakePawn.h"

void AFuelPickup::OnHit(UPrimitiveComponent * HitComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit)
{
	this->Destroy();
	return;
	if (OtherActor->IsA(AEngineBrakePawn::StaticClass()))
		this->Destroy();
}

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

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FuelPickupMesh"));

	// Set the mesh for the static mesh component.
	ConstructorHelpers::FObjectFinder<UStaticMesh> FuelPickupAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/FuelPickup.FuelPickup'"));
	MeshComponent->SetStaticMesh(FuelPickupAsset.Object);

	//MeshComponent->OnComponentBeginOverlap.AddDynamic(this, &AFuelPickup::OnHit);
	MeshComponent->OnComponentHit.AddDynamic(this, &AFuelPickup::OnHit);

	this->RootComponent = MeshComponent;
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

