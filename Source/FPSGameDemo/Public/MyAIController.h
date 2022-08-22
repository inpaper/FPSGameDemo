// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AICharacter.h"
#include "AIController.h"
#include "FPSGameDemo/FPSGameDemoGameModeBase.h"
#include "MyAIController.generated.h"

/**
 * 只在Server中存在，包括Tick也是在Server中运行
 */
UCLASS()
class FPSGAMEDEMO_API AMyAIController : public AAIController
{
	GENERATED_BODY()

	// TODO Tick没有执行，不清楚原因，使用GameMode的Tick来代替
	virtual void Tick(float DeltaSeconds) override;

	// 只要在AIController中使用了该方法，就会导致Pawn不能正确附加AIController，
	// 原因：在蓝图实现中写了一边Onpossess逻辑，可能冲突了
	// virtual void OnPossess(APawn* InPawn) override;

	// 初始化游戏模式和控制Pawn（默认不变Pawn）
	void Init();
	
	// AI逻辑：
	// 寻找自己视角内120度的玩家，如果找到就立刻转向并向玩家走去，当走到一个距离的时候开火
	// 如果被射击到，此时优先攻击击中AI的人物
	
	// ------------------------------------
	// 使用行为树蓝图代替代码开发！！
	void AIBehavior();

	// 寻找到不被遮挡且最近的玩家
	UFUNCTION(BlueprintCallable)
	AActor* FindNearlyPlayer();

	// 看向最近玩家
	UFUNCTION(BlueprintCallable)
	void LookToNearlyPlayer(AActor* const NearlyPlayer);

	// 如果返回True，说明目标在视野精度内部
	UFUNCTION(BlueprintCallable)
	bool CheckNearlyPlayerRot(AActor* const NearlyPlayer,float LookPrecision);

	AFPSGameDemoGameModeBase* GameMode;
	// 控制的Pawn
	AAICharacter* ControlPawn;

	// AI的视角角度
	float AI_AngleOfView = 120.0f;
	
	// AI转身速度，默认每秒钟可以转120度
	UPROPERTY(EditDefaultsOnly)
	float MaxDegreePerSecond = 120.0f;

public:
	// 该方法已被弃用
	void GetPlayerPawnTransform();
	
	UPROPERTY(BlueprintReadOnly)
	FVector AimToDirection;

	UFUNCTION(NetMulticast,Reliable)
	void NotifyClientsLookForward(float LookForward);
	
	// UFUNCTION(BlueprintImplementableEvent)
	// void NotifyClientsLookForwardToUMG(float LookForward);

	// UPROPERTY(Replicated)
	// float GetLookForward;

	UFUNCTION(BlueprintImplementableEvent)
	void NotifyToBehaviorTreeIsAlive(bool isAlive);
};
