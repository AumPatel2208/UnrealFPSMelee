// Fill out your copyright notice in the Description page of Project Settings.


#include "Dash.h"

#include "CollisionQueryParams.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Utility/BoisMaths.h"

// Sets default values for this component's properties
UDash::UDash() {
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	
	// dash
	bToDash    = false;
	dashAmount = 10000.f;
	// dashDuration          = 0.5f;
	dashDistance          = 500.0f;
	dashSpeed             = 8.0f;
	dashHitSafeClipAmount = 0.65f;
}


// Called when the game starts
void UDash::BeginPlay() {
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UDash::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(bToDash) {
		Dashing(DeltaTime);
	}

	
	// ...
}

void UDash::DashBegin(const FVector2D& input) {
		// will allow calling Dashing()

	bToDash = true;


	/* TODO: Dash
	 * Clipping has been fixed by not dashing if a certain distance away.
	 * The way the current dashing works may introduce isses when dashing into enemies,
	 * 		- The Dash Collision check is calculated when shift is pressed, so within the duration from Button down to dash end, any new collision will not be checked
	 * 			- Solution: Real-time collision checking rather than at the beginning
	 * 		- Dash effect on collision with enemies will need to be calculated.
	*/

	FHitResult OutHitPlayerFloor;
	FVector    SweepStart = GetOwner()->GetActorLocation();
	FVector    SweepEnd   = GetOwner()->GetActorLocation() + -(GetOwner()->GetActorUpVector() * 100.f);

	FCollisionQueryParams CollisionParameters;
	CollisionParameters.AddIgnoredActor(GetOwner());
	bool    isFloorCollision = GetWorld()->LineTraceSingleByChannel(OutHitPlayerFloor, SweepStart, SweepEnd, ECC_WorldStatic, CollisionParameters);
	FString toPrint;

	FVector dashForward = GetOwner()->GetActorForwardVector();
	FVector dashRight   = GetOwner()->GetActorRightVector();

	bool isForwardInclined = false;
	bool isRightInclined   = false;
	if (isFloorCollision) {
		// Getting perpendicular maths : https://answers.unity.com/questions/1662376/how-to-calculate-the-3d-vector-perpendicular-to-a.html

		// Draw Surface Normal
		DrawDebugLine(GetWorld(), OutHitPlayerFloor.Location, OutHitPlayerFloor.Location + OutHitPlayerFloor.Normal * 100.f, FColor::Blue, true, 100, 0, 5);

		// Cross product of player-right & surface-normal will be surface-forward vector
		FVector surfaceForward = FVector::CrossProduct(GetOwner()->GetActorRightVector(), OutHitPlayerFloor.Normal);

		//Draw Surface Forward
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

	dashStartDestination     = GetOwner()->GetActorLocation();
	dashFinalDestination     = (dashDirection * dashDistance) + GetOwner()->GetActorLocation();
	dashWishFinalDestination = dashFinalDestination;
	dashLerpAlpha            = 0.f;


	// collision pre-check
	// create a collision sphere to use for the check
	FCollisionShape colCapsule = FCollisionShape::MakeCapsule(capsuleRadius, capsuleHalfHeight);

	// create array for hit results
	TArray<FHitResult> OutHitsDash;

	// start and end locations
	SweepStart = dashStartDestination;
	SweepEnd   = dashFinalDestination;

	// check if something got hit in the sweep
	bool isHit = GetWorld()->SweepMultiByChannel(OutHitsDash, SweepStart, SweepEnd, FQuat::Identity, ECC_WorldStatic, colCapsule);
	// float dashReduction;
	dashReductionRatio = 1.f;
	for (auto hit : OutHitsDash) {
		toPrint += "\n" + hit.Actor->GetName();
		if (!hit.Actor->GetName().Equals(GetOwner()->GetName())) {
			
			// don't dash if the object is too close
			if (FVector::Distance(hit.Location, dashStartDestination) <= dashHitSafeClipAmount) {
				bToDash = false; // 
				break; // exit
			}
			
			float reductionLength = (dashFinalDestination - hit.Location).Size();
			dashReductionRatio    = reductionLength / dashDistance;
			dashReductionRatio    = 1.0f - dashReductionRatio;

			dashFinalDestination = hit.Location;

		
			break; // exit as no need checking other collisions.
		}

	}

	// draw collision capsule
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, (TEXT("%s"), "bvasda"));

	DrawDebugCapsule(GetWorld(), dashStartDestination, capsuleHalfHeight, capsuleRadius, FQuat::Identity, FColor::Blue, true);
	DrawDebugCapsule(GetWorld(), dashFinalDestination, capsuleHalfHeight, capsuleRadius,  FQuat::Identity, FColor::Blue, true);
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, (TEXT("%s"), toPrint));
	// auto dsasdsd = FMath::Lerp(actor.GetActorLocation(), finalDestination);
}

void UDash::Dashing(float DeltaTime) {
	dashLerpAlpha += dashSpeed * DeltaTime; // to make it constant

	// FVector lerpedLocation = FMath::Lerp(vDashStartDestination, vDashWishFinalDestination, fDashLerpAlpha);
	FVector lerpedLocation = BoisMaths::Slerp(dashStartDestination, dashWishFinalDestination, dashLerpAlpha);
	GetOwner()->SetActorLocation(lerpedLocation);
	// FString toPrint = "Dash: " + vDashStartDestination.ToString() + "  " + vDashFinalDestination.ToString() + "    " + lerpedLocation.ToString() + "   " + FString::SanitizeFloat(fDashLerpAlpha);
	// GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, (TEXT("%s"), toPrint));
	if (dashLerpAlpha >= dashReductionRatio) {
		DashEnd();
	}
}

void UDash::DashEnd() {
	// just makes it so that Dashing is no longer called
	bToDash = false;

	// to get out of clipped states just move the character at the end of the dash
	// GetOwner()->MoveForward(1.0f); // HACK might be a little hacky, but shouldn't presents issues
}

void UDash::SetCapsuleHalfHeight(float halfHeight) {
	capsuleHalfHeight = halfHeight;
}

void UDash::SetCapsuleRadius(float radius) {
	capsuleRadius = radius;
}

