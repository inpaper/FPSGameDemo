// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerState.h"

#include "FPSGameDemo/FPSGameDemoGameModeBase.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

void AMyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(AMyPlayerState,PlayerNumber,COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(AMyPlayerState,PlayerScore,COND_SimulatedOnly);
}

void AMyPlayerState::InitPlayerNumber(int PlayerNumberToSet)
{
	if(HasAuthority())
	{
		PlayerNumber = PlayerNumberToSet;
	}
}


void AMyPlayerState::AddScore(int const GetScore)
{
	if(HasAuthority())
	{
		PlayerScore += GetScore;
		ScoreChange();
	}
}

void AMyPlayerState::DecreaseScore(int const GetScore)
{
	if(HasAuthority())
	{
		PlayerScore -= GetScore;
		ScoreChange();
	}
}

void AMyPlayerState::ResetScore()
{
	if(HasAuthority())
	{
		PlayerScore = 0;
		ScoreChange();
	}
}

void AMyPlayerState::ScoreChange_Implementation()
{
	// UE_LOG(LogTemp,Warning,TEXT("ScoreChange_Implementation"));
	// AGameModeBase* GameMode = UGameplayStatics::GetGameMode(this);
	// AFPSGameDemoGameModeBase* FPSGameMode = Cast<AFPSGameDemoGameModeBase>(GameMode);
	// FPSGameMode->RefreshPlayerInfoAndNotify();
}


