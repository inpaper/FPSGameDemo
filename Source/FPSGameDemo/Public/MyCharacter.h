// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FireBoomComponent.h"
#include "FireComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SpotLightComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "MyCharacter.generated.h"

UCLASS()
class FPSGAMEDEMO_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere,Category="Gun")
	USpotLightComponent* GunSpotLightComponent;
	
	UPROPERTY(EditAnywhere,Category="Camera")
	USpringArmComponent* CameraSpringArmComponent;
	
	UFUNCTION(BlueprintCallable,Category="FireBullet")
	void FireBullet();

	// 服务器上使用DeprojectScreenPositionToWorld会因为获取不到ULocalPlayer而输出位置为0
	// 因此DeprojectScreenPositionToWorld在客户端进行计算
	// TODO 但因为发射位置和发射旋转都在客户端计算，会有作弊的可能性吧？
	void CalculateFireRotator(const FVector FireLocation,FRotator& FireRotator);

	UFUNCTION(BlueprintCallable,Category="FireBoom")
	void FireBoomIndicatorOpen();
	UFUNCTION(BlueprintCallable,Category="FireBoom")
	void FireBoomIndicatorClose();
	
public:
	// Sets default values for this character's properties
	AMyCharacter();

	// 玩家伤害计算
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
	// 内部实现的摄像机设置不了初始旋转度数 原因：spring组件使用了bUsePawnControlRotation后控制权不在spring组件上
	// TODO 开始时如何调整摄像机角度
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category= "Camera")
	UCameraComponent* CameraComponent;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Gun")
	USkeletalMeshComponent* GunComponent;

	// 子弹生成物
	UPROPERTY(EditDefaultsOnly,Category=Fire)
	TSubclassOf<class AProjectile> ProjectileClass;

	// 炸弹生成物
	UPROPERTY(EditDefaultsOnly,Category=FireBoom)
	TSubclassOf<class AProjectile_Boom> ProjectileBoomClass;

	// 指示线粒子系统
	UPROPERTY(EditDefaultsOnly,Category=FireBoom)
	UParticleSystem* IndicatorBeamParticle;

	UPROPERTY(BlueprintReadOnly,EditAnywhere,Category=Fire)
	UFireComponent* FireComponent;

	UPROPERTY(BlueprintReadOnly,EditAnywhere,Category=FireBoom)
	UFireBoomComponent* FireBoomComponent;
	
	UPROPERTY(EditAnywhere,Category=Camera)
	float StartCameraTurnPitch = 15.0f;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Move();
	
	float XValue,YValue;
	void MoveForward(float Value);
	void MoveRight(float Value);

	// 更改当前是否可以八方向移动的情况。
	void StartRunEight();
	void StopRunEight();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
private:
	// 控制八方向移动，限制只有X和Y不能同时输入
	bool bRunEight = false;
	
	// 判断是否当前正在按下X或者Y
	bool bRunX = false;
	bool bRunY = false;

	UPROPERTY(EditAnywhere,Category="HP")
	int32 MaxHP = 100;

	UPROPERTY(VisibleAnywhere,Category="HP")
	int32 CurrentHP = MaxHP;

	// 根据瞄准点确定
	UPROPERTY(EditDefaultsOnly,Category=Fire)
	float AimPercentX = 0.5f;

	UPROPERTY(EditDefaultsOnly,Category=Fire)
	float AimPercentY = 0.3f;
	
	// 经试验，以300m外的集中距离发射角度发射子弹，感觉最合理
	UPROPERTY(EditDefaultsOnly,Category=Fire)
	int FireLength = 3000;
};
