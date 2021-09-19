// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/Pawn.h"
#include "SpherePawn.generated.h"

UCLASS()
class SIMPLEFPS_API ASpherePawn : public APawn {
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASpherePawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// movement
	void MoveForward(float amount);
	void MoveRight(float amount);
	void Turn(float amount);
	void LookUp(float amount);

	void Shoot();

	UPROPERTY(EditAnywhere, Category = "Shooting") 
	class TSubclassOf<class ABullet> BulletClass;
	
	// Forward declaration
	// networked and handles collision detection
	class UFloatingPawnMovement* FloatingPawnMovement;

	// variable to store the mesh inside
	UPROPERTY(EditAnywhere, Category = "Components") // Macro to tell unreal where this can be edited from and stuff
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditAnywhere, Category = "Components") // Macro to tell unreal where this can be edited from and stuff
	class UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, Category = "Components") // Macro to tell unreal where this can be edited from and stuff
	class USpringArmComponent* CameraArm; // spring arm

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
