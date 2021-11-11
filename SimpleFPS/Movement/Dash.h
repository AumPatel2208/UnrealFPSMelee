// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Dash.generated.h"

class World;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SIMPLEFPS_API UDash : public UActorComponent {
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UDash();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void DashBegin(const FVector2D& input);
	void Dashing(float DeltaTime);
	void DashEnd();

	void SetCapsuleHalfHeight(float halfHeight);
	void SetCapsuleRadius(float radius);

private:
	// Dashing
	bool         bToDash;
	float        dashAmount;
	float        dashSpeed;
	// float        dashDuration;
	float        dashDistance;
	float        dashLerpAlpha;
	float        dashReductionRatio;
	float        dashHitSafeClipAmount; // reduce the dash distance by this value if there is an obstacle in the way to prevent clipping
	FVector      dashStartDestination;
	FVector      dashFinalDestination;
	FVector      dashWishFinalDestination;
	FVector      dashDirection;
	FTimerHandle dashTimerHandle;

	// capsule height and radius for capsule-raycast check
	float capsuleHalfHeight, capsuleRadius;

public:
	// Debug
	UPROPERTY(EditAnywhere)
	bool bShowDebugCapsule;

	UPROPERTY(EditAnywhere)
	bool bShowDebugSurfaceNormalLines;
	
};
