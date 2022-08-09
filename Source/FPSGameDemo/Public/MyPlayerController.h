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
};
