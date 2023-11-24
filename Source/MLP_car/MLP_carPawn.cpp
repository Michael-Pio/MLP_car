// Copyright Epic Games, Inc. All Rights Reserved.

#include "MLP_carPawn.h"
#include "MLP_carWheelFront.h"
#include "MLP_carWheelRear.h"
#include "MLP_carHud.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "WheeledVehicleMovementComponent4W.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/Engine.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/TextRenderComponent.h"
#include "Materials/Material.h"
#include <Components/BoxComponent.h>
#include "GameFramework/Controller.h"

#ifndef HMD_MODULE_INCLUDED
#define HMD_MODULE_INCLUDED 0
#endif

// Needed for VR Headset
#if HMD_MODULE_INCLUDED
#include "IXRTrackingSystem.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#endif // HMD_MODULE_INCLUDED


#include <DrawDebugHelpers.h>

const FName AMLP_carPawn::LookUpBinding("LookUp");
const FName AMLP_carPawn::LookRightBinding("LookRight");

#define LOCTEXT_NAMESPACE "VehiclePawn"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

#define DEBMESSAGE(X) if (GEngine){GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Yellow, FString::Printf(TEXT("Value = %f"),X));}


AMLP_carPawn::AMLP_carPawn()
{

	// Car mesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CarMesh(TEXT("/Game/Vehicle/Sedan/Sedan_SkelMesh.Sedan_SkelMesh"));
	GetMesh()->SetSkeletalMesh(CarMesh.Object);

	static ConstructorHelpers::FClassFinder<UObject> AnimBPClass(TEXT("/Game/Vehicle/Sedan/Sedan_AnimBP"));
	GetMesh()->SetAnimInstanceClass(AnimBPClass.Class);
	
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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

	Vehicle4W->EngineSetup.MaxRPM = 2000.f ;
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
	
	// Colors for the incar gear display. One for normal one for reverse
	GearDisplayReverseColor = FColor(255, 0, 0, 255);
	GearDisplayColor = FColor(255, 255, 255, 255);

	// Colors for the in-car gear display. One for normal one for reverse
	GearDisplayReverseColor = FColor(255, 0, 0, 255);
	GearDisplayColor = FColor(255, 255, 255, 255);

	bInReverseGear = false;
	
	//Pio
	MyANN = NeuralNetwork(std::vector<unsigned int>{ 8, 8, 2 });
	TraceDistance = 2000.0f;
	RayCount = 7;

}


void AMLP_carPawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMLP_carPawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMLP_carPawn::MoveRight);
	PlayerInputComponent->BindAxis("LookUp");
	PlayerInputComponent->BindAxis("LookRight");

	PlayerInputComponent->BindAction("Handbrake", IE_Pressed, this, &AMLP_carPawn::OnHandbrakePressed);
	PlayerInputComponent->BindAction("Handbrake", IE_Released, this, &AMLP_carPawn::OnHandbrakeReleased);
	PlayerInputComponent->BindAction("SwitchCamera", IE_Pressed, this, &AMLP_carPawn::OnToggleCamera);
	PlayerInputComponent->BindAction("RayShoot", IE_Pressed, this, &AMLP_carPawn::OnRayShootPressed);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AMLP_carPawn::OnResetVR); 
}

void AMLP_carPawn::MoveForward(float Val)
{
	GetVehicleMovementComponent()->SetThrottleInput(Val);
}

void AMLP_carPawn::MoveRight(float Val)
{
	GetVehicleMovementComponent()->SetSteeringInput(Val);
}

void AMLP_carPawn::OnHandbrakePressed()
{
	GetVehicleMovementComponent()->SetHandbrakeInput(true);
}

void AMLP_carPawn::OnHandbrakeReleased()
{
	GetVehicleMovementComponent()->SetHandbrakeInput(false);
}

void AMLP_carPawn::OnToggleCamera()
{
	EnableIncarView(!bInCarCameraActive);
}

