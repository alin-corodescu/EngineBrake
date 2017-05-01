// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "EngineBrake.h"
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
const FName AEngineBrakePawn::LookUpBinding("LookUp");
const FName AEngineBrakePawn::LookRightBinding("LookRight");

#define LOCTEXT_NAMESPACE "VehiclePawn"

AEngineBrakePawn::AEngineBrakePawn()
{
	// A bit of a hack, but need it
	//this->AutoPossessPlayer = EAutoReceiveInput::Player0;
	//this->PlayerState = CreateDefaultSubobject<APlayerState>(TEXT("PlayerState"));

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

	// Create In-Car camera component 
	InternalCameraOrigin = FVector(0.0f, -40.0f, 120.0f);

	InternalCameraBase = CreateDefaultSubobject<USceneComponent>(TEXT("InternalCameraBase"));
	InternalCameraBase->SetRelativeLocation(InternalCameraOrigin);
	InternalCameraBase->SetupAttachment(GetMesh());

	InternalCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("InternalCamera"));
	InternalCamera->bUsePawnControlRotation = false;
	InternalCamera->FieldOfView = 90.f;
	InternalCamera->SetupAttachment(InternalCameraBase);

	//Setup TextRenderMaterial
	static ConstructorHelpers::FObjectFinder<UMaterial> TextMaterial(TEXT("Material'/Engine/EngineMaterials/AntiAliasedTextMaterialTranslucent.AntiAliasedTextMaterialTranslucent'"));
	
	UMaterialInterface* Material = TextMaterial.Object;

	// Create text render component for in car speed display
	InCarSpeed = CreateDefaultSubobject<UTextRenderComponent>(TEXT("IncarSpeed"));
	InCarSpeed->SetTextMaterial(Material);
	InCarSpeed->SetRelativeLocation(FVector(70.0f, -75.0f, 99.0f));
	InCarSpeed->SetRelativeRotation(FRotator(18.0f, 180.0f, 0.0f));
	InCarSpeed->SetupAttachment(GetMesh());
	InCarSpeed->SetRelativeScale3D(FVector(1.0f, 0.4f, 0.4f));

	// Create text render component for in car gear display
	InCarGear = CreateDefaultSubobject<UTextRenderComponent>(TEXT("IncarGear"));
	InCarGear->SetTextMaterial(Material);
	InCarGear->SetRelativeLocation(FVector(66.0f, -9.0f, 95.0f));	
	InCarGear->SetRelativeRotation(FRotator(25.0f, 180.0f,0.0f));
	InCarGear->SetRelativeScale3D(FVector(1.0f, 0.4f, 0.4f));
	InCarGear->SetupAttachment(GetMesh());
	

	FuelSystem = CreateDefaultSubobject<UFuelSystemComponent>(TEXT("FuelSystem"));
	FuelSystem->SetupConsumptionParameters(this, ConsumptionCoefficient);
	// hope it is ok
	this->AddOwnedComponent(FuelSystem);

	// Colors for the incar gear display. One for normal one for reverse
	GearDisplayReverseColor = FColor(255, 0, 0, 255);
	GearDisplayColor = FColor(255, 255, 255, 255);

	// Colors for the in-car gear display. One for normal one for reverse
	GearDisplayReverseColor = FColor(255, 0, 0, 255);
	GearDisplayColor = FColor(255, 255, 255, 255);

	bInReverseGear = false;

	//! Engine is not running at the start, we have to start it manually
	bRunningEngine = false;

	bOutOfFuel = false;


	// Set up collision callback
	OnActorBeginOverlap.AddDynamic(this, &AEngineBrakePawn::OnOverlap);
	GetMesh()->OnComponentHit.AddDynamic(this, &AEngineBrakePawn::OnCollision);

	// Setup the audio component and allocate it a sound cue
	static ConstructorHelpers::FObjectFinder<USoundCue> SoundCue(TEXT("SoundCue'/Game/VehicleAdv/Sound/Engine_Loop_Cue.Engine_Loop_Cue'"));
	EngineSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("EngineSound"));
	EngineSoundComponent->SetSound(SoundCue.Object);
	EngineSoundComponent->SetupAttachment(GetMesh());


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
}

