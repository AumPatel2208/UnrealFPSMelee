// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.generated.h"

UCLASS()
class SIMPLEFPS_API ABullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABullet();

	UPROPERTY(EditAnywhere)
	float damageValue = 10.0f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category= "Components")
	UStaticMeshComponent* BulletMesh;

	UPROPERTY(EditAnywhere, Category= "Components")
	class UProjectileMovementComponent* BulletMovement;
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

		UFUNCTION()
	void OnBulletHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit );

};
