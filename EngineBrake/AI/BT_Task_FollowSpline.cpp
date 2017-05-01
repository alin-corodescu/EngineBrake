// Fill out your copyright notice in the Description page of Project Settings.

#include "EngineBrake.h"
#include "BT_Task_FollowSpline.h"
#include "VehicleAIController.h"
#include "VehicleAIPawn.h"
#include "Classes/Components/SplineComponent.h"
#include "../RoadConstruction/EndlessTrackGenerator.h"

#define LOOK_AHEAD_STEERING 2000
// Distance Threshold after which the car is considered off-track for Throttle management
#define DISTANCE_THRESHOLD 75
#define DISTANCE_THRESHOLD_BRAKING 300


EBTNodeResult::Type UBT_Task_FollowSpline::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AVehicleAIController* Controller = Cast<AVehicleAIController>(OwnerComp.GetAIOwner());
	if (!Controller)
		return EBTNodeResult::Failed;

	AVehicleAIPawn* ControlledVehicle = Cast<AVehicleAIPawn>(Controller->GetPawn());
	if (!ControlledVehicle)
		return EBTNodeResult::Failed;


	ControlledVehicle->GetVehicleMovementComponent()->SetThrottleInput(CalculateThrottle(ControlledVehicle));

	ControlledVehicle->GetVehicleMovementComponent()->SetSteeringInput(CalculateSteering(ControlledVehicle));

	return EBTNodeResult::Succeeded;
}

float UBT_Task_FollowSpline::CalculateSteering(AVehicleAIPawn * ControlledVehicle)
{
	USplineComponent* SplineToFollow = ControlledVehicle->GetFollowedPath()->GetSplineComponent();

	// Predict a location
	FVector PredictedLocation = ControlledVehicle->GetActorLocation();
	PredictedLocation += ControlledVehicle->GetActorForwardVector() * LOOK_AHEAD_STEERING;
	
	// Find the closest location on the spline to the predicted one
	FVector SplineClosestLocation = SplineToFollow->FindLocationClosestToWorldLocation(PredictedLocation, ESplineCoordinateSpace::World);

	// Compute the Rotation needed to face the point on the spline
	FRotator Rotation = (SplineClosestLocation - ControlledVehicle->GetActorLocation()).Rotation();

	Rotation.Normalize();
	FRotator ActorRotation = ControlledVehicle->GetActorRotation();
	ActorRotation.Normalize();

	//Compute the difference we need to apply
	FRotator DeltaRotation = (Rotation - ActorRotation);

	// Apply handbrake if the angle is higher than 45 degrees
	//ControlledVehicle->GetVehicleMovementComponent()->SetHandbrakeInput(DeltaRotation.Yaw > 45 || DeltaRotation.Yaw < -45);

	// Map the angle range to steering input range
	return FMath::GetMappedRangeValueClamped(FVector2D(-90, 90), FVector2D(-1, 1), DeltaRotation.Yaw);
}

float UBT_Task_FollowSpline::CalculateThrottle(AVehicleAIPawn * ControlledVehicle)
{
	// Apply the logic only if we are moving at a considerable speed
	if (ControlledVehicle->GetVehicleMovementComponent()->GetForwardSpeed() > 300)
	{
		USplineComponent* SplineToFollow = ControlledVehicle->GetFollowedPath()->GetSplineComponent();
		FVector PredictedLocation = ControlledVehicle->GetActorLocation();
		// Predict a location
		PredictedLocation += ControlledVehicle->GetActorForwardVector() *
			ControlledVehicle->GetVehicleMovementComponent()->GetForwardSpeed() *
			(ControlledVehicle->LookAheadMiliSeconds / 1000.0f);

		// Find the closest point on the spline (in relation to the predicted location)
		FVector SplineClosestLocation = SplineToFollow->FindLocationClosestToWorldLocation(PredictedLocation, ESplineCoordinateSpace::World);

		// Compute the distance between the 2 points
		float Distance = FVector::Dist(SplineClosestLocation, PredictedLocation);
		Distance = FMath::Abs<float>(Distance);

		// If we are about to go highly off track
		if (Distance > DISTANCE_THRESHOLD_BRAKING)
		{	
			return -1;

			// This here is a dropped idea, could be interesting though to work more on it

			/*UE_LOG(LogTemp, Warning, TEXT("Distance is %f"), Distance);
			float DistMax = ControlledVehicle->GetVehicleMovementComponent()->GetForwardSpeed() * (ControlledVehicle->LookAheadMiliSeconds / 1000.0f);
			UE_LOG(LogTemp, Warning, TEXT("Max Distance is %f"), DistMax);
	
			// Clamp to the -1, 2.5 interval because the car needs to be more sensitive when approaching a turn
			float Throttle = -FMath::GetMappedRangeValueClamped(FVector2D(0.0f, DistMax), FVector2D(-1.0f, 2.0f), Distance);

			UE_LOG(LogTemp, Warning, TEXT("Resulting throttle: %f"), Throttle);
			return Throttle;*/
		}
		return 1.0f;

	}

	return 1.0f;
}
