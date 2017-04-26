// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "VehicleAIController.generated.h"

/**
 * 
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
	
	UBehaviorTreeComponent* BehaviorComp;

	UBlackboardComponent* BlackboardComp;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName ObstacleKeyName;

public:
	void SetObstacle(APawn* obstacle);

	APawn* GetObstacle();
};
