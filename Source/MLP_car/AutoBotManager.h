// Feel Free to copy my works ,Have a great day -Michael Pio 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Brain/NeuralNetwork.h"
#include "AutoBotVehicle.h"
#include "AIController.h"
#include "MLP_GameInstance.h"
#include "AutoBotManager.generated.h"

UCLASS()
class MLP_CAR_API AAutoBotManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAutoBotManager();

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY()
	TArray<AAutoBotVehicle*> AutoBots;
	UPROPERTY()
	UMLP_GameInstance* myGameInstance;


	AAutoBotVehicle* bestAutoBot;
	float best = 0;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void MoveToNextGen();
	void UpdateActorCount();


	void RestartLevel();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;
};
