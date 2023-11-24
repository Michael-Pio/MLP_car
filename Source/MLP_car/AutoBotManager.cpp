// Feel Free to copy my works ,Have a great day -Michael Pio 

#include "Kismet/GameplayStatics.h"
#include "AutoBotManager.h"

#define DEBMESSAGE(X) if (GEngine){GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, FString::Printf(TEXT(X)));}


// Sets default values
AAutoBotManager::AAutoBotManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Spawner"));

}

// Called when the game starts or when spawned
void AAutoBotManager::BeginPlay()
{
	Super::BeginPlay();
	DEBMESSAGE("AutobotManager Constructed")
	myGameInstance = Cast<UMLP_GameInstance>(GetGameInstance());
	SetActorEnableCollision(false);
	FVector location = GetActorLocation();
	//FVector location = FVector(7780.000000, -30665.000000, 270.000000);
	FRotator rotation = GetActorRotation();
	//FRotator rotation = FRotator(0, 90, 0);
	int pop = myGameInstance->Population;
	for (int i = 0; i < pop; i++) {
		AAutoBotVehicle* myAutoBot = GetWorld()->SpawnActor<AAutoBotVehicle>(AAutoBotVehicle::StaticClass(), location, rotation);
		AAIController* myAutoBotController = GetWorld()->SpawnActor<AAIController>(AAIController::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
		
		if (myAutoBot != nullptr && myAutoBotController != nullptr)
		{
			// Possess the pawn with the AI controller
			myAutoBotController->Possess(myAutoBot);
			AutoBots.Add(myAutoBot);
		}
	}
}

void AAutoBotManager::UpdateActorCount()
{
	float sum = 0;
	TArray<AAutoBotVehicle*> AliveBots;
	for (AAutoBotVehicle* bot : AutoBots) {
		if (bot && !bot->IsPendingKill()) 
		{
			AliveBots.Add(bot);
			sum += bot->FitnessScore;
			if (best < bot->FitnessScore) {
				best = bot->FitnessScore;
				if (AutoBots.Num() > 0) {
					bestAutoBot = bot;
				}
			}
		}
	}
	AutoBots = AliveBots;
	myGameInstance->CurrentAlive = AliveBots.Num();
	myGameInstance->CurrentAverageFitness = sum / (AutoBots.Num()==0?1:AutoBots.Num());
	myGameInstance->CurrentMaxFitness = best;
}

// Called every frame
void AAutoBotManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateActorCount();
	MoveToNextGen();

}

void AAutoBotManager::MoveToNextGen()
{
	if (AutoBots.Num() == 0) {
		myGameInstance->CurrentGeneration++;
		RestartLevel();
	}
}

void AAutoBotManager::RestartLevel()
{
	// Get a reference to the current level's name
	FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(GetWorld());
	myGameInstance->BestNN = NeuralNetwork(bestAutoBot->MyANN);
	// Open the current level again to restart it
	UGameplayStatics::OpenLevel(GetWorld(), FName(*CurrentLevelName));
}


void AAutoBotManager::Destroyed()
{
	DEBMESSAGE("AutoBotManagerDestroyed")
}



