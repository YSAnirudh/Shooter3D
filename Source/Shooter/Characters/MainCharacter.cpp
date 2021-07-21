// Fill out your copyright notice in the Description page of Project Settings.

#include "MainCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AMainCharacter::AMainCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Spring Arm"));
	CameraSpringArm->SetupAttachment(RootComponent, TEXT("Follow Camera Socket"));
	CameraSpringArm->TargetArmLength = 300.f;
	CameraSpringArm->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Follow Camera"));
	FollowCamera->SetupAttachment(CameraSpringArm);

	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationPitch = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AMainCharacter::MoveForward(float Value) 
{
	FRotator ControlYaw {0.0f, Controller->GetControlRotation().Yaw , 0.0f};
	FVector Direction { FRotationMatrix(ControlYaw).GetUnitAxis(EAxis::X) };

	AddMovementInput(Direction, GetWorld()->GetDeltaSeconds() * Value * MoveSpeed);
}

void AMainCharacter::MoveRight(float Value) 
{
	FRotator ControlYaw {0.0f, Controller->GetControlRotation().Yaw , 0.0f};
	FVector Direction { FRotationMatrix(ControlYaw).GetUnitAxis(EAxis::Y) };

	AddMovementInput(Direction, GetWorld()->GetDeltaSeconds() * Value * MoveSpeed);
}

void AMainCharacter::TurnRate(float Value) 
{
	AddControllerYawInput(Value * GetWorld()->GetDeltaSeconds() * TurnScale);
}

void AMainCharacter::LookUpRate(float Value) 
{
	// UE_LOG(LogTemp, Warning, TEXT("Hello, %d"), Value);
	AddControllerPitchInput(Value * GetWorld()->GetDeltaSeconds() * LookUpScale);
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainCharacter::MoveRight);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMainCharacter::TurnRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMainCharacter::LookUpRate);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
}
