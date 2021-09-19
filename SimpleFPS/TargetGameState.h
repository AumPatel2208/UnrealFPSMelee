// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "TargetGameState.generated.h"

/**
 * 
 */
UCLASS()
class SIMPLEFPS_API ATargetGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ATargetGameState();
	int32 points;
};
