// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "Components/ActorComponent.h"
#include "FireComponent.generated.h"

// 控制射击频率的状态
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
	
	UFUNCTION(Server,Reliable)
	void Fire(TSubclassOf<class AProjectile> GetProject,FVector FireLocation,FRotator FireDirection);

	EFireState GetFireState();
	
	UPROPERTY(EditDefaultsOnly,Category=Projectile)
	float LineLength = 10000.0f;

	UPROPERTY(EditAnywhere,Category=Aim)
	float ReloadTime = 0.3f;
private:
	// 攻击状态
	EFireState FireState;
	
	// 记录时间
	double RecordTime = 0.0f;
	
	// Deprecated 弃用该方法，转而使用屏幕位置生成发射点位，获取子弹发射方向
	void GetFireDirection(FVector FireLocation,UCameraComponent* CameraComponent,FRotator &FireDirection);
};
