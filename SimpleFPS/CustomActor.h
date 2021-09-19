// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CustomActor.generated.h"

UCLASS()
class SIMPLEFPS_API ACustomActor : public AActor {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACustomActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// variable to store the mesh inside
	UPROPERTY(EditAnywhere, Category = "Components") // Macro to tell unreal where this can be edited from and stuff
	UStaticMeshComponent* StaticMesh;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
