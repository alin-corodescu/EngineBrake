// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameMode.h"
#include "EngineBrakeGameMode.generated.h"

/**
* Class representing the Game mode, acting as base class for it's Blueprint counterpart BP_EngineBrakeGameMode
*/
UCLASS()
class AEngineBrakeGameMode : public AGameMode
{
	GENERATED_BODY()


public:
	AEngineBrakeGameMode();
	
	virtual void BeginPlay() override;

	//! Function called to end the game
	/*! Called when the player pawn gets destroyed or falls off track*/
	void GameOver();

	//! Reference UMG Asset in the Editor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UUserWidget> GameOverWidgetClass;

	//! Variable to hold the widget After Creating it.
	UUserWidget* GameOverWidget;
};