void AMLP_carPawn::EnableIncarView(const bool bState, const bool bForce)
{
	if ((bState != bInCarCameraActive) || ( bForce == true ))
	{
		bInCarCameraActive = bState;
		
		if (bState == true)
		{
			OnResetVR();
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

void AMLP_carPawn::Tick(float Delta)
{
	Super::Tick(Delta);
	// Setup the flag to say we are in reverse gear
	bInReverseGear = GetVehicleMovement()->GetCurrentGear() < 0;
	// Update the strings used in the hud (incar and onscreen)
	
	UpdateHUDStrings();

	// Set the string in the incar hud
	SetupInCarHUD();
	bool bHMDActive = false;

	updateFitnessScore(&FitnessScore);
	ANN_OUT(MyANN);

#if HMD_MODULE_INCLUDED
	if ((GEngine->XRSystem.IsValid() == true) && ((GEngine->XRSystem->IsHeadTrackingAllowed() == true) || (GEngine->IsStereoscopic3D() == true)))
	{
		bHMDActive = true;
	}
#endif // HMD_MODULE_INCLUDED
	if (bHMDActive == false)
	{
		if ( (InputComponent) && (bInCarCameraActive == true ))
		{
			FRotator HeadRotation = InternalCamera->GetRelativeRotation();
			HeadRotation.Pitch += InputComponent->GetAxisValue(LookUpBinding);
			HeadRotation.Yaw += InputComponent->GetAxisValue(LookRightBinding);
			InternalCamera->SetRelativeRotation(HeadRotation);
		}
	}
	
}

void AMLP_carPawn::Destroyed()
{
	FitnessScore /= 3600;
	MyANN.saveModel("Best.csv");
	DEBMESSAGE(FitnessScore);
}

void AMLP_carPawn::BeginPlay()
{
	Super::BeginPlay();
	this->SetActorEnableCollision(false);
	bool bEnableInCar = false;
	MyANN = NeuralNetwork(std::vector<unsigned int>{ 8, 8, 2 });
	MyANN.Mutate(0.2, 0.3);
	
#if HMD_MODULE_INCLUDED
	bEnableInCar = UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled();
#endif // HMD_MODULE_INCLUDED
	EnableIncarView(bEnableInCar,true);
}

void AMLP_carPawn::OnResetVR()
{
#if HMD_MODULE_INCLUDED
	if (GEngine->XRSystem.IsValid())
	{
		GEngine->XRSystem->ResetOrientationAndPosition();
		InternalCamera->SetRelativeLocation(InternalCameraOrigin);
		GetController()->SetControlRotation(FRotator());
	}
#endif // HMD_MODULE_INCLUDED
}

TArray<double> AMLP_carPawn::RayTrace() //~~Pio
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
		float angle = 180.f / (RayCount -1);
		for (int i = 0; i < RayCount; i++) {
			FVector EndOfRay = SensorOrigin + (FRotator(SensorOrientation.Pitch,SensorOrientation.Yaw - 90.f + (angle * i), SensorOrientation.Roll).Vector() * TraceDistance);
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

void AMLP_carPawn::UpdateHUDStrings()
{
	float KPH = FMath::Abs(GetVehicleMovement()->GetForwardSpeed()) * 0.036f;
	int32 KPH_int = FMath::FloorToInt(KPH);

	// Using FText because this is display text that should be localizable
	SpeedDisplayString = FText::Format(LOCTEXT("SpeedFormat", "{0} km/h"), FText::AsNumber(KPH_int));
	
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

float AMLP_carPawn::GetFitnessScore()
{
	return FitnessScore;
}

void AMLP_carPawn::updateFitnessScore(float *FitNessRef)
{
	CarSpeed = FMath::Abs(GetVehicleMovement()->GetForwardSpeed()) * 0.036f;
	*FitNessRef += CarSpeed;
}

void AMLP_carPawn::SetupInCarHUD()
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

void AMLP_carPawn::OnRayShootPressed()
{
	RayTrace();
}

void AMLP_carPawn::ANN_OUT(NeuralNetwork copyNN) {
	std::vector<double> input;
	TArray<double> Tinput = RayTrace();

	input.resize(Tinput.Num()+1);
	input[0] = CarSpeed;
	for (int i = 1; i < Tinput.Num()+1; i++) {
		input[i] = Tinput[i-1];
	}

	std::vector<double> out1234 = copyNN.feedForward(input);
	if (out1234.size() !=0 ) {
		MoveForward(out1234[0]);
		MoveRight(out1234[1]);
	}
}

#undef LOCTEXT_NAMESPACE

PRAGMA_ENABLE_DEPRECATION_WARNINGS
