// Fill out your copyright notice in the Description page of Project Settings.


#include "FireComponent.h"

#include "Projectile.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UFireComponent::UFireComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UFireComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	APawn* Owner = Cast<APawn>(GetOwner());
	if(Owner == nullptr)return;
	Owner->InputComponent->BindAction("Fire",IE_Pressed,this,&UFireComponent::Fire);
}


// Called every frame
void UFireComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

	if(GunComponent == nullptr || CameraComponent == nullptr)return;

	// 设置开枪的状态
	if(FPlatformTime::Seconds() - RecordTime < ReloadTime)
	{
		FireState = EFireState::Reload;
	}
	else
	{
		FireState = EFireState::Ready;
	}
}

void UFireComponent::Init(USkeletalMeshComponent* GunToSet,UCameraComponent* CameraToSet,TSubclassOf<AProjectile> ProjectileToSet)
{
	GunComponent = GunToSet;
	ProjectileClass = ProjectileToSet;
	CameraComponent = CameraToSet;
}

// 按下开火键执行该操作
void UFireComponent::Fire()
{
	if(GunComponent == nullptr || CameraComponent == nullptr)
	{
		UE_LOG(LogTemp,Warning,TEXT("%s FireComponent miss necessary"),*GetOwner()->GetName());
		return;
	}

	if(FireState == EFireState::Reload)return;
	
	FRotator OffsetRot;
	GetFireDirection(OffsetRot);
	
	AProjectile* CreateProjectile = GetWorld()->SpawnActor<AProjectile>(
		ProjectileClass,
		GunComponent->GetSocketLocation(FName("FirePoint")),
		OffsetRot
		);

	RecordTime = FPlatformTime::Seconds();
}

// 通过计算摄像头的位置和摄像头正方向确定开始与结束位置，生成射线，将射线击中的点作为射击的点，从子弹初始位置看向击中位置获取发射方向。
void UFireComponent::GetFireDirection(FRotator &FireDirection)
{
	FVector StartLocation = CameraComponent->GetComponentLocation();
	FVector EndLocation = StartLocation + CameraComponent->GetForwardVector() * LineLength;
	
	FVector HitLocation = EndLocation;
	
	FHitResult HitResult;
	if(GetWorld()->LineTraceSingleByChannel(HitResult,StartLocation,EndLocation,ECollisionChannel::ECC_Visibility))
	{
		HitLocation = HitResult.Location;
	}

	// 发射位置默认为枪位置，计算中使用摄像机位置，所以需要在结果处补充这个偏差。
	FVector GunCameraOffset = StartLocation - GunComponent->GetSocketLocation(FName("FirePoint"));

	FireDirection = UKismetMathLibrary::FindLookAtRotation(StartLocation,HitLocation + GunCameraOffset);
}

