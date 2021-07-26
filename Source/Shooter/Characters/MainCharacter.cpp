// Fill out your copyright notice in the Description page of Project Settings.

#include "MainCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"
#include "Sound/SoundCue.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "DrawDebugHelpers.h"
#include "particles/ParticleSystem.h"
#include "particles/ParticleSystemComponent.h"

// Sets default values
AMainCharacter::AMainCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Spring Arm"));
	CameraSpringArm->SetupAttachment(RootComponent, TEXT("Follow Camera Socket"));
	CameraSpringArm->TargetArmLength = 300.f;
	CameraSpringArm->bUsePawnControlRotation = true;
	CameraSpringArm->SocketOffset.Y = 60.f;
	CameraSpringArm->SocketOffset.Z = 75.f;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Follow Camera"));
	FollowCamera->SetupAttachment(CameraSpringArm);

	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;
	bUseControllerRotationPitch = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;
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

void AMainCharacter::GetOffsetYaw() 
{
	return;
}

void AMainCharacter::Shoot() 
{
	FTransform SocketTransform;
	if (GetMesh()->GetSocketByName("BarrelSocket")) {
		SocketTransform = GetMesh()->GetSocketTransform("BarrelSocket");
		if (MuzzleFlashCue) {
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), MuzzleFlashCue, SocketTransform.GetLocation());
		}
		if (MuzzleFlash) {
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);
		}
	}
	UAnimInstance* MainCharacterAnimInstance = GetMesh()->GetAnimInstance();
	if (MainCharacterAnimInstance && HipFireMontage) {
		MainCharacterAnimInstance->Montage_Play(HipFireMontage);
		MainCharacterAnimInstance->Montage_JumpToSection(FName("Shoot"));
	}
	FVector2D CrosshairScreenLocation;
	FVector CrosshairLocation;
	FVector CrosshairDirection;
	bool DeprojectSuccess;
	if(GEngine && GEngine->GameViewport) {
		GEngine->GameViewport->GetViewportSize(CrosshairScreenLocation);
		CrosshairScreenLocation.X /= 2;
		CrosshairScreenLocation.Y /= 2;
		CrosshairScreenLocation.Y -= 50.f;
	
		DeprojectSuccess = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(GetWorld(), 0), 
		CrosshairScreenLocation, 
		CrosshairLocation,
		CrosshairDirection);
	}

	LineTraceShoot(SocketTransform, CrosshairLocation, CrosshairDirection);

}

void AMainCharacter::LineTraceShoot(FTransform SocketTransform, FVector CrosshairLocation, FVector CrosshairDirection) 
{
	FHitResult ShootHitResult, CrossShootHitResult;
	FVector Start { SocketTransform.GetLocation() };
	FVector End { CrosshairLocation + CrosshairDirection*50'000.f };

	GetWorld()->LineTraceSingleByChannel(ShootHitResult, CrosshairLocation, End, ECollisionChannel::ECC_Visibility);
	

	if (ShootHitResult.bBlockingHit) {
		End = ShootHitResult.Location;
	}

	// UE_LOG(LogTemp, Warning, TEXT("END: %f, %f, %f"), End.X, End.Y, End.Z);
	GetWorld()->LineTraceSingleByChannel(CrossShootHitResult, SocketTransform.GetLocation(), End, ECollisionChannel::ECC_Visibility);

	if (CrossShootHitResult.bBlockingHit) {
		End = CrossShootHitResult.Location;
		// UE_LOG(LogTemp, Warning, TEXT("Hallo"));
	}
	// UE_LOG(LogTemp, Warning, TEXT("END: %f, %f, %f"), End.X, End.Y, End.Z);

	if (BulletImpact) {
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BulletImpact, End);
	}

	if (BulletTrail) {
		UParticleSystemComponent* Trail = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BulletTrail, SocketTransform);
		if (Trail) {
			Trail->SetVectorParameter(FName("Target"), End);
		}
	}

	// DrawDebugLine(GetWorld(), Start, End, FColor::Red, true, 5.0f, 0U, 5.f);
	// UE_LOG(LogTemp, Warning, TEXT("START: %f, %f, %f"), Start.X, Start.Y, Start.Z);
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
	PlayerInputComponent->BindAction("Shoot", IE_Pressed, this, &AMainCharacter::Shoot);
}
