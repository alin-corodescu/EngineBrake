// Fill out your copyright notice in the Description page of Project Settings.

#include "EngineBrake.h"
#include "VehicleAIPawn.h"
#include "EngineBrakeWheelFront.h"
#include "EngineBrakeWheelRear.h"
#include "VehicleAIController.h"


/* AI Include */
#include "Perception/PawnSensingComponent.h"
#include "BehaviorTree/BehaviorTree.h"

AVehicleAIPawn::AVehicleAIPawn()
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


	/* Sensing component. */
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	PawnSensingComp->SetPeripheralVisionAngle(60.0f);
	PawnSensingComp->SightRadius = 2000;
	PawnSensingComp->bOnlySensePlayers = false;

	// We need to set the AI Controller class
	AIControllerClass = AVehicleAIController::StaticClass();

	// Instantiate the behaviour tree here
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BT(TEXT("BehaviorTree'/Game/AI_Blueprints/VehicleAI_BT.VehicleAI_BT'"));
	if (BT.Succeeded())
	{
		BehaviorTree = BT.Object;
	}

}

void AVehicleAIPawn::BeginPlay()
{
	Super::BeginPlay();

	if (PawnSensingComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("Pawn Sensing comp is set properly"));
		// Set up the callback when we see a vehicle
		PawnSensingComp->OnSeePawn.AddDynamic(this, &AVehicleAIPawn::OnDetectVehicle);
	}
	// Set up the AI controller
}

void AVehicleAIPawn::OnDetectVehicle(APawn * Vehicle)
{
	UE_LOG(LogTemp, Warning, TEXT("Detected something"));
	// Announce the controller that we are seeing things
	AVehicleAIController* Controller = Cast<AVehicleAIController>(GetController());
	if (Controller)
	{
		Controller->SetObstacle(Vehicle);
	}
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Detected a vehicle"));
	}
}

USplineComponent * AVehicleAIPawn::GetFollowedPath()
{
	return SplineToFollow;
}

void AVehicleAIPawn::SetPathToFollow(USplineComponent * Spline)
{
	this->SplineToFollow = Spline;
}

void AVehicleAIPawn::Tick(float Delta)
{
	Super::Tick(Delta);
}