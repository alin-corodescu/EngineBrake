// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "EngineBrake.h"
#include "EngineBrakeGameMode.h"
#include "EngineBrakePawn.h"
#include "EngineBrakeWheelFront.h"
#include "EngineBrakeWheelRear.h"
#include "EngineBrakeHud.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Vehicles/WheeledVehicleMovementComponent4W.h"
#include "FuelSystemComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Engine.h"
#include "Score/ScoreCalculator.h"
#include "Spawner.h"
#include "FuelSystem/FuelPickup.h"
#include "AI/VehicleAIPawn.h"

#define MAX_GEAR 5
#define LOCTEXT_NAMESPACE "VehiclePawn"

AEngineBrakePawn::AEngineBrakePawn()
{
	// Car mesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CarMesh(TEXT("/Game/Vehicle/Sedan/Sedan_SkelMesh.Sedan_SkelMesh"));
	GetMesh()->SetSkeletalMesh(CarMesh.Object);

	static ConstructorHelpers::FClassFinder<UObject> AnimBPClass(TEXT("/Game/Vehicle/Sedan/Sedan_AnimBP"));
	GetMesh()->SetAnimInstanceClass(AnimBPClass.Class);

	// Simulation
	UWheeledVehicleMovementComponent4W* Vehicle4W = CastChecked<UWheeledVehicleMovementComponent4W>(GetVehicleMovement());

	check(Vehicle4W->WheelSetups.Num() == 4);

	Vehicle4W->WheelSetups[0].WheelClass = UEngineBrakeWheelFront::StaticClass();
	Vehicle4W->WheelSetups[0].BoneName = FName("Wheel_Front_Left");
	Vehicle4W->WheelSetups[0].AdditionalOffset = FVector(0.f, -12.f, 0.f);

	Vehicle4W->WheelSetups[1].WheelClass = UEngineBrakeWheelFront::StaticClass();
	Vehicle4W->WheelSetups[1].BoneName = FName("Wheel_Front_Right");
	Vehicle4W->WheelSetups[1].AdditionalOffset = FVector(0.f, 12.f, 0.f);

	Vehicle4W->WheelSetups[2].WheelClass = UEngineBrakeWheelRear::StaticClass();
	Vehicle4W->WheelSetups[2].BoneName = FName("Wheel_Rear_Left");
	Vehicle4W->WheelSetups[2].AdditionalOffset = FVector(0.f, -12.f, 0.f);

	Vehicle4W->WheelSetups[3].WheelClass = UEngineBrakeWheelRear::StaticClass();
	Vehicle4W->WheelSetups[3].BoneName = FName("Wheel_Rear_Right");
	Vehicle4W->WheelSetups[3].AdditionalOffset = FVector(0.f, 12.f, 0.f);


	Vehicle4W->DifferentialSetup.DifferentialType = EVehicleDifferential4W::LimitedSlip_4W;
	Vehicle4W->TransmissionSetup.bUseGearAutoBox = false;

	// Create a spring arm component
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm0"));
	SpringArm->TargetOffset = FVector(0.f, 0.f, 200.f);
	SpringArm->SetRelativeRotation(FRotator(-15.f, 0.f, 0.f));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 600.0f;
	SpringArm->bEnableCameraRotationLag = true;
	SpringArm->CameraRotationLagSpeed = 7.f;
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritRoll = false;

	// Create camera component 
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera0"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;
	Camera->FieldOfView = 90.f;

	// Create the Fuel system of this vehicle
	FuelSystem = CreateDefaultSubobject<UFuelSystemComponent>(TEXT("FuelSystem"));
	FuelSystem->SetupConsumptionParameters(this, ConsumptionCoefficient);
	this->AddOwnedComponent(FuelSystem);

	// Setup the audio component and allocate it a sound cue
	static ConstructorHelpers::FObjectFinder<USoundCue> SoundCue(TEXT("SoundCue'/Game/VehicleAdv/Sound/Engine_Loop_Cue.Engine_Loop_Cue'"));
	EngineSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("EngineSound"));
	EngineSoundComponent->SetSound(SoundCue.Object);
	EngineSoundComponent->SetupAttachment(GetMesh());

	// Set up collision callback
	OnActorBeginOverlap.AddDynamic(this, &AEngineBrakePawn::OnOverlap);
	GetMesh()->OnComponentHit.AddDynamic(this, &AEngineBrakePawn::OnCollision);

	// Set up effects for collision
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ExplosionFinder(TEXT("ParticleSystem'/Game/StarterContent/Particles/P_Explosion.P_Explosion'"));
	if (ExplosionFinder.Succeeded())
	{
		Explosion= ExplosionFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> ExplosionSoundFinder(TEXT("SoundCue'/Game/StarterContent/Audio/Explosion_Cue.Explosion_Cue'"));
	if (ExplosionSoundFinder.Succeeded())
	{
		ExplosionSound = ExplosionSoundFinder.Object;
	}

	// Set up initial flags
	bInReverseGear = false;
	// Engine is not running at the start, we have to start it manually
	bRunningEngine = false;
	bOutOfFuel = false;
}

