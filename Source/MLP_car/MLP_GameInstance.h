// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GenerationDetails.h"
#include "MLP_GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class MLP_CAR_API UMLP_GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PresetTraining)
	int Population;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Training)
	int CurrentGeneration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PresetTraining)
	int SelectionMethod;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PresetTraining)
	float MutationRate;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Training)
	float CurrentAverageFitness;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Training)
	float CurrentMaxFitness;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Training)
	float CurrentAlive;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = Training)
	TArray<FGenerationDetails> AllGenerationDetatils;


public:
	NeuralNetwork BestNN;

public:
	UMLP_GameInstance();
};
