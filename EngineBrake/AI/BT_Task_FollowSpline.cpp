// Fill out your copyright notice in the Description page of Project Settings.

#include "EngineBrake.h"
#include "BT_Task_FollowSpline.h"
#include "VehicleAIController.h"
#include "VehicleAIPawn.h"
#include "Classes/Components/SplineComponent.h"

#define LOOK_AHEAD_STEERING 3000

EBTNodeResult::Type UBT_Task_FollowSpline::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	//UE_LOG(LogTemp, Warning, TEXT("Follow spline called"));

	AVehicleAIController* Controller = Cast<AVehicleAIController>(OwnerComp.GetAIOwner());
	if (!Controller)
	{
		UE_LOG(LogTemp, Warning, TEXT("Controller failed"));
		return EBTNodeResult::Failed;
	}

	AVehicleAIPawn* ControlledVehicle = Cast<AVehicleAIPawn>(Controller->GetPawn());
	if (!ControlledVehicle)
	{
		UE_LOG(LogTemp, Warning, TEXT("Vehicle failed"));
		return EBTNodeResult::Failed;
	}

	
	// Placeholders for actual logic of spline following
	ControlledVehicle->GetVehicleMovementComponent()->SetThrottleInput(CalculateThrottle(ControlledVehicle));

	ControlledVehicle->GetVehicleMovementComponent()->SetSteeringInput(CalculateSteering(ControlledVehicle));

	return EBTNodeResult::Succeeded;
}

float UBT_Task_FollowSpline::CalculateSteering(AVehicleAIPawn * ControlledVehicle)
{
	USplineComponent* SplineToFollow = ControlledVehicle->GetFollowedPath();
	FVector PredictedLocation = ControlledVehicle->GetActorLocation();
	PredictedLocation += ControlledVehicle->GetActorForwardVector() * LOOK_AHEAD_STEERING;
	
	FVector SplineClosestLocation = SplineToFollow->FindLocationClosestToWorldLocation(PredictedLocation, ESplineCoordinateSpace::World);

	FRotator Rotation = (SplineClosestLocation - ControlledVehicle->GetActorLocation()).Rotation();

	Rotation.Normalize();
	FRotator ActorRotation = ControlledVehicle->GetActorRotation();
	ActorRotation.Normalize();

	FRotator DeltaRotation = (Rotation - ActorRotation);

	// Apply handbrake if the angle is higher than 45 degrees
	ControlledVehicle->GetVehicleMovementComponent()->SetHandbrakeInput(DeltaRotation.Yaw > 45 || DeltaRotation.Yaw < -45);

	return FMath::GetMappedRangeValueClamped(FVector2D(-90, 90), FVector2D(-1, 1), DeltaRotation.Yaw);
}

float UBT_Task_FollowSpline::CalculateThrottle(AVehicleAIPawn * ControlledVehicle)
{
	USplineComponent* SplineToFollow = ControlledVehicle->GetFollowedPath();
	FVector PredictedLocation = ControlledVehicle->GetActorLocation();
	// Predict a location
	PredictedLocation += ControlledVehicle->GetActorForwardVector() * ControlledVehicle->LookAheadSeconds;

	FVector SplineClosestLocation = SplineToFollow->FindLocationClosestToWorldLocation(PredictedLocation, ESplineCoordinateSpace::World);

	float Distance = FVector::Dist(SplineClosestLocation, PredictedLocation);
	Distance = FMath::Abs<float>(Distance);
	float DistMax = ControlledVehicle->GetVehicleMovementComponent()->GetForwardSpeed() * ControlledVehicle->LookAheadSeconds;

	float Throttle = - FMath::GetMappedRangeValueUnclamped(FVector2D(0.0f, DistMax), FVector2D(-1, 1), Distance);

	return Throttle;
}
