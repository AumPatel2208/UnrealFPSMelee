// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Dash.generated.h"

class World;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SIMPLEFPS_API UDash : public UActorComponent {
	GENERATED_BODY()

	struct MovementDefaults {
		FVector Velocity;
		float   BrakingFrictionFactor;
		float   GravityScale;
	};

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

	int         GetDashCount();
	int         GetMaxDashCount();
	const float GetRunningDashCooldownTimer();

private:
	// Dashing
	bool bToDash; // whether to call Dashing();

	UPROPERTY(VisibleAnywhere)
	float         maxDashForce;      // max dash force to intereplate between
	float         minDashForce;      // min dash force to interpolate between
	float         dashDuration;      // time to dash for
	float         remenantDashForce; // force to apply after the dash is over.
	FVector       dashDirection;     // direction to dash in, calculated in DashBegin()
	float         dashTimer;
	int           maxDashCount;
	int           dashCount; // current count of the dash
	const float   dashCooldownDuration = 0.8f; // cooldown duration
	TQueue<float> dashCoolDownTimers;


	// capsule height and radius for capsule-raycast check
	float capsuleHalfHeight, capsuleRadius;

	// defaults to reset the Character Momvemnt Component's parameters to
	MovementDefaults movementDefaults;

	void DashCooldown(float DeltaTime);

public:
	UPROPERTY(EditAnywhere)
	bool bShowDebugSurfaceNormalLines;


};
