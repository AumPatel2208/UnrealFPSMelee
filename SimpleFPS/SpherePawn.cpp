// Fill out your copyright notice in the Description page of Project Settings.


#include "SpherePawn.h"

#include "Bullet.h"
#include "Components/InputComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
ASpherePawn::ASpherePawn() {
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FloatingPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>("PawnMovement");

	// creating and adding component to the actor
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");

	CameraArm = CreateDefaultSubobject<USpringArmComponent>("CameraSpringArm");
	CameraArm->SetupAttachment(StaticMesh); // attach static mesh to camera
	CameraArm->TargetArmLength = 500.f; // the length of the arm

	// create camera component
	Camera = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	Camera->SetupAttachment(CameraArm); // attach to the spring camera arm


	// sets the root component in the hierarchy
	SetRootComponent(StaticMesh);

	// enabled so that whenever the Yaw and pitch change in the controller, they change here as well.
	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = true;


}

// Called when the game starts or when spawned
void ASpherePawn::BeginPlay() {
	Super::BeginPlay();

}

void ASpherePawn::MoveForward(float amount) {
	// FloatingPawnMovement->AddInputVector(GetActorForwardVector() * amount);
	
	// Find out which way is "forward" and record that the player wants to move that way.
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
	AddMovementInput(Direction, amount);
}

void ASpherePawn::MoveRight(float amount) {
	FloatingPawnMovement->AddInputVector(GetActorRightVector() * amount);
}

void ASpherePawn::Turn(float amount) {
	AddControllerYawInput(amount);
}

void ASpherePawn::LookUp(float amount) {
	AddControllerPitchInput(-amount);
}

void ASpherePawn::Shoot() {

	if (BulletClass != nullptr) {
		FActorSpawnParameters spawnParameters;
		spawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		spawnParameters.bNoFail = true;
		spawnParameters.Owner = this;
		spawnParameters.Instigator = this; // the actor responsible for spawning

		FTransform bulletSpawnTransform;
		bulletSpawnTransform.SetLocation(GetActorForwardVector() * 500.f + GetActorLocation());
		// 500 units in front of the player
		bulletSpawnTransform.SetRotation(GetActorRotation().Quaternion());
		bulletSpawnTransform.SetScale3D(FVector(1.f));


		GetWorld()->SpawnActor<ABullet>(BulletClass, bulletSpawnTransform, spawnParameters);
	}
}

// Called every frame
void ASpherePawn::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
// Inputs are handled every frame
void ASpherePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// "MoveForward" = the name of the input mapping in project settings
	// this = the method is in this object,
	// &MethodName = address of the method to call, the amount will be passed through
	PlayerInputComponent->BindAxis("MoveForward", this, &ASpherePawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASpherePawn::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &ASpherePawn::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ASpherePawn::LookUp);

	// if fire button pressed
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASpherePawn::Shoot);
}
