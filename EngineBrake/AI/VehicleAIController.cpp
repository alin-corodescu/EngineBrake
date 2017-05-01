// Fill out your copyright notice in the Description page of Project Settings.

#include "EngineBrake.h"
#include "VehicleAIController.h"

/* AI Specific includes */
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "VehicleAIPawn.h"

AVehicleAIController::AVehicleAIController(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	BehaviorComp = ObjectInitializer.CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("BehaviorComp"));
	BlackboardComp = ObjectInitializer.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("BlackboardComp"));
	
	ObstacleKeyName = "Obstacle";
}

void AVehicleAIController::Possess(APawn * InPawn)
{
	Super::Possess(InPawn);

	AVehicleAIPawn* Vehicle = Cast<AVehicleAIPawn>(InPawn);
	if (Vehicle)
		// Initialize the Blackboard component
		BlackboardComp->InitializeBlackboard(*Vehicle->BehaviorTree->BlackboardAsset);

	// Start running the BT
	BehaviorComp->StartTree(*Vehicle->BehaviorTree);
}

void AVehicleAIController::UnPossess()
{
	Super::UnPossess();
	BehaviorComp->StopTree();
}

void AVehicleAIController::SetObstacle(APawn * Obstacle)
{
	if (BlackboardComp)
		BlackboardComp->SetValueAsObject(ObstacleKeyName, Obstacle);
}

APawn* AVehicleAIController::GetObstacle()
{
	if (BlackboardComp)
	{
		APawn* Obstacle = Cast<APawn>(BlackboardComp->GetValueAsObject(ObstacleKeyName));
		// Clear the value after getting the reference to it, will probably think of something else to modify it
		BlackboardComp->ClearValue(ObstacleKeyName);
		return Obstacle;
	}
	return nullptr;
}