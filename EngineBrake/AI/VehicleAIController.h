// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "VehicleAIController.generated.h"

/**
 * Controller class for the VehicleAIPawn
 */
UCLASS()
class ENGINEBRAKE_API AVehicleAIController : public AAIController
{
	GENERATED_BODY()
private:
	AVehicleAIController(const class FObjectInitializer& ObjectInitializer);
	
	/* Called whenever the controller possesses a character bot */
	virtual void Possess(class APawn* InPawn) override;

	virtual void UnPossess() override;
	
	UBehaviorTreeComponent* BehaviorComp; //!< Reference to the Behavior tree component used by this Controller

	UBlackboardComponent* BlackboardComp; //!< Reference to the Blackboard component used in conjuction with the Behavior tree

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName ObstacleKeyName; //!< Blackboard key name of the Obstacle object

public:
	void SetObstacle(APawn* obstacle); //!< Updates the blackboard with a new obstacle

	APawn* GetObstacle(); //!< Returns the reference to the obstacle stored on the blackboard. Clears the value from the blackboard.
};
