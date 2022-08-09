// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MyPlayerController.h"
#include "MyPlayerState.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameState.h"
#include "FPSGameDemoGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class FPSGAMEDEMO_API AFPSGameDemoGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	virtual void PostLogin(APlayerController* NewPlayer) override;

public:
	UFUNCTION(Server,Reliable)
	void RespawnPlayerPawn(APlayerController* PlayerController);
	
	void RefreshPlayerInfoAndNotify();
	
	UPROPERTY(BlueprintReadOnly)
	TArray<AMyPlayerController*> AllPlayerController;

	TArray<AActor*> GetGameStarts;

	TArray<AMyPlayerState*> AllGameStates;
};
