// Fill out your copyright notice in the Description page of Project Settings.


#include "Target.h"

// Sets default values
ATarget::ATarget() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATarget::BeginPlay() {
	Super::BeginPlay();

	// creating and adding component to the actor
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	// sets the root component in the hierarchy
	SetRootComponent(StaticMesh);

}

// Called every frame
void ATarget::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}
