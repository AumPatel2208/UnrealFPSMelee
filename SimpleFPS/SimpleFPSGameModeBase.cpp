// Copyright Epic Games, Inc. All Rights Reserved.


#include "SimpleFPSGameModeBase.h"

#include "TargetGameState.h"

ASimpleFPSGameModeBase::ASimpleFPSGameModeBase() {
	pointsToWin = 5;
}

void ASimpleFPSGameModeBase::OnTargetHit() {
	// creating a variable while casting
	if(ATargetGameState* gs = Cast<ATargetGameState>(GameState)) {
		gs->points++;

		if(gs->points >= pointsToWin) {
			UE_LOG(LogTemp, Warning, TEXT("You Won the Game!"));
		}else {
			UE_LOG(LogTemp, Warning, TEXT("You scored a point, now you have %d points"), gs->points);
		}
	}
}

