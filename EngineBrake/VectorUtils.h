// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "EngineBrake.h"
/**
 * 
 */
class ENGINEBRAKE_API VectorUtils
{
private:
	VectorUtils();
public:
	static float CosineAngleBetweenVectors(FVector A, FVector B);
};
