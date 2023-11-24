// Feel Free to copy my works ,Have a great day -Michael Pio 

#pragma once

#include "Brain/NeuralNetwork.h"
#include "CoreMinimal.h"
#include "GenerationDetails.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct MLP_CAR_API FGenerationDetails
{
	GENERATED_BODY()

public:
	FGenerationDetails();
	FGenerationDetails(int Generation, float AverageFitness, float MaxFitness, NeuralNetwork NN_Winner, NeuralNetwork NN_Runner);

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category = GenerationData)
	int Generation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = GenerationData)
	float AverageFitness;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = GenerationData)
	float MaxFitness;

	NeuralNetwork NN_Winner;
	NeuralNetwork NN_Runner;

	~FGenerationDetails();
};