AEngineBrakePawn::~AEngineBrakePawn()
{
	if (Explosion && ExplosionSound) {
		UGameplayStatics::SpawnEmitterAtLocation(this, Explosion, GetActorLocation(), GetActorRotation(), true);
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
	}
}

void AEngineBrakePawn::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	// set up gameplay key bindings
	check(InputComponent);

	InputComponent->BindAxis("MoveForward", this, &AEngineBrakePawn::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AEngineBrakePawn::MoveRight);
	InputComponent->BindAxis("LookUp");
	InputComponent->BindAxis("LookRight");

	InputComponent->BindAction("Handbrake", IE_Pressed, this, &AEngineBrakePawn::OnHandbrakePressed);
	InputComponent->BindAction("Handbrake", IE_Released, this, &AEngineBrakePawn::OnHandbrakeReleased);
	InputComponent->BindAction("SwitchCamera", IE_Pressed, this, &AEngineBrakePawn::OnToggleCamera);
	InputComponent->BindAction("Request_Fuel", IE_Pressed, this, &AEngineBrakePawn::OnFuelRequest);

	InputComponent->BindAction("Upshift", IE_Pressed, this, &AEngineBrakePawn::OnUpShift);
	InputComponent->BindAction("Downshift", IE_Pressed, this, &AEngineBrakePawn::OnDownShift);
	InputComponent->BindAction("Reverse", IE_Pressed, this, &AEngineBrakePawn::OnReverseGear);
	InputComponent->BindAction("Neutral", IE_Pressed, this, &AEngineBrakePawn::OnNeutralGear);
	InputComponent->BindAction("StartEngine", IE_Pressed, this, &AEngineBrakePawn::StartEngine);
}

void AEngineBrakePawn::MoveForward(float Val)
{
	//! Only apply throttle if the engine is running, otherwise don't
	if (bRunningEngine)
	{
		bHasThrottleInput = true;
		if (bInReverseGear)
			Val = -Val;
		GetVehicleMovementComponent()->SetThrottleInput(Val);
		return;
	}
	bHasThrottleInput = false;
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

void AEngineBrakePawn::OnToggleCamera()
{
	EnableIncarView(!bInCarCameraActive);
}

void AEngineBrakePawn::OnFuelRequest()
{
	SpawnerRef->SpawnFuelPickup();
}

void AEngineBrakePawn::EnableIncarView(const bool bState, const bool bForce)
{
	if ((bState != bInCarCameraActive) || ( bForce == true ))
	{
		bInCarCameraActive = bState;
		
		if (bState == true)
		{
			Camera->Deactivate();
			InternalCamera->Activate();
		}
		else
		{
			InternalCamera->Deactivate();
			Camera->Activate();
		}
		
		InCarSpeed->SetVisibility(bInCarCameraActive);
		InCarGear->SetVisibility(bInCarCameraActive);
	}
}


void AEngineBrakePawn::Tick(float Delta)
{
	// Hope the input component ticks after this
	//bHasThrottleInput = false;

	Super::Tick(Delta);

	//if (InputComponent->GetAxisValue("MoveForward")) 
	//	bHasThrottleInput = true;
	//else bHasThrottleInput = false;

	// Check if we are still on track
	

	if (bRunningEngine && CheckLowSpeedThreshold()) 
	{
		UE_LOG(LogTemp, Warning, TEXT("Engine Stalled at gear %d and speed %d"),
			(int)GetVehicleMovement()->GetCurrentGear(), (int)GetVehicleMovement()->GetForwardSpeed());
		StallEngine();
	}
	// Setup the flag to say we are in reverse gear
	bInReverseGear = GetVehicleMovement()->GetCurrentGear() < 0;
	
	// Update the strings used in the hud (incar and onscreen)
	UpdateHUDStrings();

	// Set the string in the incar hud
	SetupInCarHUD();

	// Compute score depending on the RPM
	ScoreCalculator* Calculator = ScoreCalculator::GetInstance();

	float ScoreValue = Calculator->ComputeTickingScore(Delta);

	// Now update the score somehow
	PlayerState->Score += ScoreValue;

	// Pass the engine RPM to the sound component
	float RPMToAudioScale = 2500.0f / GetVehicleMovement()->GetEngineMaxRotationSpeed();
	EngineSoundComponent->SetFloatParameter("RPM", GetVehicleMovement()->GetEngineRotationSpeed()*RPMToAudioScale);
}

void AEngineBrakePawn::BeginPlay()
{
	Super::BeginPlay();

	bool bEnableInCar = false;
	EnableIncarView(bEnableInCar,true);
}

void AEngineBrakePawn::OnUpShift()
{
	// Find the current gear we are in
	int Gear = GetVehicleMovement()->GetCurrentGear();
	int MaxGear = MAX_GEAR;
	if (Gear < MaxGear)
	{
		// See if this works
		GetVehicleMovement()->SetTargetGear(Gear + 1, true);
		// If we are doing a normal upshift, add score to the total score
		if (Gear + 1 > 1)
		{
			// Compute score depending on the RPM
			ScoreCalculator* Calculator = ScoreCalculator::GetInstance();

			float ScoreValue = Calculator->ComputeUpshiftScore(GetVehicleMovementComponent()->GetEngineRotationSpeed());

			// Now update the score somehow
			PlayerState->Score += ScoreValue;

			if (GEngine && ScoreValue > 1)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Good upshift"));
			}
		}
	}
}

