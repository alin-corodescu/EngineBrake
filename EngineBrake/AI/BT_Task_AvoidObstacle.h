// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BT_Task_AvoidObstacle.generated.h"

/**
 * 
 */
UCLASS()
class ENGINEBRAKE_API UBT_Task_AvoidObstacle : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	float CalculateSteering(class AActor* Obstacle,class AActor* ControlledVehicle);
	
};
