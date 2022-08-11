// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyPlayerState.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"


USTRUCT()
struct FPlayerInfo
{
	GENERATED_BODY()

	int32 Name;
	int32 Score;
};

/**
 * 
 */
UCLASS()
class FPSGAMEDEMO_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	// 执行拥有该控制器的客户端需要执行的方法，如UI显示等
	UFUNCTION(Client,Reliable)
	void OwnerClientPostLogin();

	UUserWidget* HUD;

	TSubclassOf<UUserWidget> HUDWidget;

	UFUNCTION(BlueprintImplementableEvent)
	void CreateHUD();

	UFUNCTION(Server,Reliable)
	void RespawnPlayerPawn();

	UFUNCTION(Client,Reliable)
	void RefreshPlayerInfo(const TArray<FPlayerInfo>& AllPlayerState);

	UFUNCTION(BlueprintImplementableEvent)
	void ClearHUDPlayerInfo();
	
	UFUNCTION(BlueprintImplementableEvent)
	void FillHUDPlayerInfo(int32 Number,int32 Score);

	UFUNCTION(Client,Reliable)
	void NotifyPlayerSum(int PlayerSum);
	
	UFUNCTION(BlueprintImplementableEvent)
	void RefreshPlayerSum(int PlayerSum);

	UFUNCTION(BlueprintCallable,Server,Reliable)
	void AskToStartGame();

	// 服务器中将玩家传送到靶场
	UFUNCTION(Server,Reliable)
	void Pass();

	// 获取到服务器发来的开始游戏信息，在WaitTime后Server进行传送，此处使用Client进行封装，避开蓝图实现不能同时使用RPC的问题
	UFUNCTION(Client,Reliable)
	void GetMessageToPass(int32 WaitTime);

	UFUNCTION(BlueprintImplementableEvent)
	void SendReadyPassMessageToUMG(int32 WaitTime);
	
	UFUNCTION(Client,Reliable)
	void GetMessageToReadyGame(int32 WaitTime);
	
	UFUNCTION(BlueprintImplementableEvent)
	void SendReadyGameMessageToUMG(int32 WaitTime);

private:
	FTimerHandle PassTimeHandle;
};
