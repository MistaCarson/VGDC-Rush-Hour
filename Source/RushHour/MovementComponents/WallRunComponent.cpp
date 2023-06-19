// Fill out your copyright notice in the Description page of Project Settings.


#include "WallRunComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Math/UnrealMathUtility.h"
#if WITH_EDITOR
#include "DrawDebugHelpers.h"
#endif


UWallRunComponent::UWallRunComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UWallRunComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UWallRunComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (WallRunningRight) {
		CameraTilt(-CameraTiltAmount);
	}
	else if (WallRunningLeft) {
		CameraTilt(CameraTiltAmount);
	}
	else {
		CameraTilt(0);
	}
		if (WallRunOnCooldown) {
		return;
	}	
	TArray<FVector> vectors = GetRightLeftVectors();
	if (WallRunMovement(vectors[0], 1)) {
		WallRunning = true;
		WallRunningRight = true;
		WallRunningLeft = false;
		CharacterMovement->AirControl = 0;
		if (UseGravity) {
			CharacterMovement->GravityScale = FMath::FInterpTo(CharacterMovement->GravityScale, WallRunTargetGravity, DeltaTime, 10);
		} else {
			CharacterMovement->GravityScale = FMath::FInterpTo(CharacterMovement->GravityScale, 0, DeltaTime, 10);
			CharacterMovement->Velocity.Z = FMath::FInterpTo(CharacterMovement->Velocity.Z, 0, DeltaTime, 10);
		}
	}
	else if (WallRunningRight) {
		WallRunEnd();
	}
	else {
		if (WallRunMovement(vectors[1], -1)) {
			WallRunning = true;
			WallRunningRight = false;
			WallRunningLeft = true;
			CharacterMovement->AirControl = 0;
			if (UseGravity) {
				CharacterMovement->GravityScale = FMath::FInterpTo(CharacterMovement->GravityScale, WallRunTargetGravity, DeltaTime, 10);
			} else {
				CharacterMovement->GravityScale = FMath::FInterpTo(CharacterMovement->GravityScale, 0, DeltaTime, 10);
				CharacterMovement->Velocity.Z = FMath::FInterpTo(CharacterMovement->Velocity.Z, 0, DeltaTime, 10);
			}
		}
		else {
			WallRunEnd();
		}
	}
}

void UWallRunComponent::CameraTilt(float roll) {
	FRotator current = Character->GetController()->GetControlRotation();
	FRotator goalRotator = FRotator(current.Pitch, current.Yaw, roll);
	Character->GetController()->SetControlRotation(FMath::RInterpTo(current, goalRotator, GetOwner()->GetWorld()->DeltaTimeSeconds, 10));

}

void UWallRunComponent::Initialize(ACharacter* player) {
	Character = player;
	CharacterMovement = Character->GetCharacterMovement();
	DefaultGravity = CharacterMovement->GravityScale;
}

void UWallRunComponent::Jump() {
	if (WallRunning) {
		WallRunEnd();
		FVector launch = FVector(WallRunNormal.X * WallRunJumpForce, WallRunNormal.Y * WallRunJumpForce, WallRunJumpHeight);
		Character->LaunchCharacter(launch, false, true);
	}

}

void UWallRunComponent::Land() {
	WallRunEnd();
	WallRunOnCooldown = false;
}

void UWallRunComponent::WallRunEnd() {
	if (!WallRunning) {
		return;
	}
	WallRunning = false;
	WallRunningRight = false;
	WallRunningLeft = false;
	WallRunOnCooldown = true;
	CharacterMovement->AirControl = 1;
	CharacterMovement->GravityScale = DefaultGravity;
	GetOwner()->GetWorldTimerManager().SetTimer(WallRunCooldownTimer, this, &UWallRunComponent::WallRunCooldownExpire, WallRunCooldown, false, WallRunCooldown);
}

TArray<FVector> UWallRunComponent::GetRightLeftVectors() {
	FVector CharacterLocation = Character->GetActorLocation();
	FVector CharacterRightVector = Character->GetActorRightVector();
	FVector CharacterForwardVector = Character->GetActorForwardVector();
	FVector RightVector = CharacterLocation + CharacterRightVector * 85 + CharacterForwardVector * -35;
	FVector LeftVector = CharacterLocation + CharacterRightVector * -85 + CharacterForwardVector * -35;
	TArray<FVector> Vectors = TArray<FVector>();
	Vectors.Add(RightVector);
	Vectors.Add(LeftVector);
	return Vectors;
}

bool UWallRunComponent::WallRunMovement(FVector ActionVector, float WallRunDirection) {
	FHitResult hit1;
	GetOwner()->GetWorld()->LineTraceSingleByChannel(hit1, Character->GetActorLocation(), ActionVector, ECollisionChannel::ECC_GameTraceChannel3); //Game Trace Channel 3 = Wall Run surface
	
#if WITH_EDITOR
	if (DrawRayCasts) {
		DrawDebugLine(GetWorld(), Character->GetActorLocation(), ActionVector, FColor::Red, false, 10.f);
	}
#endif
	if (hit1.bBlockingHit && IsValidWall(hit1.Normal) && CharacterMovement->IsFalling()) {
		WallRunNormal = hit1.Normal;
		//Character->LaunchCharacter(GetPlayerToWallVector(), false, false); //stick to wall
		//Character->LaunchCharacter(FVector::CrossProduct(FVector::UpVector, WallRunNormal) * WallRunForceMultiplier * WallRunDirection * ForwardInput, true, !UseGravity);//move forward
		FVector HorizontalVelocity = FVector(Character->GetVelocity().X, Character->GetVelocity().Y, 0);
		if (HorizontalVelocity.Length() <= MaxWallRunVelocity) {
			CharacterMovement->AddForce(FVector::CrossProduct(FVector::UpVector, WallRunNormal) * WallRunForceMultiplier * WallRunDirection * ForwardInput);
		}
		return true;
	}
	return false;
}

bool UWallRunComponent::IsValidWall(FVector HitVector) {
	if (HitVector.Z > -.52 && HitVector.Z < .52) {
		return true;
	}
	return false;
}

FVector UWallRunComponent::GetPlayerToWallVector() {
	return WallRunNormal * FVector::Dist(WallRunNormal, Character->GetActorLocation());
}

void UWallRunComponent::WallRunCooldownExpire() {
	GetOwner()->GetWorldTimerManager().ClearTimer(WallRunCooldownTimer);
	WallRunOnCooldown = false;
}

void UWallRunComponent::SetForwardInput(float forwardInput) {
	ForwardInput = forwardInput;
}

bool UWallRunComponent::IsWallRunning() {
	return WallRunning;
}