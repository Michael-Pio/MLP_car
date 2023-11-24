// Fill out your copyright notice in the Description page of Project Settings.


#include "MLP_GameInstance.h"
#define DEBMESSAGE(X) if (GEngine){GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, FString::Printf(TEXT(X)));}

enum SelectionMethod { SINGLE = 0, CROSSOVER = 1 };

UMLP_GameInstance::UMLP_GameInstance()
{
	DEBMESSAGE("GsmeInstance Constructed");
	Population = 30;
	CurrentGeneration = 1;
	CurrentAverageFitness = -1.f;
	CurrentMaxFitness = -1.f;
	SelectionMethod = SINGLE;
	MutationRate = 0.10;
	
}
