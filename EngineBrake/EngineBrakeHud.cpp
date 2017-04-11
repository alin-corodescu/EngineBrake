// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "EngineBrake.h"
#include "EngineBrakeHud.h"
#include "EngineBrakePawn.h"
#include "GameFramework/WheeledVehicle.h"
#include "RenderResource.h"
#include "Shader.h"
#include "Engine/Canvas.h"
#include "Vehicles/WheeledVehicleMovementComponent.h"
#include "Engine/Font.h"
#include "CanvasItem.h"
#include "Engine.h"

// Needed for VR Headset
#if HMD_MODULE_INCLUDED
#include "IHeadMountedDisplay.h"
#endif // HMD_MODULE_INCLUDED 

#define LOCTEXT_NAMESPACE "VehicleHUD"

AEngineBrakeHud::AEngineBrakeHud()
{
	static ConstructorHelpers::FObjectFinder<UFont> Font(TEXT("/Engine/EngineFonts/RobotoDistanceField"));
	HUDFont = Font.Object;
}

void AEngineBrakeHud::DrawHUD()
{
	Super::DrawHUD();

	// Calculate ratio from 1080p
	const float HUDXRatio = Canvas->SizeX / 1920.f;
	const float HUDYRatio = Canvas->SizeY / 1080.f;

	bool bWantHUD = true;
	// We dont want the onscreen hud when using a HMD device	
	if (bWantHUD == true)
	{
		// Get our vehicle so we can check if we are in car. If we are we don't want onscreen HUD
		AEngineBrakePawn* Vehicle = Cast<AEngineBrakePawn>(GetOwningPawn());
		if ((Vehicle != nullptr) && (Vehicle->bInCarCameraActive == false))
		{
			FVector2D ScaleVec(HUDYRatio * 1.4f, HUDYRatio * 1.4f);

			// Speed
			FCanvasTextItem SpeedTextItem(FVector2D(HUDXRatio * 805.f, HUDYRatio * 455), Vehicle->SpeedDisplayString, HUDFont, FLinearColor::White);
			SpeedTextItem.Scale = ScaleVec;
			Canvas->DrawItem(SpeedTextItem);

			// Gear
			FCanvasTextItem GearTextItem(FVector2D(HUDXRatio * 805.f, HUDYRatio * 500.f), Vehicle->GearDisplayString, HUDFont, Vehicle->bInReverseGear == false ? Vehicle->GearDisplayColor : Vehicle->GearDisplayReverseColor);
			GearTextItem.Scale = ScaleVec;
			Canvas->DrawItem(GearTextItem);

			FCanvasTextItem RPMTextItem(FVector2D(HUDXRatio * 805.f, HUDYRatio * 550.f), Vehicle->RPMDisplayString, HUDFont, FLinearColor::White);
			RPMTextItem.Scale = ScaleVec;
			Canvas->DrawItem(RPMTextItem);

			FCanvasTextItem FuelTextItem(FVector2D(HUDXRatio * 805.f, HUDYRatio * 600.f), Vehicle->FuelPrecentageDisplayString, HUDFont, FLinearColor::White);
			FuelTextItem.Scale = ScaleVec;
			Canvas->DrawItem(FuelTextItem);
		}
	}
}


#undef LOCTEXT_NAMESPACE
