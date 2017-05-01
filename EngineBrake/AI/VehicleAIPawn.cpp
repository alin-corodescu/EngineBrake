// Fill out your copyright notice in the Description page of Project Settings.

#include "EngineBrake.h"
#include "VehicleAIPawn.h"
#include "EngineBrakeWheelFront.h"
#include "EngineBrakeWheelRear.h"
#include "VehicleAIController.h"
#include "../RoadConstruction/EndlessTrackGenerator.h"
#include "../EngineBrakePawn.h"


/* AI Include */
#include "Perception/PawnSensingComponent.h"
#include "BehaviorTree/BehaviorTree.h"

void AVehicleAIPawn::OnHit(UPrimitiveComponent * HitComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit)
{
	if (OtherActor->IsA(this->StaticClass()) || OtherActor->IsA(AEngineBrakePawn::StaticClass()))
	{
		// Play effects
		UGameplayStatics::SpawnEmitterAtLocation(this, Explosion, GetActorLocation(), GetActorRotation(), true);
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());

		// Hide the mesh
		GetMesh()->SetVisibility(false);
		AVehicleAIPawn* OtherVehicle = Cast<AVehicleAIPawn>(OtherActor);
		// Hide the other vehicle if it is another AI Pawn
		if (OtherVehicle)
			OtherVehicle->GetMesh()->SetVisibility(false);
	}
	if (OtherActor->IsA(AEngineBrakePawn::StaticClass()))
	{
		// Hack to solve the collision problems
		AEngineBrakePawn* OtherVehicle = Cast<AEngineBrakePawn>(OtherActor);
		OtherVehicle->OnCollision(NULL, this, NULL, FVector::ZeroVector, FHitResult());
	}
}

AVehicleAIPawn::AVehicleAIPawn()
{
	// Car mesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CarMesh(TEXT("/Game/Vehicle/Sedan/Sedan_SkelMesh.Sedan_SkelMesh"));
	GetMesh()->SetSkeletalMesh(CarMesh.Object);

	static ConstructorHelpers::FClassFinder<UObject> AnimBPClass(TEXT("/Game/Vehicle/Sedan/Sedan_AnimBP"));
	GetMesh()->SetAnimInstanceClass(AnimBPClass.Class);

	// Set up collision callback
	GetMesh()->OnComponentHit.AddDynamic(this, &AVehicleAIPawn::OnHit);

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


	/* Sensing component. */
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	PawnSensingComp->SetPeripheralVisionAngle(60.0f);
	PawnSensingComp->SightRadius = 2000;
	PawnSensingComp->bOnlySensePlayers = false;



	// Set up AI
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AVehicleAIController::StaticClass();


	// Instantiate the behaviour tree here
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BT(TEXT("BehaviorTree'/Game/AI_Blueprints/VehicleAI_BT.VehicleAI_BT'"));
	if (BT.Succeeded())
	{
		BehaviorTree = BT.Object;
	}

	// Setup the audio component and allocate it a sound cue
	static ConstructorHelpers::FObjectFinder<USoundCue> SoundCue(TEXT("SoundCue'/Game/VehicleAdv/Sound/Engine_Loop_Cue.Engine_Loop_Cue'"));
	EngineSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("EngineSound"));
	EngineSoundComponent->SetSound(SoundCue.Object);
	EngineSoundComponent->SetupAttachment(GetMesh());

	// Set up the destroying effects
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ExplosionFinder(TEXT("ParticleSystem'/Game/StarterContent/Particles/P_Explosion.P_Explosion'"));
	if (ExplosionFinder.Succeeded())
	{
		Explosion = ExplosionFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> ExplosionSoundFinder(TEXT("SoundCue'/Game/StarterContent/Audio/Explosion_Cue.Explosion_Cue'"));
	if (ExplosionSoundFinder.Succeeded())
	{
		ExplosionSound = ExplosionSoundFinder.Object;
	}
}


void AVehicleAIPawn::BeginPlay()
{
	Super::BeginPlay();

	if (PawnSensingComp)
		// Set up the callback when we see a vehicle
		PawnSensingComp->OnSeePawn.AddDynamic(this, &AVehicleAIPawn::OnDetectVehicle);
}

void AVehicleAIPawn::OnDetectVehicle(APawn * Vehicle)
{
	// Announce the controller that we are seeing things
	AVehicleAIController* Controller = Cast<AVehicleAIController>(GetController());
	if (Controller)
		Controller->SetObstacle(Vehicle);
}

AEndlessTrackGenerator * AVehicleAIPawn::GetFollowedPath()
{
	return RoadToFollow;
}

void AVehicleAIPawn::SetPathToFollow(AEndlessTrackGenerator * RoadGenerator)
{
	this->RoadToFollow = RoadGenerator;
}

void AVehicleAIPawn::Tick(float Delta)
{
	Super::Tick(Delta);

	// Adjust Sight radius based on speed
	PawnSensingComp->SightRadius = GetVehicleMovementComponent()->GetForwardSpeed() * 5;
	if (PawnSensingComp->SightRadius < 2000) PawnSensingComp->SightRadius = 2000;

	// Pass the engine RPM to the sound component
	float RPMToAudioScale = 2500.0f / GetVehicleMovement()->GetEngineMaxRotationSpeed();
	EngineSoundComponent->SetFloatParameter("RPM", GetVehicleMovement()->GetEngineRotationSpeed()*RPMToAudioScale);
}