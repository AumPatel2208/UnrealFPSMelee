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
	
	// Movement
	void MoveForward(float amount);
	void MoveRight(float amount);
	void Turn(float amount);
	void LookUp(float amount);
	// Actions
	void Shoot();
	void Jump();
	void Dash();
	
	// UFUNCTION() // to allow the function to be be bound on a timer
	void Dashing(float DeltaTime);
	void EndDash();

	void DisplayTempHud() const;

private:
	// Camera
	bool  invertCamera = false;
	float mouseSens    = 1.0f;

	// Character atributes
	float gravityScale         = 1.8f;
	float normalGroundFriction = 8.0f;

	// Input
	FVector2D playerInput = FVector2D(0, 0);



public:
	UPROPERTY(EditAnywhere, Category = "Components") // Macro to tell unreal where this can be edited from and stuff
	class UDash* dashComponent;

	UPROPERTY(EditAnywhere, Category = "Components") // Macro to tell unreal where this can be edited from and stuff
	class UHealth* healthComponent;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
};
