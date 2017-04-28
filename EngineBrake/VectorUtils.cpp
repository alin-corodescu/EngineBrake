// Fill out your copyright notice in the Description page of Project Settings.

#include "EngineBrake.h"
#include "VectorUtils.h"

float VectorUtils::CosineAngleBetweenVectors(FVector A, FVector B)
{
	A.Normalize();
	B.Normalize();

	float Result = FVector::DotProduct(A, B);
	return Result;
}
