// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BT_Task_FollowSpline.generated.h"

/**
 * Behaviour tree task used to adjust steering and throttle for a Vehicle Pawn to follow the Spline
 * it was assigned
 */
UCLASS()
class ENGINEBRAKE_API UBT_Task_FollowSpline : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	float CalculateSteering(class AVehicleAIPawn* ControlledVehicle); //!< Outputs the steering required to keep following the Spline

	float CalculateThrottle(class AVehicleAIPawn* ControlledVehicle); //!< Outputs the throttle required to keep following the Spline without over/under turning
};
