// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/KillZVolume.h"
#include "PawnFollowingKillZVolume.generated.h"

/**
 * KillZVolume which follows a pawn (places itself below the pawn)
 */
UCLASS()
class ENGINEBRAKE_API APawnFollowingKillZVolume : public AKillZVolume
{
	GENERATED_BODY()
	//! Pawn to be followed
	APawn* PawnToFollow;
public:
	APawnFollowingKillZVolume();
	virtual void Tick(float Delta) override;
	
	
	
};
