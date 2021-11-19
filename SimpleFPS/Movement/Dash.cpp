// Fill out your copyright notice in the Description page of Project Settings.


#include "Dash.h"

#include "CollisionQueryParams.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SimpleFPS/Utility/BoisMaths.h"

// Sets default values for this component's properties
UDash::UDash() {
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;


	// dash
	bToDash           = false;
	maxDashForce      = 3000000.f;
	minDashForce      = 10000.f;
	remenantDashForce = 300.f;
	dashDuration      = 0.15f;
	maxDashCount      = 2;
	// dashCooldownDuration = 0.0f;

}


// Called when the game starts
void UDash::BeginPlay() {
	Super::BeginPlay();

	// ...

}


// Called every frame
void UDash::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bToDash) {
		Dashing(DeltaTime);
	}

	if (dashTimer > 0.0f) {
		dashTimer -= DeltaTime;
		if (dashTimer <= 0.0f) {
			DashEnd();
		}
	}

	if (!dashCoolDownTimers.IsEmpty()) {
		DashCooldown(DeltaTime);
	}

}

void UDash::DashCooldown(float DeltaTime) {
	float& timer = *dashCoolDownTimers.Peek();
	timer -= DeltaTime;
	if (timer <= 0.0f) {
		dashCount -= 1;           // allow another dash
		dashCoolDownTimers.Pop(); // remove the timer from the queue
	}
}


void UDash::DashBegin(const FVector2D& input) {

	// don't perform if already dashing
	if (dashTimer > 0.0f) {
		return;
	}
	if (dashCount + 1 > maxDashCount) {
		return;
	}
	else {
		dashCount += 1;
	}

	dashTimer = dashDuration;
	//
	UCharacterMovementComponent* movement  = GetOwner()->FindComponentByClass<UCharacterMovementComponent>();
	movementDefaults.BrakingFrictionFactor = movement->BrakingFrictionFactor;
	movementDefaults.GravityScale          = movement->GravityScale;
	movementDefaults.Velocity              = movement->Velocity;

	movement->BrakingFrictionFactor = 0.0f;
	movement->GravityScale          = 0.0f;


	bToDash = true;


	FHitResult OutHitPlayerFloor;
	FVector    SweepStart = GetOwner()->GetActorLocation();
	FVector    SweepEnd   = GetOwner()->GetActorLocation() + -(GetOwner()->GetActorUpVector() * 100.f);

	FCollisionQueryParams CollisionParameters;
	CollisionParameters.AddIgnoredActor(GetOwner());
	bool isFloorCollision = GetWorld()->LineTraceSingleByChannel(OutHitPlayerFloor, SweepStart, SweepEnd, ECC_WorldStatic, CollisionParameters);
	// FString toPrint;

	FVector dashForward = GetOwner()->GetActorForwardVector();
	FVector dashRight   = GetOwner()->GetActorRightVector();

	bool isForwardInclined = false;
	bool isRightInclined   = false;
	if (isFloorCollision) {
		// Getting perpendicular maths : https://answers.unity.com/questions/1662376/how-to-calculate-the-3d-vector-perpendicular-to-a.html

		// Draw Surface Normal
		if (bShowDebugSurfaceNormalLines)
			DrawDebugLine(GetWorld(), OutHitPlayerFloor.Location, OutHitPlayerFloor.Location + OutHitPlayerFloor.Normal * 100.f, FColor::Blue, true, 100, 0, 5);

		// Cross product of player-right & surface-normal will be surface-forward vector
		FVector surfaceForward = FVector::CrossProduct(GetOwner()->GetActorRightVector(), OutHitPlayerFloor.Normal);

		//Draw Surface Forward
		if (bShowDebugSurfaceNormalLines)
			DrawDebugLine(GetWorld(), OutHitPlayerFloor.Location, OutHitPlayerFloor.Location + surfaceForward * 100.f, FColor::Red, true, 100, 0, 5);

		// check if the slope is inclined
		// dont apply surface forward if on a decline
		FVector inclinedFuturePos = GetOwner()->GetActorLocation() + surfaceForward * 10.f;
		// check whether the surface is on an incline, and only then change the forward
		if (inclinedFuturePos.Z > GetOwner()->GetActorLocation().Z) {
			isForwardInclined = true;
			dashForward       = surfaceForward;
		}

		// Cross product of surface-forward & surface-normal will give surface-right vector
		FVector surfaceRight = FVector::CrossProduct(OutHitPlayerFloor.Normal, surfaceForward);

		// Draw Surface Right
		if (bShowDebugSurfaceNormalLines)
			DrawDebugLine(GetWorld(), OutHitPlayerFloor.Location, OutHitPlayerFloor.Location + surfaceRight * 100.f, FColor::Green, true, 100, 0, 5);

		inclinedFuturePos = GetOwner()->GetActorLocation() + surfaceRight * 10.f;

		// check whether the surface is on an incline, and only then change the right
		if (inclinedFuturePos.Z > GetOwner()->GetActorLocation().Z) {
			isRightInclined = true;
			dashRight       = surfaceRight;
		}


	}

	if (input.Equals(FVector2D(0.0f, 0.0f))) {
		// dashDirection = GetOwner().GetActorForwardVector();
		dashDirection = dashForward;
	}
	else {

		// reset the incline if it is going left or back on inclines.
		if (input.Y < 0 && isForwardInclined) {
			dashForward = GetOwner()->GetActorForwardVector();
		}
		if (input.X < 0 && isRightInclined) {
			dashRight = GetOwner()->GetActorRightVector();
		}

		dashDirection = dashForward * input.Y + dashRight * input.X;
		dashDirection.Normalize();
	}

	if (bShowDebugSurfaceNormalLines) {
		auto location = OutHitPlayerFloor.Location + GetOwner()->GetActorUpVector() * capsuleHalfHeight;
		DrawDebugLine(GetWorld(), location, location + dashDirection * 100.f, FColor::Green, true, 100, 0, 5);
	}
}

