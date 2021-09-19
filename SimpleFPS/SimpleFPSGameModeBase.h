// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SimpleFPSGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class SIMPLEFPS_API ASimpleFPSGameModeBase : public AGameModeBase {
	GENERATED_BODY()

public:
	ASimpleFPSGameModeBase();

	void OnTargetHit();

	UPROPERTY(EditAnywhere, Category = "Game Rules")
	int32 pointsToWin;
	
};
