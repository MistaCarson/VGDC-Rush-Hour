// Fill out your copyright notice in the Description page of Project Settings.


#include "SlideComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
USlideComponent::USlideComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USlideComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
void USlideComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Sliding) {
		FHitResult hit1;
		GetOwner()->GetWorld()->LineTraceSingleByChannel(hit1, Character->GetActorLocation(), Character->GetActorLocation() + 100*FVector::DownVector, ECollisionChannel::ECC_Visibility);
		if (hit1.bBlockingHit) {
			FVector GroundNormal = hit1.ImpactNormal;
			float Slope = FMath::Acos(GroundNormal.Z);
			UE_LOG(LogTemp, Warning, TEXT("Slope: %f"), Slope);
		}
	}
}

void USlideComponent::Initialize(ACharacter* player) {
	Character = player;
	CharacterMovement = Character->GetCharacterMovement();
	DefaultFriction = CharacterMovement->GroundFriction;
}

bool USlideComponent::IsSliding() {
	return Sliding;
}

void USlideComponent::TryBeginSlide() {
	if (CharacterMovement->IsFalling()) {

	}
	else {
		FVector2D HorizontalVelocity = FVector2D(CharacterMovement->Velocity.X, CharacterMovement->Velocity.Y);
		if (HorizontalVelocity.Length() > BeginSlideSpeedThreshold) {
			Sliding = true;
			CharacterMovement->GroundFriction = .2;
		}
	}
}

void USlideComponent::EndSlide() {
	Sliding = false;
	CharacterMovement->GroundFriction = DefaultFriction;
}

void USlideComponent::CharacterLanded() {

}