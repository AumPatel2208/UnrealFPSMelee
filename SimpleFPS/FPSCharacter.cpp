// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSCharacter.h"


#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AFPSCharacter::AFPSCharacter() {
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Orient the Character to the forward direction of movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 500.0f;
	GetCharacterMovement()->AirControl = 0.7f; // plenty of air control, might tone down later.
	GetCharacterMovement()->GravityScale = fGravityScale;
	GetCharacterMovement()->GroundFriction = fNormalGroundFriction;

	// can't change mid game
	const int invInt = invertCamera ? -1 : 1;
	mouseSens *= invInt;

	// dash
	bToDash = false;
	fDashAmount = 10000.f;
	fDashDuration = 0.5f;
	fDashDistance = 800.0f;
	fDashSpeed = 8.0f;
	fDashHitSafeClipAmount = 0.05f;


}

// Called when the game starts or when spawned
void AFPSCharacter::BeginPlay() {
	Super::BeginPlay();

}

void AFPSCharacter::MoveForward(float amount) {
	// if ((Controller != nullptr) && (amount != 0.0f)) {
	// Find out which way is forward
	const FRotator rotation = Controller->GetControlRotation();
	const FRotator yawRotation(0, rotation.Yaw, 0);

	// apply to player input vector
	PlayerInput = FVector2D(PlayerInput.X, amount);

	// Get forward vector
	const FVector direction = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(direction, amount);
	// }
}

void AFPSCharacter::MoveRight(float amount) {
	// Find out which way is forward
	const FRotator rotation = Controller->GetControlRotation();
	const FRotator yawRotation(0, rotation.Yaw, 0);

	// Get right vector 
	const FVector direction = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y);

	// apply to player input vector
	PlayerInput = FVector2D(amount, PlayerInput.Y);

	// Add movement in that direction
	AddMovementInput(direction, amount);
}

void AFPSCharacter::Turn(float amount) {
	if (amount != 0.f && Controller && Controller->IsLocalPlayerController()) {
		APlayerController* const playerController = CastChecked<APlayerController>(Controller);

		playerController->AddYawInput(amount * mouseSens);
	}
}

void AFPSCharacter::LookUp(float amount) {
	if (amount != 0.f && Controller && Controller->IsLocalPlayerController()) {
		APlayerController* const playerController = CastChecked<APlayerController>(Controller);
		playerController->AddPitchInput(amount * mouseSens);
	}
}


void AFPSCharacter::Shoot() {
}

void AFPSCharacter::Jump() {
	// GetCharacterMovement()->DoJump(true);
	// ACharacter::Jump();
	Super::Jump();

}

