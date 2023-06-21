// Fill out your copyright notice in the Description page of Project Settings.


#include "DashComponent.h"
#include "RushHour/RushHourCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values for this component's properties
UDashComponent::UDashComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UDashComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UDashComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	UE_LOG(LogTemp, Warning, TEXT("Dashing: %d"), Dashing);
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (Dashing && DashTimeLeft > 0) {
		DashTimeLeft -= DeltaTime;
		if (DashTimeLeft <= 0) {
			Dashing = false;
			FVector DashingVelocity = PreDashDirection * PreDashSpeed;
			CharacterMovement->Velocity = DashingVelocity;
		}
		else {
			FVector DashingVelocity = PreDashDirection * CurrentDashSpeed;
			CharacterMovement->Velocity = DashingVelocity;
		}


	}
}

void UDashComponent::Initialize(ARushHourCharacter* player) {
	Character = player;
	CharacterMovement = Character->GetCharacterMovement();
}

void UDashComponent::Dash(int DashType) {
	if (!Dashing) {
		PreDashSpeed = CharacterMovement->Velocity.Length();
		PreDashDirection = Character->GetFirstPersonCameraComponent()->GetForwardVector();
		switch (DashType) {
		case 0:
			CurrentDashSpeed = PreDashSpeed;
			break;
		case 1:
			CurrentDashSpeed = PreDashSpeed * 1.3;
			break;
		case 2:
			CurrentDashSpeed = DashSpeed;
			break;
		default:
			break;
		}
		DashTimeLeft = DashTime;
		Dashing = true;
	}

}

void UDashComponent::CancelDash() {
	Dashing = false;
	UE_LOG(LogTemp, Warning, TEXT("HIT: Dash Cancelled"));
}

bool UDashComponent::IsDashing() {
	return Dashing;
}