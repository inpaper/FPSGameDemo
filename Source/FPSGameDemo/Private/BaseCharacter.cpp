// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include "AICharacter.h"
#include "MyAIController.h"
#include "MyGameInstance.h"
#include "PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "FPSGameDemo/FPSGameDemoGameModeBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	GunComponent = CreateDefaultSubobject<USkeletalMeshComponent>("Gun");
	GunSpotLightComponent = CreateDefaultSubobject<USpotLightComponent>("GunSpotLightComponent");
	
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	FireComponent = CreateDefaultSubobject<UFireComponent>("FireComponent");
	CameraSpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("CameraSpringArmComponent");
	
	// spring使用了pawn控制，因此设置不了初始旋转度数
	// const FRotator SpringRot = FRotator::FRotator(-20.0f,.0f,.0f);
	// CameraSpringArmComponent->SetRelativeRotation(SpringRot);
	
	CameraSpringArmComponent->AttachTo(RootComponent);
	CameraSpringArmComponent->TargetArmLength = 600.0f;
	
	CameraComponent->AttachTo(CameraSpringArmComponent);
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	if(CameraSpringArmComponent != nullptr)
	{
		AddControllerPitchInput(StartCameraTurnPitch);
	}
	// 使枪的模型贴合人物手部骨骼点，并使SpotLight贴合到枪的射击口
	if(GunComponent != nullptr)
	{
		GunComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GunPoint"));
		if(GunSpotLightComponent)
		{
			GunSpotLightComponent->AttachToComponent(GunComponent, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("LightPoint"));
		}
	}
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABaseCharacter::Fire()
{
	// UE_LOG(LogTemp,Warning,TEXT("%s Fire"),*GetName());
	NotifyServerIsFire();
}

void ABaseCharacter::NotifyServerIsFire_Implementation()
{
	if(!HasAuthority())return;

	if(bFireAbility && FireComponent->GetFireState() == EFireState::Ready)
	{
		NotifyClientsIsFire();
	}
}

void ABaseCharacter::NotifyClientsIsFire_Implementation()
{
	NotifyAnimToFire();
}

float ABaseCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	UE_LOG(LogTemp,Warning,TEXT("受到伤害 %s %f"),*GetName(),DamageAmount);
	
	int32 GetDamage = FPlatformMath::RoundToInt(DamageAmount);
	GetDamage = FMath::Clamp(GetDamage,0,CurrentHP);
	
	CurrentHP -= GetDamage;
	GetMessageToTakeDamage(CurrentHP);

	// 输出伤害者为玩家时需要显示玩家UMG对应血量
	APlayerCharacter* DamageCauserPawn = Cast<APlayerCharacter>(DamageCauser);
	if(DamageCauserPawn != nullptr)
	{
		DamageCauserPawn->GetMessageToShowHPUMG(this);
	}
	
	if(CurrentHP <= 0)
	{
		UE_LOG(LogTemp,Warning,TEXT("%s 玩家血量归零"),*GetName());
	}

	UpdatePlayerHPInfo();
	
	return DamageAmount;
}

void ABaseCharacter::GetMessageToTakeDamage_Implementation(int32 CurrentHPMessage)
{
	CurrentHP = CurrentHPMessage;
	SendCurrentHPToUMG(CurrentHP);
}

void ABaseCharacter::GetMessageToShowHPUMG_Implementation(ABaseCharacter* InjuredPawn)
{
	if(InjuredPawn == nullptr)
		return;

	InjuredPawn->SendForceShowHPToUMG();
}

