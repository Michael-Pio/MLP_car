// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/HUD.h"
#include "MLP_carHud.generated.h"


UCLASS(config = Game)
class AMLP_carHud : public AHUD
{
	GENERATED_BODY()

public:
	AMLP_carHud();

	/** Font used to render the vehicle info */
	UPROPERTY()
	UFont* HUDFont;

	// Begin AHUD interface
	virtual void DrawHUD() override;
	// End AHUD interface
};