void AEngineBrakePawn::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	// Set up key bindings
	check(InputComponent);

	InputComponent->BindAxis("MoveForward", this, &AEngineBrakePawn::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AEngineBrakePawn::MoveRight);

	InputComponent->BindAction("Handbrake", IE_Pressed, this, &AEngineBrakePawn::OnHandbrakePressed);
	InputComponent->BindAction("Handbrake", IE_Released, this, &AEngineBrakePawn::OnHandbrakeReleased);
	InputComponent->BindAction("Request_Fuel", IE_Pressed, this, &AEngineBrakePawn::OnFuelRequest);

	InputComponent->BindAction("Upshift", IE_Pressed, this, &AEngineBrakePawn::OnUpShift);
	InputComponent->BindAction("Downshift", IE_Pressed, this, &AEngineBrakePawn::OnDownShift);
	InputComponent->BindAction("Reverse", IE_Pressed, this, &AEngineBrakePawn::OnReverseGear);
	InputComponent->BindAction("Neutral", IE_Pressed, this, &AEngineBrakePawn::OnNeutralGear);
	InputComponent->BindAction("StartEngine", IE_Pressed, this, &AEngineBrakePawn::StartEngine);
}

void AEngineBrakePawn::MoveForward(float Val)
{
	// Only apply negative throttle if the vehicle is moving forward
	if (Val < 0)
	{
		if (GetVehicleMovementComponent()->GetForwardSpeed() > 0)
			GetVehicleMovementComponent()->SetThrottleInput(Val);
	}

	// Only apply throttle if the engine is running, otherwise don't
	if (bRunningEngine)
	{
		// Change the throttle value if we are in reverse
		if (bInReverseGear)
			Val = -Val;
		GetVehicleMovementComponent()->SetThrottleInput(Val);
		return;
	}
}

void AEngineBrakePawn::MoveRight(float Val)
{
	GetVehicleMovementComponent()->SetSteeringInput(Val);
}

void AEngineBrakePawn::OnHandbrakePressed()
{
	GetVehicleMovementComponent()->SetHandbrakeInput(true);
}

void AEngineBrakePawn::OnHandbrakeReleased()
{
	GetVehicleMovementComponent()->SetHandbrakeInput(false);
}

void AEngineBrakePawn::OnFuelRequest()
{
	SpawnerRef->SpawnFuelPickup();
}

void AEngineBrakePawn::Tick(float Delta)
{
	Super::Tick(Delta);

	// Check if the engine should stall
	if (bRunningEngine && CheckLowSpeedThreshold()) 
		StallEngine();

	// Setup the flag to say we are in reverse gear
	bInReverseGear = GetVehicleMovement()->GetCurrentGear() < 0;
	
	// Update the strings used in the hud (incar and onscreen)
	UpdateHUDStrings();

	// Update the score
	ScoreCalculator* Calculator = ScoreCalculator::GetInstance();
	float ScoreValue = Calculator->ComputeTickingScore(Delta);
	PlayerState->Score += ScoreValue;

	// Pass the engine RPM to the sound component
	float RPMToAudioScale = 2500.0f / GetVehicleMovement()->GetEngineMaxRotationSpeed();
	EngineSoundComponent->SetFloatParameter("RPM", GetVehicleMovement()->GetEngineRotationSpeed()*RPMToAudioScale);
}

void AEngineBrakePawn::BeginPlay()
{
	Super::BeginPlay();
}

void AEngineBrakePawn::OnUpShift()
{
	// Find the current gear we are in
	int Gear = GetVehicleMovement()->GetCurrentGear();
	int MaxGear = MAX_GEAR;
	if (Gear < MaxGear)
	{
		GetVehicleMovement()->SetTargetGear(Gear + 1, true);
		// If we are doing a normal upshift, add score to the total score
		if (Gear + 1 > 1)
		{
			// Compute score depending on the RPM
			ScoreCalculator* Calculator = ScoreCalculator::GetInstance();
			float ScoreValue = Calculator->ComputeUpshiftScore(GetVehicleMovementComponent()->GetEngineRotationSpeed());
			PlayerState->Score += ScoreValue;

			if (ScoreValue > 0)
			{
				// Display a popup message
				PopupString = FText::FromString("Good upshift");

				GetWorld()->GetTimerManager().ClearTimer(PopupCleanerTimerHandle);
				GetWorld()->GetTimerManager().SetTimer(PopupCleanerTimerHandle, this, &AEngineBrakePawn::ClearPopupMessage, 3.0f);
			}
		}
	}
}

