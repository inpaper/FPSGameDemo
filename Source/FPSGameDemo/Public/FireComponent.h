// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
// #include "MyCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/ActorComponent.h"
#include "FireComponent.generated.h"

UENUM()
enum class EFireState:uint8
{
	Ready,
	Reload,
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPSGAMEDEMO_API UFireComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFireComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable,Category="Setup")
	void Init(TSubclassOf<class AProjectile> ProjectileToSet);
	
	UFUNCTION(Server,Reliable)
	void Fire(TSubclassOf<class AProjectile> GetProject, FVector FireLocation,UCameraComponent* CameraComponent);

	// 获取子弹发射方向
	void GetFireDirection(FVector FireLocation,UCameraComponent* CameraComponent,FRotator &FireDirection);
	
	UPROPERTY(EditDefaultsOnly,Category=Projectile)
	float LaunchSpeed = 10000.0f;

	UPROPERTY(EditDefaultsOnly,Category=Projectile)
	float LineLength = 10000.0f;

	UPROPERTY(EditAnywhere,Category=Aim)
	float ReloadTime = 0.3f;
private:
	USkeletalMeshComponent* GunComponent;

	// UCameraComponent* CameraComponent;

	TSubclassOf<class AProjectile> ProjectileClass;

	// 攻击状态
	EFireState FireState;
	
	// 记录时间
	double RecordTime = 0.0f;
};
