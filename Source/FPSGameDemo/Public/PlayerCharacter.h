// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "FireBoomComponent.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class FPSGAMEDEMO_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable,Category="Fire")
	virtual void Fire() override;

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
	APlayerCharacter();
	
	// 炸弹生成物
	UPROPERTY(EditDefaultsOnly,Category=FireBoom)
	TSubclassOf<class AProjectile_Boom> ProjectileBoomClass;

	// 指示线粒子系统
	UPROPERTY(EditDefaultsOnly,Category=FireBoom)
	UParticleSystem* IndicatorBeamParticle;

	UPROPERTY(BlueprintReadOnly,EditAnywhere,Category=FireBoom)
	UFireBoomComponent* FireBoomComponent;
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

	UFUNCTION(Server,Reliable)
	void NotifyServerLookForward(float LookForward);

	UFUNCTION(NetMulticast,Reliable)
	void NotifyClientsLookForward(float LookForward);

	// 更新所有用户HP中的名称
	UFUNCTION(NetMulticast,Unreliable)
	void GetMessageToRefreshHPName(const FString & NewName);

	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
	void SendHPNameMessageToUMG(const FString & NewName);
	
	UPROPERTY(BlueprintReadOnly)
	float GetLookForward;

	// UFUNCTION(BlueprintCallable)
	// void AIFire();
	//
	// // 蓝图中使用行为树进行管理
	// UFUNCTION(BlueprintCallable)
	// void AIFireBluePrint(AActor* FireToActor);
private:
	// 控制八方向移动，限制只有X和Y不能同时输入
	bool bRunEight = false;
	
	// 判断是否当前正在按下X或者Y
	bool bRunX = false;
	bool bRunY = false;
	
	// 根据瞄准点确定
	UPROPERTY(EditDefaultsOnly,Category=Fire)
	float AimPercentX = 0.5f;

	UPROPERTY(EditDefaultsOnly,Category=Fire)
	float AimPercentY = 0.3f;
	
	// 经试验，以300m外的集中距离发射角度发射子弹，感觉最合理
	UPROPERTY(EditDefaultsOnly,Category=Fire)
	int FireLength = 3000;
};
