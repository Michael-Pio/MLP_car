// Fill out your copyright notice in the Description page of Project Settings.


#include "MLP_SpectatorPawn.h"
#include <Kismet/GameplayStatics.h>



#define DEBMESSAGE(X) if (GEngine){GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Yellow, FString::Printf(TEXT(X)));}


AMLP_SpectatorPawn::AMLP_SpectatorPawn()
{
	
}


