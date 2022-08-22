// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "GameFramework/Character.h"
#include "AICharacter.generated.h"

// 管理AI的性格
UENUM()
enum class EAITemp:uint8
{
	// 愤怒性格，在血量变低的时候会更加倾向于靠近玩家增加少量移速并且加快攻速
	Angry,
	// 逃跑性格，在血量变低的时候会更加倾向于停止攻击增加大量移速并逃跑
	Escape,

	Unknown
};

UCLASS()
class FPSGAMEDEMO_API AAICharacter : public ABaseCharacter
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable,Category="Fire")
	virtual void Fire() override;
public:
	// Sets default values for this character's properties
	AAICharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 蓝图中使用行为树进行管理
	UFUNCTION(BlueprintCallable)
	void AIFireBluePrint(AActor* FireToActor);

	// 只在服务器运行
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	bool bAIAlive = true;

	// 通知行为树AI生命情况
	UFUNCTION(Server,Unreliable)
	void ChangeAILive(bool isAlive);

	UFUNCTION(BlueprintImplementableEvent)
	void NotifyToBehaviorTreeIsAlive(bool isAlive);
	
	UFUNCTION(BlueprintImplementableEvent)
	void AIEscape();

	UFUNCTION(BlueprintImplementableEvent)
	void AIAngry();

	// 在巡逻的时候被攻击，转向攻击者并发起攻击
	UFUNCTION(BlueprintImplementableEvent)
	void PatrolStatusBeAttacked(AActor* DamageCauser);
private:
	
	EAITemp AiTemp = EAITemp::Unknown;
};
