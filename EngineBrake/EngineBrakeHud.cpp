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

	UTexture2D* EngineOnImageAsset;
	static ConstructorHelpers::FObjectFinder<UTexture2D> EngineOnTextureOb(TEXT("Texture2D'/Game/Imagesd/Engine_on.Engine_on'"));
	EngineOnImageAsset = EngineOnTextureOb.Object;
	float x = EngineOnImageAsset->GetSizeX();
	float y = EngineOnImageAsset->GetSizeY();
	EngineOnIcon = UCanvas::MakeIcon(EngineOnImageAsset, 0, 0, x, y);

	UTexture2D* EngineOffImageAsset; 
	static ConstructorHelpers::FObjectFinder<UTexture2D> EngineOffTextureOb(TEXT("Texture2D'/Game/Imagesd/Engine_off.Engine_off'"));
	EngineOffImageAsset = EngineOffTextureOb.Object;
	x = EngineOffImageAsset->GetSizeX();
	y = EngineOffImageAsset->GetSizeY();
	EngineOffIcon = UCanvas::MakeIcon(EngineOffImageAsset, 0, 0, x, y);

	UTexture2D* RPMMeterImageAsset; 
	static ConstructorHelpers::FObjectFinder<UTexture2D> RPMMeterTextureOb(TEXT("Texture2D'/Game/Imagesd/RPM_meter.RPM_meter'"));
	RPMMeterImageAsset = RPMMeterTextureOb.Object;
	x = RPMMeterImageAsset->GetSizeX();
	y = RPMMeterImageAsset->GetSizeY();
	RPMMeterIcon = UCanvas::MakeIcon(RPMMeterImageAsset, 0, 0, x, y);

}

void AEngineBrakeHud::DrawHUD()
{
	Super::DrawHUD();

	DrawScore();

	DrawPopupMessage();

	DrawFuelLevel();

	DrawEngineIndicator();

	DrawGear();

	DrawSpeed();

	DrawRPM();

	// Calculate ratio from 1080p
	const float HUDXRatio = Canvas->SizeX / 1920.f;
	const float HUDYRatio = Canvas->SizeY / 1080.f;

	/*AEngineBrakePawn* Vehicle = Cast<AEngineBrakePawn>(GetOwningPawn());

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

			/*FCanvasTextItem FuelTextItem(FVector2D(HUDXRatio * 805.f, HUDYRatio * 600.f), Vehicle->FuelPrecentageDisplayString, HUDFont, FLinearColor::White);
			FuelTextItem.Scale = ScaleVec;
			Canvas->DrawItem(FuelTextItem);*/

			/*FCanvasTextItem ScoreTextItem(FVector2D(HUDXRatio * 805.f, HUDYRatio * 650.f), Vehicle->ScoreDisplayString, HUDFont, FLinearColor::White);
			ScoreTextItem.Scale = ScaleVec;
			Canvas->DrawItem(ScoreTextItem);*/
}


#undef LOCTEXT_NAMESPACE

void AEngineBrakeHud::DrawScore()
{
	AEngineBrakePawn* Vehicle = Cast<AEngineBrakePawn>(GetOwningPawn());


	FVector2D ScreenDimensions = FVector2D(Canvas->SizeX, Canvas->SizeY);

	FText ScoreString = FText::Format(FText::FromString("Score : {0}"), FText::AsNumber(Vehicle->PlayerState->Score));
	int size = ScoreString.ToString().Len();
	FVector2D TextCentrePos = FVector2D((Canvas->SizeX - 12 * size), 15);
	FCanvasTextItem TextItem(TextCentrePos, ScoreString,
		HUDFont, FLinearColor::Blue);
	Canvas->DrawItem(TextItem);

	/*// Calculate ratio from 1080p
	const float HUDXRatio = Canvas->SizeX / 1920.f;
	const float HUDYRatio = Canvas->SizeY / 1080.f;
	FVector2D ScaleVec(HUDYRatio * 1.4f, HUDYRatio * 1.4f);

	FCanvasTextItem ScoreTextItem(FVector2D(HUDXRatio * 805.f, HUDYRatio * 650.f), Vehicle->ScoreDisplayString, HUDFont, FLinearColor::White);
	ScoreTextItem.Scale = ScaleVec;
	Canvas->DrawItem(ScoreTextItem);*/
}

void AEngineBrakeHud::DrawPopupMessage()
{
}

void AEngineBrakeHud::DrawFuelLevel()
{
	AEngineBrakePawn* Vehicle = Cast<AEngineBrakePawn>(GetOwningPawn());
	FVector2D TextPos(15, 15);
	FCanvasTextItem FuelTextItem(TextPos, Vehicle->FuelPrecentageDisplayString, HUDFont, FLinearColor::White);
	Canvas->DrawItem(FuelTextItem);
}

void AEngineBrakeHud::DrawEngineIndicator()
{
	AEngineBrakePawn* Vehicle = Cast<AEngineBrakePawn>(GetOwningPawn());
	if (Vehicle->bRunningEngine)
		Canvas->DrawIcon(EngineOnIcon, 15, Canvas->SizeY - 200, 0.5);
	else 
		Canvas->DrawIcon(EngineOffIcon, 15, Canvas->SizeY - 200, 0.5);

}

void AEngineBrakeHud::DrawGear()
{
	AEngineBrakePawn* Vehicle = Cast<AEngineBrakePawn>(GetOwningPawn());
	const float HUDXRatio = Canvas->SizeX / 1920.f;
	const float HUDYRatio = Canvas->SizeY / 1080.f;
	//FVector2D ScaleVec(HUDYRatio * 1.4f, HUDYRatio * 1.4f);

	FCanvasTextItem GearTextItem(FVector2D(Canvas->SizeX - 270, Canvas->SizeY - 330), Vehicle->GearDisplayString, HUDFont, FLinearColor::White);
	GearTextItem.Scale = FVector2D(2,2);
	Canvas->DrawItem(GearTextItem);
}

void AEngineBrakeHud::DrawSpeed()
{
	AEngineBrakePawn* Vehicle = Cast<AEngineBrakePawn>(GetOwningPawn());
	//const float HUDXRatio = Canvas->SizeX / 1920.f;
	//const float HUDYRatio = Canvas->SizeY / 1080.f;
	//FVector2D ScaleVec(HUDYRatio * 1.4f, HUDYRatio * 1.4f);

	FCanvasTextItem SpeedTextItem(FVector2D(Canvas->SizeX - 200, Canvas->SizeY - 360), Vehicle->SpeedDisplayString, HUDFont, FLinearColor::White);
	SpeedTextItem.Scale = FVector2D(2,2);
	Canvas->DrawItem(SpeedTextItem);
}

void AEngineBrakeHud::DrawRPM()
{
	AEngineBrakePawn* Vehicle = Cast<AEngineBrakePawn>(GetOwningPawn());

	Canvas->DrawIcon(RPMMeterIcon, Canvas->SizeX - 320, Canvas->SizeY - 310, 0.85);

	FVector2D Center(Canvas->SizeX - 170, Canvas->SizeY - 150);
	FVector2D End(Canvas->SizeX - 170, Canvas->SizeY - 250);

	float RPM = Vehicle->GetVehicleMovementComponent()->GetEngineRotationSpeed();
	float MaxRPM = Vehicle->GetVehicleMovementComponent()->GetEngineMaxRotationSpeed();

	float IndicatorRotation = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, MaxRPM), FVector2D(-150.0f, 75.0f), RPM);
	FVector2D NewEnd = Center - (Center - End).GetRotated(IndicatorRotation);

	Canvas->K2_DrawLine(Center, NewEnd , 2.0f );

}
