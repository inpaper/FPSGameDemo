// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AICharacter.h"
#include "AIController.h"
#include "MyCharacter.h"
#include "PlayerCharacter.h"
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

	// AI逻辑：
	// 寻找自己视角内120度的玩家，如果找到就立刻转向并向玩家走去，当走到一个距离的时候开火
	// 如果被射击到，此时优先攻击击中AI的人物
	
	// ------------------------------------
	// 使用行为树蓝图代替代码开发！！
	void AIBehavior();

	// 寻找到不被遮挡且最近的玩家
	AMyPlayerController* FindNearlyPlayer();

	// 看向最近玩家
	UFUNCTION(BlueprintCallable)
	void LookToNearlyPlayer(AMyPlayerController* const NearlyPlayer);

	AFPSGameDemoGameModeBase* GameMode;
	// 控制的Pawn
	AAICharacter* ControlPawn;

	// AI的视角角度
	float AI_AngleOfView = 120.0f;
	
	// AI转身速度，默认每秒钟可以转120度
	UPROPERTY(EditDefaultsOnly)
	float MaxDegreePerSecond = 120.0f;

public:
	void GetPlayerPawnTransform();
	
	UPROPERTY(BlueprintReadOnly)
	FVector AimToDirection;

	UFUNCTION(NetMulticast,Reliable)
	void NotifyClientsLookForward(float LookForward);
	
	// UFUNCTION(BlueprintImplementableEvent)
	// void NotifyClientsLookForwardToUMG(float LookForward);

	// UPROPERTY(Replicated)
	// float GetLookForward;
};
