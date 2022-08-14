// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MyPlayerController.h"
#include "MyPlayerState.h"
#include "GameFramework/GameModeBase.h"
#include "FPSGameDemoGameModeBase.generated.h"

/**
 * GameMode只存在于服务器，因此代码都是在服务器运行
 */
UCLASS()
class FPSGAMEDEMO_API AFPSGameDemoGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	virtual void PostLogin(APlayerController* NewPlayer) override;

public:
	// 目前没有使用该方法
	// TODO 不使用Server看一下是否可以正常运行
	UFUNCTION(Server,Reliable)
	void RespawnPlayerPawn(APlayerController* PlayerController);
	
	void ClearPlayerScore();

	// 控制玩家在靶场等待时间的开火，倒计时结束前不允许开火
	void ChangeFireAbility(bool GetFireAbility);

	void RespawnAIPawn();
	
	// 存储所有游戏控制器
	UPROPERTY(BlueprintReadOnly)
	TArray<AMyPlayerController*> AllPlayerController;

	bool bCreateRoom = true;
	
	// 保存游戏开始点
	TArray<AActor*> GetGameStarts;

	// 保存传送到靶场的用户点
	TArray<AActor*> Target_PlayerPoints;
	TArray<AActor*> AI_PlayerPoints;
	TArray<AActor*> AI_AIPoints;
	
	// 传送到靶场的等待时间
	UPROPERTY(EditAnywhere)
	int32 PassWaitTime = 5;

	// 到靶场后游戏正式开始的等待时间
	UPROPERTY(EditAnywhere)
	int32 StartGameWaitTime = 3;

	// 游戏时间
	UPROPERTY(EditAnywhere)
	int32 TotalGameTime = 60;

	// 玩家是否具备开火功能，在准备阶段禁止使用
	bool bFireAbility = true;
};