void ABaseCharacter::UpdatePlayerHPInfo_Implementation()
{
	if(!HasAuthority())return;

	AGameModeBase* GameModeBase = UGameplayStatics::GetGameMode(this);
	AFPSGameDemoGameModeBase* GameMode = Cast<AFPSGameDemoGameModeBase>(GameModeBase);

	TArray<FString> PlayerNameArray;
	TArray<int32> PlayerHPArray;

	// 用于AI场游戏结束计算排名
	bool bNotEnd = false;
	int32 CurDeadNumber = 0;
	ABaseCharacter* DeadPlayer = nullptr;
	
	for (auto PlayerController : GameMode->AllPlayerController)
	{
		AMyPlayerState* MyPlayerState = Cast<AMyPlayerState>(PlayerController->PlayerState);
		PlayerNameArray.Add(MyPlayerState->PlayerName);

		ABaseCharacter* MyPawn = Cast<ABaseCharacter>(PlayerController->GetPawn());
		PlayerHPArray.Add(MyPawn->CurrentHP);

		if(MyPawn->CurrentHP > 0)
		{
			bNotEnd = true;
			MyPawn->bDead = false;
		}
		else
		{
			// 第一次死亡
			if(!MyPawn->bDead)
			{
				bDead = true;
				DeadPlayer = MyPawn;
				MyPawn->NotifyHPZero();
			}
			else
			{
				CurDeadNumber++;
			}
		}
	}
	
	if(DeadPlayer != nullptr)
	{
		DeadPlayer->DeadNumber = CurDeadNumber;
	}

	// bNotEnd为true，此时全体玩家血量归零，游戏结束
	// 获取游戏状态，如果不在AI场内部，血量归零不需要显示排名榜
	UMyGameInstance* ServerGameInstance = Cast<UMyGameInstance>(GameMode->GetGameInstance());
	if(!ServerGameInstance->IsCurrentState(EPlayerGameMode::PlayingAI))
	{
		bNotEnd = true;
	}

	if(!bNotEnd)
	{
		// 如果不能改变状态，说明已经展示了排名榜了，不需要再更新了
		if(!ServerGameInstance->TransitionToState(EPlayerGameMode::PlayingAIEnd))
		{
			bNotEnd = true;
		}
	}
	
	TArray<FString> PlayerNameNumberArray;
	if(!bNotEnd)
	{
		for (int i = 0; i < GameMode->AllPlayerController.Num();++i)
		{
			for (auto PlayerController : GameMode->AllPlayerController)
			{
				ABaseCharacter* MyPawn = Cast<ABaseCharacter>(PlayerController->GetPawn());
				if(MyPawn->DeadNumber == i)
				{
					AMyPlayerState* MyPlayerState = Cast<AMyPlayerState>(PlayerController->PlayerState);
					PlayerNameNumberArray.Add(MyPlayerState->PlayerName);
					break;
				}
			}
		}
	}
	
	for (auto PlayerController : GameMode->AllPlayerController)
	{
		// 更新血量信息
		PlayerController->RefreshPlayerHP(PlayerNameArray,PlayerHPArray);

		// 血量全都为0，游戏结束，显示排名榜
		if(!bNotEnd)
		{
			ABaseCharacter* MyPawn = Cast<ABaseCharacter>(PlayerController->GetPawn());
			MyPawn->GetMessageToAIGameOver(PlayerNameNumberArray);
		}
	}
}

void ABaseCharacter::GetMessageToAIGameOver_Implementation(const TArray<FString>& PlayerName)
{
	SendAIGameOverMessageToUMG(PlayerName);
}

void ABaseCharacter::ResumePlayerHP_Implementation()
{
	CurrentHP = MaxHP;
	bFireAbility = true;
	bDead = false;
	DeadNumber = -1;
	GetMessageToTakeDamage(CurrentHP);
	UpdatePlayerHPInfo();
}

void ABaseCharacter::ResumeAIHP_Implementation()
{
	CurrentHP = MaxHP;
	bFireAbility = true;
	bDead = false;
	DeadNumber = -1;
	Cast<AAICharacter>(this)->ChangeAILive(true);
	GetMessageToTakeDamage(CurrentHP);
}


void ABaseCharacter::NotifyHPZero_Implementation()
{
	bFireAbility = false;
}