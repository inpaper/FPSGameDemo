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
	UPROPERTY(BlueprintReadOnly,Replicated)
	int32 PlayerNumber = 0;

	UPROPERTY(BlueprintReadOnly,Replicated)
	int32 PlayerScore = 0;
	
	UFUNCTION()
	void InitPlayerNumber(int PlayerNumberToSet);

	//------------------
	// 对于更改分数的方法，只有拥有权限才能进行修改
	
	// UFUNCTION(NetMulticast,Reliable)
	// void SetPlayerScore();
	
	UFUNCTION()
	void AddScore(int GetScore);

	UFUNCTION()
	void DecreaseScore(int GetScore);

	UFUNCTION()
	void ResetScore();

	// 通知服务器更新数据
	UFUNCTION(Server,Reliable)
	void ScoreChange();
};
