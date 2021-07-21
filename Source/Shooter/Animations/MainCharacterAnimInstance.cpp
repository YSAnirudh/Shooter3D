// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacterAnimInstance.h"
#include "../Characters/MainCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UMainCharacterAnimInstance::UpdateAnimationProperties(float DeltaTime) 
{
    if (!MainCharacterRef) {
        MainCharacterRef = Cast<AMainCharacter>(TryGetPawnOwner());
    }
    if (MainCharacterRef) {
        FVector Velocity = MainCharacterRef->GetVelocity();
        Velocity.Z = 0;
        Speed = Velocity.Size();
        bIsInAir = MainCharacterRef->GetCharacterMovement()->IsFalling();
        bIsAccelarating = MainCharacterRef->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0 ? true : false;
    }
}

void UMainCharacterAnimInstance::NativeInitializeAnimation() 
{
    MainCharacterRef = Cast<AMainCharacter>(TryGetPawnOwner());
}
