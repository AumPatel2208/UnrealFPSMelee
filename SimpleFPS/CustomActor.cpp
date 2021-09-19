// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomActor.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ACustomActor::ACustomActor() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// creating and adding component to the actor
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");


}

// Called when the game starts or when spawned
void ACustomActor::BeginPlay() {
	Super::BeginPlay();
	
	// scale randomly it at the beginning	
	StaticMesh->SetWorldScale3D(FMath::VRand()); // FMath VRand gives random vector
}

// Called every frame
void ACustomActor::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}
