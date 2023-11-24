// Feel Free to copy my works ,Have a great day -Michael Pio 

#pragma once

#include "CoreMinimal.h"

#include "WheeledVehicle.h"
#include "MLP_GameInstance.h"
#include "Kismet/GameplayStatics.h"

#include "MLP_SpectatorPawn.h"

#include "Brain/NeuralNetwork.h"
#include <iostream>
#include "Components/TextRenderComponent.h"

#include <Engine/EngineTypes.h>
#include "AutoBotVehicle.generated.h"


PRAGMA_DISABLE_DEPRECATION_WARNINGS

UCLASS(config=Game)
class MLP_CAR_API AAutoBotVehicle : public AWheeledVehicle
{
	GENERATED_BODY()
	
public:
	NeuralNetwork MyANN = NeuralNetwork();
	UMLP_GameInstance* myGameInstance;

protected:
	FRotator playerRot;
	FVector playerLoc;

	//TraceDistance ~~Pio
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sensor")
		float TraceDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sensor")
		float CarSpeed;

	UPROPERTY(EditAnywhere, BluePrintReadOnly, Category = "Sensor")
		int RayCount;

	UPROPERTY(EditAnywhere, BluePrintReadOnly, Category = "Sensor")
		bool ShowRays;

	/** Text component for the Car Rank*/
	UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UTextRenderComponent* RankTextRendComp;

public:
	UPROPERTY(Category = Training, BluePrintReadWrite, EditAnywhere)
		float FitnessScore = 0;	
	
	UPROPERTY(Category = Training, BluePrintReadWrite, EditAnywhere)
		int Rank = 0;

	/** Are we in reverse gear */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly)
		bool bInReverseGear;

public:
	AAutoBotVehicle();
	virtual void Tick(float Delta) override;
	virtual void Destroyed() override;

protected:
	virtual void BeginPlay() override;
	void updateRankTextOrientation();
	void updateRankText();
	// Reference to the spectatorBP blueprint class
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spectator")
		TSubclassOf<AMLP_SpectatorPawn> SpectatorBPClass;

	AMLP_SpectatorPawn* myMlp_SpecPawnInstance;
public:
	void MoveForward(float Val);
	void MoveRight(float Val);
	float GetFitnessScore();
	void OnRayShootPressed();
	void ANN_OUT(NeuralNetwork copyNN);

	TArray<double> RayTrace();
private:
	void updateFitnessScore(float* FitNessRef);

};

PRAGMA_ENABLE_DEPRECATION_WARNINGS