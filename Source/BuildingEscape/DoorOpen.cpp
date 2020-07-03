// Copyright Kuchinskas 2020

#include "DoorOpen.h"
#include "Components/AudioComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
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
	FindPressurePlate();
	FindAudioComponent();
}

void UDoorOpen::FindAudioComponent() {
	AudioComponent = GetOwner()->FindComponentByClass<UAudioComponent>();
	if (!AudioComponent) {
		UE_LOG(LogTemp, Error, TEXT("%s missind audio component!"), *GetOwner()->GetName())
	}
}
void UDoorOpen::FindPressurePlate() {
	if (!PressurePlate) {
		UE_LOG(LogTemp, Error, TEXT("%s has the open door component on it, but no pressureplate set!"), *GetOwner()->GetName());
	}
}

// Called every frame
void UDoorOpen::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (PressurePlate && TotalMassOfActors() > MassToOpenDoors)
	{
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

	CloseDoorSound = false;
	if (!AudioComponent) { UE_LOG(LogTemp, Warning, TEXT(" SOUND!")); return; }
	if (!OpenDorSound) {
		UE_LOG(LogTemp, Warning, TEXT(" SOUND!"));
		AudioComponent->Play();
		OpenDorSound = true;
	}


}

void UDoorOpen::CloseDoor(float DeltaTime) {
	CurrentYaw = FMath::FInterpTo(CurrentYaw,  InitYaw, DeltaTime, SpeeOfClosingDoor);
	FRotator DoorRotation = GetOwner()->GetActorRotation();
	DoorRotation.Yaw = CurrentYaw;
	GetOwner()->SetActorRotation(DoorRotation);

	OpenDorSound = false;
	if (!AudioComponent) { return; }
	if (!CloseDoorSound) {
		AudioComponent->Play();
		CloseDoorSound = true;
	}
}

float UDoorOpen::TotalMassOfActors() const {
	float TotalMass = 0.f;

	TArray<AActor*> OverlappingActors;
	if (!PressurePlate) { return TotalMass; }
	PressurePlate->GetOverlappingActors(OUT OverlappingActors); 

	for (AActor* Actor : OverlappingActors)
	{
		TotalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
		UE_LOG(LogTemp, Warning, TEXT("%s is on the pressureplate!"), *Actor->GetName());
	}

	return TotalMass;
}