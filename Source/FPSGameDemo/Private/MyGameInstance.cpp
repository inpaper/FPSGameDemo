// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"


bool UMyGameInstance::TransitionToState(EGameState GetState)
{
	if(IsCurrentState(GetState))
	{
		UE_LOG(LogTemp,Warning,TEXT("The Current Game State is the same as the changed state %d"),GetState);
		return false;
	}
	
	CurrentState = GetState;
	// TODO 这里可以针对不同的状态更改情况执行对应的方法。
	
	return true;
}

bool UMyGameInstance::IsCurrentState(EGameState GetState)
{
	return CurrentState == GetState;
}
