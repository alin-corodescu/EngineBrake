// Fill out your copyright notice in the Description page of Project Settings.

#include "EngineBrake.h"
#include "BT_Task_FollowSpline.h"
#include "VehicleAIController.h"
#include "VehicleAIPawn.h"


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
	ControlledVehicle->GetVehicleMovementComponent()->SetThrottleInput(0.5);

	ControlledVehicle->GetVehicleMovementComponent()->SetSteeringInput(0);

	return EBTNodeResult::Succeeded;
}
