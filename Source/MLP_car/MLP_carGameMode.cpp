// Copyright Epic Games, Inc. All Rights Reserved.

#include "MLP_carGameMode.h"
#include "MLP_carPawn.h"
#include "MLP_carHud.h"

#include "MLP_SpectatorPawn.h"
#include <DrawDebugHelpers.h>

#define DEBMESSAGE(X) if (GEngine){GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Yellow, FString::Printf(TEXT("Value = %f"),X));}



AMLP_carGameMode::AMLP_carGameMode()
{
	DefaultPawnClass = AMLP_SpectatorPawn::StaticClass();
	HUDClass = AMLP_carHud::StaticClass();
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, "Hello Pio MLP_GameMode constructed");
	}
	NeuralNetwork NN[10];
	NN[2].saveModel("Pio\\model1.csv");
	NN[0].displayDetails();
	 
}



void AMLP_carGameMode::BeginPlay()
{
	Super::BeginPlay();


}


void AMLP_carGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}