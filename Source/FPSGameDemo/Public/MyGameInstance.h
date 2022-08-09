// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

UENUM()
enum class EGameState:uint8
{
	Start,
	MainMenu,
	Playing,
	Unknown,
};


/**
 * 
 */
UCLASS()
class FPSGAMEDEMO_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly,Category=GameState)
	EGameState CurrentState = EGameState::Start;

	UFUNCTION(BlueprintCallable,Category=GameState)
	bool TransitionToState(EGameState GetState);

	UFUNCTION(BlueprintCallable,Category=GameState)
	bool IsCurrentState(EGameState GetState);
};
