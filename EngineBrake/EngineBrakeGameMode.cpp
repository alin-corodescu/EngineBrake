// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "EngineBrake.h"
#include "EngineBrakeGameMode.h"
#include "EngineBrakePawn.h"
#include "EngineBrakeHud.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"

AEngineBrakeGameMode::AEngineBrakeGameMode()
{
	DefaultPawnClass = AEngineBrakePawn::StaticClass();
	PlayerStateClass = APlayerState::StaticClass();
	HUDClass = AEngineBrakeHud::StaticClass();


}

void AEngineBrakeGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (GameOverWidgetClass)
	{
		GameOverWidget = CreateWidget<UUserWidget>(GetWorld(), GameOverWidgetClass);
	}
}

void AEngineBrakeGameMode::GameOver()
{
	GameOverWidget->AddToViewport();
	UGameplayStatics::SetGamePaused(this, true);
}
