// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSCharacter.h"


#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Utility/BoisMaths.h"
#include "SimpleFPS/Movement/Dash.h"
#include "SimpleFPS/Stats/Health.h"


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
	dashComponent   = CreateDefaultSubobject<UDash>(TEXT("Dash Componenet"));
	healthComponent = CreateDefaultSubobject<UHealth>(TEXT("Health Componenet"));


}

// Called when the game starts or when spawned
void AFPSCharacter::BeginPlay() {
	Super::BeginPlay();

	dashComponent->SetCapsuleRadius(GetCapsuleComponent()->GetScaledCapsuleRadius());
	dashComponent->SetCapsuleHalfHeight(GetCapsuleComponent()->GetScaledCapsuleHalfHeight());

}

#pragma region Movmement
// Movement controls

// Forward Backwards movement
void AFPSCharacter::MoveForward(float amount) {
	// Find out which way is forward
	const FRotator rotation = Controller->GetControlRotation();
	const FRotator yawRotation(0, rotation.Yaw, 0);

	// apply to player input vector
	playerInput = FVector2D(playerInput.X, amount);

	// Get forward vector
	const FVector direction = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(direction, amount);
}

// Right Left movement
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

// Turning the player
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

void AFPSCharacter::Jump() {
	Super::Jump();
}

void AFPSCharacter::Dash() {
	
	dashComponent->DashBegin(playerInput);
}
#pragma endregion Movement


void AFPSCharacter::Shoot() {
}




void AFPSCharacter::DisplayTempHud() const {
	FString healthToPrint = "HP  : ";

	healthToPrint += FString::SanitizeFloat(healthComponent->GetHealth());
	GEngine->AddOnScreenDebugMessage(-1, 0.001f, FColor::Red, (TEXT("%s"), healthToPrint));

	FString dashToPrint = "Dash: ";
	dashToPrint += FString::FromInt(dashComponent->GetMaxDashCount() - dashComponent->GetDashCount());
	dashToPrint += "\ntime: ";
	dashToPrint += FString::SanitizeFloat(dashComponent->GetRunningDashCooldownTimer());
	GEngine->AddOnScreenDebugMessage(-1, 0.001f, FColor::Green, (TEXT("%s"), dashToPrint));

}

// Called every frame
void AFPSCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	DisplayTempHud();
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
