// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameModeBase.h"
#include "MLP_GameInstance.h"
#include "AutoBotVehicle.h"
#include "Brain/NeuralNetwork.h"
#include <vector>
#include "MLP_carGameMode.generated.h"

UCLASS(minimalapi)


class AMLP_carGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMLP_carGameMode();



public:
    // Override the default Tick function to implement the game logic
    virtual void Tick(float DeltaSeconds) override;

    // Override the default BeginPlay function to initialize the environment
    virtual void BeginPlay() override;
};



