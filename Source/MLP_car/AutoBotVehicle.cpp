// Feel Free to copy my works ,Have a great day -Michael Pio 


#include "AutoBotVehicle.h"
#include "MLP_carWheelFront.h"
#include "MLP_carWheelRear.h"
#include "UObject/ConstructorHelpers.h"
#include "MLP_SpectatorPawn.h"

#include "Components/SkeletalMeshComponent.h"
#include "WheeledVehicleMovementComponent4W.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/Engine.h"
#include "UObject/ConstructorHelpers.h"
#include "Materials/Material.h" 
#include <Components/BoxComponent.h>
#include "GameFramework/Controller.h"

#include <DrawDebugHelpers.h>


#define LOCTEXT_NAMESPACE "VehiclePawn"

#define DEBMESSAGE(X) if (GEngine){GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Yellow, FString::Printf(TEXT("Value = %f"),X));}

PRAGMA_DISABLE_DEPRECATION_WARNINGS

AAutoBotVehicle::AAutoBotVehicle()
{

	// Car TextRenderComponent (RANK):
	RankTextRendComp = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Rotating Text"));
	RankTextRendComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	RankTextRendComp->SetText(FString("Pio"));
	RankTextRendComp->AddLocalOffset(FVector(0, 0, 150));
	RankTextRendComp->SetWorldSize(80.0f);
	RankTextRendComp->SetTextRenderColor(FColor::Red);
	RankTextRendComp->SetHorizontalAlignment(EHTA_Center);
	
	// Car mesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CarMesh(TEXT("/Game/Vehicle/Sedan/Sedan_SkelMesh.Sedan_SkelMesh"));
	GetMesh()->SetSkeletalMesh(CarMesh.Object);

	static ConstructorHelpers::FClassFinder<UObject> AnimBPClass(TEXT("/Game/Vehicle/Sedan/Sedan_AnimBP"));
	GetMesh()->SetAnimInstanceClass(AnimBPClass.Class);

	// Simulation
	UWheeledVehicleMovementComponent4W* Vehicle4W = CastChecked<UWheeledVehicleMovementComponent4W>(GetVehicleMovement());

	
	check(Vehicle4W->WheelSetups.Num() == 4);
	Vehicle4W->WheelSetups[0].WheelClass = UMLP_carWheelFront::StaticClass();
	Vehicle4W->WheelSetups[0].BoneName = FName("Wheel_Front_Left");
	Vehicle4W->WheelSetups[0].AdditionalOffset = FVector(0.f, -12.f, 0.f);

	Vehicle4W->WheelSetups[1].WheelClass = UMLP_carWheelFront::StaticClass();
	Vehicle4W->WheelSetups[1].BoneName = FName("Wheel_Front_Right");
	Vehicle4W->WheelSetups[1].AdditionalOffset = FVector(0.f, 12.f, 0.f);

	Vehicle4W->WheelSetups[2].WheelClass = UMLP_carWheelRear::StaticClass();
	Vehicle4W->WheelSetups[2].BoneName = FName("Wheel_Rear_Left");
	Vehicle4W->WheelSetups[2].AdditionalOffset = FVector(0.f, -12.f, 0.f);

	Vehicle4W->WheelSetups[3].WheelClass = UMLP_carWheelRear::StaticClass();
	Vehicle4W->WheelSetups[3].BoneName = FName("Wheel_Rear_Right");
	Vehicle4W->WheelSetups[3].AdditionalOffset = FVector(0.f, 12.f, 0.f);

	Vehicle4W->EngineSetup.MaxRPM = 4000.f;
	bInReverseGear = false;

	//Pio
	MyANN = NeuralNetwork(std::vector<unsigned int>{ 8, 8, 2 });
	TraceDistance = 2000.0f;
	RayCount = 7;
}

void AAutoBotVehicle::MoveForward(float val) {
	GetVehicleMovementComponent()->SetThrottleInput(val);
}

void AAutoBotVehicle::MoveRight(float val) {
	GetVehicleMovementComponent()->SetSteeringInput(val);
}



void AAutoBotVehicle::Tick(float Delta) {
	Super::Tick(Delta);
	// Setup the flag to say we are in reverse gear
	bInReverseGear = GetVehicleMovement()->GetCurrentGear() < 0;
	updateRankTextOrientation();
	updateFitnessScore(&FitnessScore);
	ANN_OUT(MyANN);
}

void AAutoBotVehicle::Destroyed()
{
	FitnessScore /= 3600;
	MyANN.saveModel("Best.csv");
	DEBMESSAGE(FitnessScore);
}

