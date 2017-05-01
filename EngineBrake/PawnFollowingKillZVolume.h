// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/KillZVolume.h"
#include "PawnFollowingKillZVolume.generated.h"

/**
 * 
 */
UCLASS()
class ENGINEBRAKE_API APawnFollowingKillZVolume : public AKillZVolume
{
	GENERATED_BODY()
	APawn* PawnToFollow;
public:
	APawnFollowingKillZVolume();
	virtual void Tick(float Delta) override;
	
	
	
};