void AEngineBrakePawn::OnDownShift()
{
	// Find the current gear we are in
	int Gear = GetVehicleMovement()->GetCurrentGear();
	if (Gear > 0)
		GetVehicleMovement()->SetTargetGear(Gear - 1, true);
}

void AEngineBrakePawn::OnNeutralGear()
{
	GetVehicleMovement()->SetTargetGear(0, true);
}

void AEngineBrakePawn::OnReverseGear()
{
	GetVehicleMovement()->SetTargetGear(-1, true);
}

void AEngineBrakePawn::OutOfFuel()
{
	bOutOfFuel = true;
	StallEngine();
}

void AEngineBrakePawn::OnCollision(UPrimitiveComponent * HitComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit)
{
	// Don't destory on collision with the track elements, just when colliding with other vehicles
	if (OtherActor->IsA(AVehicleAIPawn::StaticClass()))
	{
		// Play destruction effects
		UGameplayStatics::SpawnEmitterAtLocation(this, Explosion, GetActorLocation(), GetActorRotation(), true);
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());

		// Lock the camera in position
		Camera->DetachFromParent(true);
		
		// Hide the mesh of the vehicle
		GetMesh()->SetVisibility(false);

		// Initiate a delayed game over sequence, to play the effects
		AEngineBrakeGameMode* GameMode = Cast<AEngineBrakeGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		static FTimerHandle GameOverDelayer;
		GetWorld()->GetTimerManager().SetTimer(GameOverDelayer, GameMode, &AEngineBrakeGameMode::GameOver, 2.0f);
	}
}

void AEngineBrakePawn::OnOverlap(AActor * OverlappedActor, AActor * OtherActor)
{
	// Refuel only if we overlap with a FuelPickup
	if (OtherActor->IsA(AFuelPickup::StaticClass()))
		this->FuelSystem->ResetFuelLevel();
}

void AEngineBrakePawn::SetSpawner(ASpawner * Spawner)
{
	this->SpawnerRef = Spawner;
}

void AEngineBrakePawn::UpdateHUDStrings()
{
	float KPH = FMath::Abs(GetVehicleMovement()->GetForwardSpeed()) * 0.036f;
	int32 KPH_int = FMath::FloorToInt(KPH);

	// Using FText because this is display text that should be localizable
	SpeedDisplayString = FText::Format(LOCTEXT("SpeedFormat", "{0} km/h"), FText::AsNumber(KPH_int));

	// Update fuel precentage string
	float FuelPrecentage = FuelSystem->GetFuelPrecentage();
	FuelPrecentageDisplayString = FText::Format(LOCTEXT("FuelFormat", "{0}% FUEL"), FText::AsNumber(FuelPrecentage));

	// Update the score string
	ScoreDisplayString = FText::Format(LOCTEXT("Score", "Score : {0}"), FText::AsNumber(PlayerState->Score));
	
	if (bInReverseGear == true)
	{
		GearDisplayString = FText(LOCTEXT("ReverseGear", "R"));
	}
	else
	{
		int32 Gear = GetVehicleMovement()->GetCurrentGear();
		GearDisplayString = (Gear == 0) ? LOCTEXT("N", "N") : FText::AsNumber(Gear);
	}	
}

void AEngineBrakePawn::ClearPopupMessage()
{
	PopupString = FText::GetEmpty();
}

bool AEngineBrakePawn::CheckLowSpeedThreshold()
{
	int Gear = GetVehicleMovement()->GetCurrentGear();

	// Only for forward gears
	if (Gear < 1)
		return false;

	// Translate into km / h
	float Speed = GetVehicleMovement()->GetForwardSpeed() * 0.036f;

	return Speed > 0 && Speed < MinGearSpeeds[Gear];
}

void AEngineBrakePawn::StallEngine()
{
	bRunningEngine = false;

	// Switch to neutral as the engine stops
	GetVehicleMovement()->SetTargetGear(0, true);
	GetVehicleMovement()->SetThrottleInput(0);

	// Score penalty for engine stalling
	// It's a constant because later in the game progress it will be hard to focus on engine stuff, so let it be less punishing
	PlayerState->Score -= 100.0f;

	// Set up the popup string
	PopupString = FText::FromString("Engine stalled");
	// Clear this after 3 sec
	GetWorld()->GetTimerManager().ClearTimer(PopupCleanerTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(PopupCleanerTimerHandle, this, &AEngineBrakePawn::ClearPopupMessage, 3.0f);
}

void AEngineBrakePawn::StartEngine()
{
	if (!bRunningEngine && !bOutOfFuel)
		bRunningEngine = true;
}
#undef LOCTEXT_NAMESPACE
