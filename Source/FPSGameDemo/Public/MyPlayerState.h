// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MyPlayerState.generated.h"


/**
 * 
 */
UCLASS()
class FPSGAMEDEMO_API AMyPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	int32 PlayerNumber = 0;

	UPROPERTY(BlueprintReadOnly)
	FString PlayerName = "DefaultPlayer";

	UPROPERTY(BlueprintReadOnly,Category=Score)
	int32 PlayerScore = 0;
	
	int32 PlayerCauseDamage = 0;
	
	// 玩家首次登录时用于初始化
	// TODO 后续考虑使用名称替代Number
	UFUNCTION()
	void InitPlayerNumber(int32 const PlayerNumberToSet);

	//--------------------------------------
	// 对于更改分数的方法，只有拥有权限才能进行修改
	UFUNCTION()
	void AddScore(int32 const GetScore);

	UFUNCTION()
	void DecreaseScore(int32 const GetScore);

	UFUNCTION()
	void SetPlayerScore(int32 const GetScore);
	
	UFUNCTION()
	void ResetScore();

	// 通知服务器更新数据
	UFUNCTION(Server,Reliable,BlueprintCallable)
	void InfoUpdate();

	UFUNCTION(Server,Unreliable,BlueprintCallable)
	void ChangeName(const FString& GetName);
};
