// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"

#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ABullet::ABullet() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// initialising the components
	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>("BulletMesh");
	SetRootComponent(BulletMesh);

	BulletMovement = CreateDefaultSubobject<UProjectileMovementComponent>("BulletMovement");
	BulletMovement-> InitialSpeed = 500.f;
	BulletMovement->MaxSpeed = 500.f;
	
	// Delegate
	// calling it on this object
	OnActorHit.AddDynamic(this, &ABullet::OnBulletHit);
}

// Called when the game starts or when spawned
void ABullet::BeginPlay() {
	Super::BeginPlay();

}

// Called every frame
void ABullet::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void ABullet::OnBulletHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit) {
	Destroy();
}
