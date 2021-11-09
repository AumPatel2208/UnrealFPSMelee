// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSCharacter.h"


#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Utility/BoisMaths.h"
#include "SimpleFPS/Dash.h"


// Sets default values
AFPSCharacter::AFPSCharacter() {
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Orient the Character to the forward direction of movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate              = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity             = 500.0f;
	GetCharacterMovement()->AirControl                = 0.7f; // plenty of air control, might tone down later.
	GetCharacterMovement()->GravityScale              = gravityScale;
	GetCharacterMovement()->GroundFriction            = normalGroundFriction;
	JumpMaxCount                                      = 2;

	// can't change mid game
	const int invInt = invertCamera ? -1 : 1;
	mouseSens *= invInt;

	// dash
	dashComponent = CreateDefaultSubobject<UDash>(TEXT("Dash Componenet"));


}

// Called when the game starts or when spawned
void AFPSCharacter::BeginPlay() {
	Super::BeginPlay();

	dashComponent->SetCapsuleRadius(GetCapsuleComponent()->GetScaledCapsuleRadius());
	dashComponent->SetCapsuleHalfHeight(GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	
}

void AFPSCharacter::MoveForward(float amount) {
	// if ((Controller != nullptr) && (amount != 0.0f)) {
	// Find out which way is forward
	const FRotator rotation = Controller->GetControlRotation();
	const FRotator yawRotation(0, rotation.Yaw, 0);

	// apply to player input vector
	playerInput = FVector2D(playerInput.X, amount);

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
	playerInput = FVector2D(amount, playerInput.Y);

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
	// GEngine->AddOnScreenDebugMessage();

	dashComponent->DashBegin(playerInput);
}


// // called during the dash
// void AFPSCharacter::Dashing(float DeltaTime) {
//
// 	dashLerpAlpha += dashSpeed * DeltaTime; // to make it constant
//
// 	// FVector lerpedLocation = FMath::Lerp(vDashStartDestination, vDashWishFinalDestination, fDashLerpAlpha);
// 	FVector lerpedLocation = BoisMaths::Slerp(dashStartDestination, dashWishFinalDestination, dashLerpAlpha);
// 	SetActorLocation(lerpedLocation);
// 	// FString toPrint = "Dash: " + vDashStartDestination.ToString() + "  " + vDashFinalDestination.ToString() + "    " + lerpedLocation.ToString() + "   " + FString::SanitizeFloat(fDashLerpAlpha);
// 	// GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, (TEXT("%s"), toPrint));
// 	if (dashLerpAlpha >= dashReductionRatio) {
// 		EndDash();
// 	}
//
//
// }

// // Called when dash ends
// void AFPSCharacter::EndDash() {
// 	// just makes it so that Dashing is no longer called
// 	bToDash = false;
//
// 	// to get out of clipped states just move the character at the end of the dash
// 	MoveForward(1.0f); // HACK might be a little hacky, but shouldn't presents issues
// }


// Called every frame
void AFPSCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	// if (bToDash) {
	// 	Dashing(DeltaTime);
	// }

	// FString toPrint = "Jump Max: " + FString::FromInt(JumpMaxCount) + "\n";
	// toPrint += "Jump Count: " + FString::FromInt(JumpCurrentCount) + "\n";
	// GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Green, (TEXT("%s"), toPrint));
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
