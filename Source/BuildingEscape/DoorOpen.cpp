// Copyright Kuchinskas 2020


#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "DoorOpen.h"
#include "GameFramework/Actor.h"

// Sets default values for this component's properties
UDoorOpen::UDoorOpen()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UDoorOpen::BeginPlay()
{
	Super::BeginPlay();
	InitYaw = GetOwner()->GetActorRotation().Yaw;
	CurrentYaw = InitYaw;
	OpenAngle += InitYaw ;
	if (!PressurePlate) {
		UE_LOG(LogTemp, Error, TEXT("%s has the open door component on it, but no pressureplate set!"), *GetOwner()->GetName());
	}

	PersonOpenDoor = GetWorld()->GetFirstPlayerController()->GetPawn();
	
}


// Called every frame
void UDoorOpen::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (PressurePlate && PressurePlate->IsOverlappingActor(PersonOpenDoor)) {
		OpenDoor(DeltaTime);
		DoorLastOpened = GetWorld()->GetTimeSeconds();
	}
	else if(GetWorld()->GetTimeSeconds() - DoorLastOpened >= DoorCloseDelay){
		CloseDoor(DeltaTime);
	}
	
}

void UDoorOpen::OpenDoor(float DeltaTime) {
	// UE_LOG(LogTemp, Warning, TEXT("%s"), *GetOwner()->GetActorRotation().ToString());
	// UE_LOG(LogTemp, Warning, TEXT("Yaw is: %f"), GetOwner()->GetActorRotation().Yaw);

	CurrentYaw = FMath::FInterpTo(CurrentYaw, OpenAngle, DeltaTime, SpeedOfOpeningDoor);
	FRotator DoorRotation = GetOwner()->GetActorRotation();
	DoorRotation.Yaw = CurrentYaw;
	GetOwner()->SetActorRotation(DoorRotation);
	//FRotator OpenDoor = { 0.f, TargetYaw,0.f };
	//float CurrentYaw = GetOwner()->GetActorRotation().Yaw;


	// GetOwner()->SetActorRotation(OpenDoor);
	//FRotator CurrentRotation = GetOwner()->GetActorRotation();
	//CurrentRotation.Yaw = 90.f;
}

void UDoorOpen::CloseDoor(float DeltaTime) {
	CurrentYaw = FMath::FInterpTo(CurrentYaw,  InitYaw, DeltaTime, SpeeOfClosingDoor);
	FRotator DoorRotation = GetOwner()->GetActorRotation();
	DoorRotation.Yaw = CurrentYaw;
	GetOwner()->SetActorRotation(DoorRotation);
}