void AEngineBrakePawn::OnDownShift()
{
	// Find the current gear we are in
	int Gear = GetVehicleMovement()->GetCurrentGear();
	if (Gear > 0)
	{
		// See if this works
		GetVehicleMovement()->SetTargetGear(Gear - 1, true);
		//GetVehicleMovement()->SetGearUp(true);
	}
}

void AEngineBrakePawn::OnNeutralGear()
{
	GetVehicleMovement()->SetTargetGear(0, true);
	//GetVehicleMovement()->SetTargetGear(0, false);
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
		UGameplayStatics::SpawnEmitterAtLocation(this, Explosion, GetActorLocation(), GetActorRotation(), true);
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
		Camera->DetachFromParent(true);
		//FVector CameraPosition = GetActorLocation() + FVector(0, 0, 2000);
		//Camera->SetWorldLocationAndRotation(CameraPosition, (CameraPosition - GetActorLocation()).Rotation());
		GetMesh()->SetVisibility(false);
		//OtherActor->Destroy();
		//this->Destroy();
	}
}

void AEngineBrakePawn::OnOverlap(AActor * OverlappedActor, AActor * OtherActor)
{
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

	float RPM = GetVehicleMovement()->GetEngineRotationSpeed();
	RPMDisplayString = FText::Format(LOCTEXT("RPMFormat", "{0} RPM"), FText::AsNumber(RPM));

	float FuelPrecentage = FuelSystem->GetFuelPrecentage();
	FuelPrecentageDisplayString = FText::Format(LOCTEXT("FuelFormat", "{0}% FUEL"), FText::AsNumber(FuelPrecentage));

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

bool AEngineBrakePawn::CheckLowSpeedThreshold()
{
	int Gear = GetVehicleMovement()->GetCurrentGear();
	if (Gear < 1) return false;
	//! Translate into km / h 
	float Speed = GetVehicleMovement()->GetForwardSpeed() * 0.036f;

/*	UE_LOG(LogTemp, Warning, TEXT("Speed check at gear %d and speed %f, threshold being %d"),
		Gear, Speed, MinGearSpeeds[Gear]);*/
	return Speed < MinGearSpeeds[Gear];
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

	//! play some sounds and display something
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Engine Stalled"));
	}

}

void AEngineBrakePawn::StartEngine()
{
	if (!bRunningEngine && !bOutOfFuel)
	{
		bRunningEngine = true;
		// TODO: add some sounds
		// Debug some info to the screen if needed
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Engine Started"));
		}

	}
}

void AEngineBrakePawn::SetupInCarHUD()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if ((PlayerController != nullptr) && (InCarSpeed != nullptr) && (InCarGear != nullptr) )
	{
		// Setup the text render component strings
		InCarSpeed->SetText(SpeedDisplayString);
		InCarGear->SetText(GearDisplayString);
		
		if (bInReverseGear == false)
		{
			InCarGear->SetTextRenderColor(GearDisplayColor);
		}
		else
		{
			InCarGear->SetTextRenderColor(GearDisplayReverseColor);
		}
	}
}

#undef LOCTEXT_NAMESPACE
