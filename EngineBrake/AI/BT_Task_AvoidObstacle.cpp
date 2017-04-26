// Fill out your copyright notice in the Description page of Project Settings.

#include "EngineBrake.h"
#include "BT_Task_AvoidObstacle.h"
#include "VehicleAIPawn.h"
#include "VehicleAIController.h"

EBTNodeResult::Type UBT_Task_AvoidObstacle::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AVehicleAIController* Controller = Cast<AVehicleAIController>(OwnerComp.GetAIOwner());
	if (!Controller)
		return EBTNodeResult::Failed;

	APawn* Obstacle = Controller->GetObstacle();

	AVehicleAIPawn* ControlledVehicle = Cast<AVehicleAIPawn>(Controller->GetOwner());


	// Brake to avoid impact
	ControlledVehicle->GetVehicleMovementComponent()->SetThrottleInput(-1);

	// Adjust steering
	ControlledVehicle->GetVehicleMovementComponent()->SetSteeringInput(CalculateSteering(Obstacle, ControlledVehicle));

	
}

float UBT_Task_AvoidObstacle::CalculateSteering(AActor * Obstacle, AActor * ControlledVehicle)
{

	// Placeholder for the moment
	return 0.0f;
}
