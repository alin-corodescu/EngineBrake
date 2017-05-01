// Fill out your copyright notice in the Description page of Project Settings.

#include "EngineBrake.h"
#include "BT_Task_AvoidObstacle.h"
#include "VehicleAIPawn.h"
#include "VehicleAIController.h"
#include "../EngineBrakePawn.h"
#include "../VectorUtils.h"

EBTNodeResult::Type UBT_Task_AvoidObstacle::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//UE_LOG(LogTemp, Warning, TEXT("Evasive action called"));
	AVehicleAIController* Controller = Cast<AVehicleAIController>(OwnerComp.GetAIOwner());
	if (!Controller)
		return EBTNodeResult::Failed;

	APawn* Obstacle = Controller->GetObstacle();

	AVehicleAIPawn* ControlledVehicle = Cast<AVehicleAIPawn>(Controller->GetPawn());

	if (!ControlledVehicle)
		return EBTNodeResult::Failed;


	ControlledVehicle->GetVehicleMovementComponent()->SetThrottleInput(CalculateThrottle(Obstacle, ControlledVehicle));
	// Adjust steering
	ControlledVehicle->GetVehicleMovementComponent()->SetSteeringInput(CalculateSteering(Obstacle, ControlledVehicle));
	return EBTNodeResult::Succeeded;
	
}

float UBT_Task_AvoidObstacle::CalculateSteering(AActor * Obstacle, AActor * ControlledVehicle)
{
	// Store the forward vector
	FVector ForwardVector = ControlledVehicle->GetActorForwardVector();

	// Direction to the obstacle
	FVector DirectionToObstacle = ControlledVehicle->GetActorLocation() - Obstacle->GetActorLocation();

	DirectionToObstacle.Normalize();

	FVector SteeringVector = ForwardVector - DirectionToObstacle;
	
	// Compute the angle between the controlled vehicle and the other actor
	FRotator VehicleRotator = SteeringVector.Rotation();

	// Steer in order to avoid the actor
	if (VehicleRotator.Yaw < 0)
		return 1;
	return -1;
}
float UBT_Task_AvoidObstacle::CalculateThrottle(AActor* Obstacle, AVehicleAIPawn* ControlledVehicle)
{
	if (ControlledVehicle->GetVehicleMovementComponent()->GetForwardSpeed() > 0.1)
		// Brake to avoid impact
		return -1;
	else
		return 0;
}