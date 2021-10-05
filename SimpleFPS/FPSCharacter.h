// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPSCharacter.generated.h"

UCLASS()
class SIMPLEFPS_API AFPSCharacter : public ACharacter {
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFPSCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// movement
	void MoveForward(float amount);
	void MoveRight(float amount);
	void Turn(float amount);
	void LookUp(float amount);
	void Shoot();
	void Jump();
	void Dash();
	UFUNCTION() // to allow the function to be be bound on a timer
	void Dashing(float DeltaTime);
	void EndDash();
	
private:
	bool invertCamera = false;
	float mouseSens = 1.0f;

	// character atributes
	float fGravityScale = 1.5f;
	float fNormalGroundFriction = 8.0f;

	FVector2D PlayerInput = FVector2D(0,0);
	
	// dashing
	float fDashAmount;
	bool bToDash;
	float fDashDuration;
	float fDashDistance;
	float fDashSpeed;
	float fDashLerpAlpha;
	float fDashReductionRatio;
	float fDashHitSafeClipAmount; // reduce the dash distance by this value if there is an obstacle in the way to prevent clipping

	
	FVector vDashStartDestination;
	FVector vDashFinalDestination;
	FVector vDashWishFinalDestination;
	FVector dashDirection;
	FTimerHandle dashTimerHandle;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