void AFPSCharacter::Dash() {
	// will allow calling Dashing()
	bToDash = true;

	FHitResult OutHitPlayerFloor;
	FVector SweepStart = GetActorLocation();
	FVector SweepEnd = GetActorLocation() + (-GetActorLocation().UpVector * 100.f);
	FCollisionQueryParams CollisionParameters;
	CollisionParameters.AddIgnoredActor(this);
	bool isFloorCollision = GetWorld()->LineTraceSingleByChannel(OutHitPlayerFloor, SweepStart, SweepEnd, ECC_WorldStatic, CollisionParameters);
	FString toPrint;

	FVector dashForward = GetActorForwardVector();
	FVector dashRight = GetActorRightVector();

	bool isForwardInclined = false;
	bool isRightInclined = false;
	if (isFloorCollision) {
		// Getting perpendicular maths : https://answers.unity.com/questions/1662376/how-to-calculate-the-3d-vector-perpendicular-to-a.html

		// Draw Surface Normal
		// DrawDebugLine(GetWorld(), OutHitPlayerFloor.Location, OutHitPlayerFloor.Location + OutHitPlayerFloor.Normal * 100.f, FColor::Blue, true, 100, 0, 5);

		// Cross product of player-right & surface-normal will be surface-forward vector
		FVector surfaceForward = FVector::CrossProduct(GetActorRightVector(), OutHitPlayerFloor.Normal);

		//Draw Surface Forward
		// DrawDebugLine(GetWorld(), OutHitPlayerFloor.Location, OutHitPlayerFloor.Location + surfaceForward * 100.f, FColor::Red, true, 100, 0, 5);

		// check if the slope is inclined
		// dont apply surface forward if on a decline
		FVector inclinedFuturePos = GetActorLocation() + surfaceForward * 10.f;
		// check whether the surface is on an incline, and only then change the forward
		if (inclinedFuturePos.Z > GetActorLocation().Z) {
			isForwardInclined = true;
			dashForward = surfaceForward;
		}

		// Cross product of surface-forward & surface-normal will give surface-right vector
		FVector surfaceRight = FVector::CrossProduct(OutHitPlayerFloor.Normal, surfaceForward);

		// Draw Surface Right
		// DrawDebugLine(GetWorld(), OutHitPlayerFloor.Location, OutHitPlayerFloor.Location + surfaceRight * 100.f, FColor::Green, true, 100, 0, 5);

		inclinedFuturePos = GetActorLocation() + surfaceRight * 10.f;

		// check whether the surface is on an incline, and only then change the right
		if (inclinedFuturePos.Z > GetActorLocation().Z) {
			isRightInclined = true;
			dashRight = surfaceRight;
		}


	}

	if (PlayerInput.Equals(FVector2D(0.0f, 0.0f))) {
		// dashDirection = GetActorForwardVector();
		dashDirection = dashForward;
	}
	else {

		// reset the incline if it is going left or back on inclines.
		if(PlayerInput.Y < 0 && isForwardInclined) {
			dashForward = GetActorForwardVector();
		}
		if(PlayerInput.X < 0 && isRightInclined) {
			dashRight = GetActorRightVector();
		}
		
		dashDirection = dashForward * PlayerInput.Y + dashRight * PlayerInput.X;
		dashDirection.Normalize();
	}

	vDashStartDestination = GetActorLocation();
	vDashFinalDestination = (dashDirection * fDashDistance) + GetActorLocation();
	vDashWishFinalDestination = vDashFinalDestination;
	fDashLerpAlpha = 0.f;


	// collision pre-check
	// create a collision sphere to use for the check
	FCollisionShape colCapsule = FCollisionShape::MakeCapsule(GetCapsuleComponent()->GetScaledCapsuleRadius(), GetCapsuleComponent()->GetScaledCapsuleHalfHeight());

	// create array for hit results
	TArray<FHitResult> OutHitsDash;

	// start and end locations
	SweepStart = vDashStartDestination;
	SweepEnd = vDashFinalDestination;

	// check if something got hit in the sweep
	bool isHit = GetWorld()->SweepMultiByChannel(OutHitsDash, SweepStart, SweepEnd, FQuat::Identity, ECC_WorldStatic, colCapsule);
	// float dashReduction;
	fDashReductionRatio = 1.f;
	for (auto hit : OutHitsDash) {
		// toPrint += "\n" + hit.Actor->GetName();
		if (!hit.Actor->GetName().Equals(GetName())) {

			float reductionLength = (vDashFinalDestination - hit.Location).Size();
			fDashReductionRatio = reductionLength / fDashDistance;
			// calculate the ratio by which the distance is being reduced
			// reduce it by default .05f (might change) to prevent clipping
			fDashReductionRatio = 1.0f - fDashHitSafeClipAmount - fDashReductionRatio;

			vDashFinalDestination = hit.Location;
			break;
		}

	}

	// draw collision capsule
	// DrawDebugSphere(GetWorld(), GetActorLocation(), colCapsule.GetSphereRadius(), 50, FColor::Purple, true);
	// DrawDebugCapsule(GetWorld(), vDashStartDestination, colCapsule.GetCapsuleHalfHeight(), colCapsule.GetCapsuleRadius(), GetMesh()->GetComponentRotation().Quaternion(), FColor::Blue, true);
	// DrawDebugCapsule(GetWorld(), vDashFinalDestination, colCapsule.GetCapsuleHalfHeight(), colCapsule.GetCapsuleRadius(), GetMesh()->GetComponentRotation().Quaternion(), FColor::Blue, true);
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, (TEXT("%s"), toPrint));
	// auto dsasdsd = FMath::Lerp(GetActorLocation(), finalDestination);
}

void AFPSCharacter::Dashing(float DeltaTime) {

	fDashLerpAlpha += fDashSpeed * DeltaTime; // to make it constant

	// FVector lerpedLocation = FMath::Lerp(vDashStartDestination, vDashFinalDestination, fDashLerpAlpha);
	FVector lerpedLocation = FMath::Lerp(vDashStartDestination, vDashWishFinalDestination, fDashLerpAlpha);

	SetActorLocation(lerpedLocation);
	// FString toPrint = "Dash: " + vDashStartDestination.ToString() + "  " + vDashFinalDestination.ToString() + "    " + lerpedLocation.ToString() + "   " + FString::SanitizeFloat(fDashLerpAlpha);
	// GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, (TEXT("%s"), toPrint));
	if (fDashLerpAlpha >= fDashReductionRatio) {
		EndDash();
	}


}

void AFPSCharacter::EndDash() {
	// just makes it so that Dashing is no longer called
	bToDash = false;

}


// Called every frame
void AFPSCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (bToDash) {
		Dashing(DeltaTime);
	}
	FString toPrint = "Player Input: " + PlayerInput.ToString();
	GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Green, (TEXT("%s"), toPrint));
}

// Called to bind functionality to input
void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// "MoveForward" = the name of the input mapping in project settings
	// this = the method is in this object,
	// &MethodName = address of the method to call, the amount will be passed through
	PlayerInputComponent->BindAxis("MoveForward", this, &AFPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFPSCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &AFPSCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AFPSCharacter::LookUp);
	// PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	// PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	// if fire button pressed
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFPSCharacter::Shoot);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AFPSCharacter::Jump);
	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &AFPSCharacter::Dash);
}
