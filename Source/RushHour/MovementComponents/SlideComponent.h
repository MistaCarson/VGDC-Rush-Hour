// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SlideComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RUSHHOUR_API USlideComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USlideComponent();
	void Initialize(ACharacter* player);
	void EndSlide();
	bool IsSliding();
	void TryBeginSlide();
	void CharacterLanded();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	ACharacter* Character;
	class UCharacterMovementComponent* CharacterMovement;
	bool Sliding = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BlueprintProtected = "true"), Category = "Sliding")
	float BeginSlideSpeedThreshold = 400;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BlueprintProtected = "true"), Category = "Sliding")
	float MaxAngleSlide = 45;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BlueprintProtected = "true"), Category = "Sliding")
	float MaxSlideSpeed = 3000;

private:
	float DefaultFriction;
		
};
