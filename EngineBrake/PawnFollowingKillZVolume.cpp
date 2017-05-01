// Fill out your copyright notice in the Description page of Project Settings.

#include "EngineBrake.h"
#include "PawnFollowingKillZVolume.h"

#define Z_OFFSET 10000

APawnFollowingKillZVolume::APawnFollowingKillZVolume()
{
	this->SetActorScale3D(FVector(10000, 10000, 1));
}

void APawnFollowingKillZVolume::Tick(float Delta)
{
	static APawn* PawnToFollow = UGameplayStatics::GetPlayerPawn(this, 0);
	Super::Tick(Delta);
	FVector NewLocation = PawnToFollow->GetActorLocation();
	NewLocation.Z -= Z_OFFSET;

	this->SetActorLocation(NewLocation);
}