void UDash::Dashing(float DeltaTime) {
	float lerpAlpha;
	// should go up then down
	// if (dashTimer < dashDuration / 2) {
	// 	lerpAlpha = dashTimer / (dashDuration / 2);
	// }
	// else {
	// 	lerpAlpha = (dashTimer - dashDuration / 2) / (dashDuration / 2);
	// }

	// TODO: End Dash on a collision?

	
	lerpAlpha = dashTimer / dashDuration;

	float lerpedForce = FMath::CubicInterp(minDashForce, 0.0f, maxDashForce, 0.0f, lerpAlpha);

	// apply force
	UCharacterMovementComponent* movement = GetOwner()->FindComponentByClass<UCharacterMovementComponent>();
	movement->AddForce(dashDirection * lerpedForce);


	// when timer reaches 0, call dash end

}

void UDash::DashEnd() {
	UCharacterMovementComponent* movement = GetOwner()->FindComponentByClass<UCharacterMovementComponent>();

	// resetting back to what it was.
	movement->GravityScale          = movementDefaults.GravityScale;
	movement->BrakingFrictionFactor = movementDefaults.BrakingFrictionFactor;
	movement->Velocity              = movementDefaults.Velocity + dashDirection * remenantDashForce;


	// just makes it so that Dashing is no longer called
	bToDash = false;


	// send to dash cooldown
	dashCoolDownTimers.Enqueue(dashCooldownDuration);
}

void UDash::SetCapsuleHalfHeight(float halfHeight) {
	capsuleHalfHeight = halfHeight;
}

void UDash::SetCapsuleRadius(float radius) {
	capsuleRadius = radius;
}

int UDash::GetDashCount() {
	return dashCount;
}

int UDash::GetMaxDashCount() {
	return maxDashCount;
}

const float UDash::GetRunningDashCooldownTimer() {
	if (!dashCoolDownTimers.IsEmpty())
		return *dashCoolDownTimers.Peek();
	return 0.0f;
}
