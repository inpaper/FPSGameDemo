// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile_Boom.h"
#include "Components/ActorComponent.h"
#include "FireBoomComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPSGAMEDEMO_API UFireBoomComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFireBoomComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable,Category=Projectile)
	void Init(USkeletalMeshComponent* GunToSet,TSubclassOf<AProjectile_Boom> ProjectileBoomToSet,UParticleSystem* IndicatorBeamParticleToSet);

	// 控制手榴弹指示线开启关闭
	void IndicatorLineOpen();
	void IndicatorLineClose();
	
	void FireBoom();

	// 指示线总长
	UPROPERTY(EditAnywhere,Category=IndicatorLine)
	float PathLifeTime = 5.0f;

	// 指示线一段样条曲线长度
	UPROPERTY(EditAnywhere,Category=IndicatorLine)
	float TimeInterval = 0.05f;

	// 重力值
	UPROPERTY(EditAnywhere,Category=IndicatorLine)
	FVector Gravity = FVector(.0f,.0f,-980.0f);
	
private:
	
	// 绘制手榴弹指示线
	void DrawLine();
	// 绘制分段样条曲线 当返回值为true时 击中物体或者没有定义粒子特效
	bool DrawPartLine(FVector StartLocation,FVector InitVelocity,float Time1,float Time2,int Times);
	// 获取分段样条曲线的点
	FVector GetPartLinePoint(FVector StartLocation,FVector InitVelocity,float Time);
	// 清空之前的线段
	void ClearIndicatorLine();
	
	USkeletalMeshComponent* GunComponent;
	// 手榴弹
	TSubclassOf<AProjectile_Boom> ProjectileBoomClass;

	//是否绘制手榴弹指示线
	bool bDrawIndicatorLine = false;
	
	// 指示线样条曲线段数
	int IndicatorCount;
	
	// 组成一段样条曲线的粒子
	UParticleSystem* IndicatorBeamParticle;

	// 存放创建出来的粒子TArray
	TArray<UParticleSystemComponent*> CreateBeamParticleArray;
};