void AAutoBotVehicle::BeginPlay()
{
	Super::BeginPlay();
	this->SetActorEnableCollision(true);


	myGameInstance = Cast<UMLP_GameInstance>(GetGameInstance());
	myMlp_SpecPawnInstance = Cast<AMLP_SpectatorPawn>(UGameplayStatics::GetPlayerPawn(this, 0));

	if (myGameInstance->CurrentGeneration == 1) {
		MyANN = NeuralNetwork(std::vector<unsigned int>{ 8, 8, 2 });
		MyANN.Mutate(0.2, 0.3);
	}
	else {
		MyANN = myGameInstance->BestNN;
		MyANN.Mutate(myGameInstance->MutationRate, 0.05);
	}
}

void AAutoBotVehicle::updateRankTextOrientation()
{
	
	if (myMlp_SpecPawnInstance) {
		playerRot = myMlp_SpecPawnInstance->GetViewRotation();
		playerRot.Yaw += 180.f;
		playerRot.Pitch *= -1.f;
	}
	updateRankText();
	RankTextRendComp->SetWorldRotation(playerRot);

}

void AAutoBotVehicle::updateRankText()
{
	const FString rankdispstr = (const char*)"hello"+this->Rank;
	RankTextRendComp->SetText(rankdispstr);
}

TArray<double> AAutoBotVehicle::RayTrace() //~~Pio
{
	TArray<double> SensorReadings; //our sensor readings;

	/** Getting the position and orientation of the Sensor socket **/
	FVector SensorOrigin = GetMesh()->GetSocketLocation(TEXT("Sensor_Base"));
	FRotator SensorOrientation = GetMesh()->GetSocketRotation(TEXT("Sensor_Base"));

	FHitResult HitRes;
	FCollisionQueryParams TraceParams;

	double distance = 0;

	if (RayCount <= 1) {

		FVector SensorEnd = SensorOrigin + (SensorOrientation.Vector() * TraceDistance);

		bool bHit = GetWorld()->LineTraceSingleByChannel(HitRes, SensorOrigin, SensorEnd, ECC_EngineTraceChannel2, TraceParams);
		if (ShowRays == true) {
			DrawDebugLine(GetWorld(), SensorOrigin, SensorEnd, FColor::Orange, false);
		}

		if (bHit) {
			if (ShowRays) {
				DrawDebugBox(GetWorld(), HitRes.ImpactPoint, FVector(5, 5, 5), FColor::Emerald, false, 1.0f);
			}
			distance = TraceDistance - HitRes.Distance;
			DEBMESSAGE(distance);
		}
		SensorReadings.Add(distance);
	}
	else {
		float angle = 180.f / (RayCount - 1);
		for (int i = 0; i < RayCount; i++) {
			FVector EndOfRay = SensorOrigin + (FRotator(SensorOrientation.Pitch, SensorOrientation.Yaw - 90.f + (angle * i), SensorOrientation.Roll).Vector() * TraceDistance);
			bool bHit = GetWorld()->LineTraceSingleByChannel(HitRes, SensorOrigin, EndOfRay, ECC_Visibility, TraceParams);
			if (ShowRays) {
				DrawDebugLine(GetWorld(), SensorOrigin, EndOfRay, FColor::Orange, false, 1.f);
			}
			if (bHit) {
				if (ShowRays) {
					DrawDebugBox(GetWorld(), HitRes.ImpactPoint, FVector(5, 5, 5), FColor::Emerald, false, 1.0f);
				}
				distance = TraceDistance - HitRes.Distance;
			}
			SensorReadings.Add(distance);
		}
	}
	return SensorReadings;
	//~~Pio
}

float AAutoBotVehicle::GetFitnessScore()
{
	return FitnessScore;
}

void AAutoBotVehicle::OnRayShootPressed()
{
	RayTrace();
}

void AAutoBotVehicle::updateFitnessScore(float* FitNessRef)
{
	CarSpeed = FMath::Abs(GetVehicleMovement()->GetForwardSpeed()) * 0.036f;
	*FitNessRef += CarSpeed/3600;
}

void AAutoBotVehicle::ANN_OUT(NeuralNetwork copyNN) {
	std::vector<double> input;
	TArray<double> Tinput = RayTrace();

	input.resize(Tinput.Num() + 1);
	input[0] = CarSpeed;
	for (int i = 1; i < Tinput.Num() + 1; i++) {
		input[i] = Tinput[i - 1];
	}

	std::vector<double> out1234 = copyNN.feedForward(input);
	if (out1234.size() != 0) {
		MoveForward(out1234[0]);
		MoveRight(out1234[1]);
	}
}

#undef LOCTEXT_NAMESPACE

PRAGMA_ENABLE_DEPRECATION_WARNINGS