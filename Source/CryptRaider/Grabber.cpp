// Fill out your copyright notice in the Description page of Project Settings.

#include "Grabber.h"

#include "Engine/World.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	PhysicsHandle = GetPhysicsHandle();
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (PhysicsHandle && IsGrabbing())
	{
		FVector TargetLocation = GetComponentLocation() + GetForwardVector() * HoldDistance;
		PhysicsHandle->SetTargetLocationAndRotation(TargetLocation, GetComponentRotation());
	}
}

bool UGrabber::IsGrabbing()
{
	UPrimitiveComponent *GrabbedComponent;
	return IsGrabbing(GrabbedComponent);
}

bool UGrabber::IsGrabbing(UPrimitiveComponent *&OutGrabbedComponent)
{
	OutGrabbedComponent = PhysicsHandle->GetGrabbedComponent();
	if (OutGrabbedComponent)
	{
		OutGrabbedComponent->WakeAllRigidBodies();
		return true;
	}

	return false;
}

void UGrabber::Grab()
{
	if (!PhysicsHandle)
		return;

	FHitResult HitResult;
	if (GetGrabblableInReach(HitResult))
	{
		UPrimitiveComponent *HitComponent = HitResult.GetComponent();
		HitComponent->SetSimulatePhysics(true);
		HitComponent->WakeAllRigidBodies();

		AActor* Actor = HitResult.GetActor();
		Actor->Tags.Add("Grabbed");
		Actor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		PhysicsHandle->GrabComponentAtLocationWithRotation(
			HitComponent,
			NAME_None,
			HitResult.ImpactPoint,
			GetComponentRotation());
	}
}

void UGrabber::Release()
{
	if (!PhysicsHandle)
		return;

	UPrimitiveComponent *GrabbedComponent;
	if (!IsGrabbing(GrabbedComponent))
		return;

	GrabbedComponent->GetOwner()->Tags.Remove("Grabbed");
	PhysicsHandle->ReleaseComponent();
}

UPhysicsHandleComponent *UGrabber::GetPhysicsHandle() const
{
	UPhysicsHandleComponent *Result = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (!Result)
	{
		UE_LOG(LogTemp, Error, TEXT("Grabber requires a UPhysicsHandleComponent."));
	}

	return Result;
}

bool UGrabber::GetGrabblableInReach(FHitResult &OutHitResult) const
{
	FVector Start = GetComponentLocation();
	FVector End = Start + GetForwardVector() * MaxGrabDistance;

	FCollisionShape Sphere = FCollisionShape::MakeSphere(GrabRadius);

	return GetWorld()->SweepSingleByChannel(
		OutHitResult,
		Start,
		End,
		FQuat::Identity,

		// O valor de collision channel precisa ser verificado no arquivo /config/DefaultEngine.ini
		ECC_GameTraceChannel2,
		Sphere);
}