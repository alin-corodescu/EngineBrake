// Fill out your copyright notice in the Description page of Project Settings.

#include "EngineBrake.h"
#include "BT_Task_AvoidObstacle.h"
#include "VehicleAIPawn.h"
#include "VehicleAIController.h"
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

	if (ControlledVehicle->GetVehicleMovementComponent()->GetForwardSpeed() > 0.1)
		// Brake to avoid impact
		ControlledVehicle->GetVehicleMovementComponent()->SetThrottleInput(-1);
	else
		// Stop braking when the vehicle stops
		ControlledVehicle->GetVehicleMovementComponent()->SetThrottleInput(0);

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

	// Right Vector corresponds to 1, -1 for the -RightVector
	FVector RightVector = ControlledVehicle->GetActorRightVector();
	FVector LeftVector = -RightVector;
	float RightCosine = VectorUtils::CosineAngleBetweenVectors(SteeringVector, RightVector);
	float LeftCosine = VectorUtils::CosineAngleBetweenVectors(SteeringVector, LeftVector);
	UE_LOG(LogTemp, Warning, TEXT("Cosines are: Left = %f ; Right = %f"), LeftCosine, RightCosine);

	if (RightCosine < LeftCosine)
		return -1.0f;
	return 1.0f;
}
