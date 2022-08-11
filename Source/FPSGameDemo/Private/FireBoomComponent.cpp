// Fill out your copyright notice in the Description page of Project Settings.


#include "FireBoomComponent.h"
#include "FPSGameDemo/FPSGameDemoGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values for this component's properties
UFireBoomComponent::UFireBoomComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UFireBoomComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
	IndicatorCount = UKismetMathLibrary::FFloor(PathLifeTime / TimeInterval);
}


// Called every frame
void UFireBoomComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

	if(bDrawIndicatorLine == false)return;
	if(GetOwner()->GetLocalRole())
	{
		DrawLine();
	}
	else
	{
		UE_LOG(LogTemp,Warning,TEXT("No"));
	}
}

void UFireBoomComponent::Init(USkeletalMeshComponent* GunComponentToSet, TSubclassOf<AProjectile_Boom> ProjectileBoomToSet,UParticleSystem* IndicatorBeamParticleToSet)
{
	GunComponent = GunComponentToSet;
	ProjectileBoomClass = ProjectileBoomToSet;
	IndicatorBeamParticle = IndicatorBeamParticleToSet;
}

void UFireBoomComponent::IndicatorLineOpen()
{
	bDrawIndicatorLine = true;
}

void UFireBoomComponent::IndicatorLineClose()
{
	ClearIndicatorLine();
	bDrawIndicatorLine = false;

	FireBoom(GunComponent,ProjectileBoomClass);
}

// 投掷手榴弹
void UFireBoomComponent::FireBoom_Implementation(USkeletalMeshComponent* GunComponentToSet, TSubclassOf<AProjectile_Boom> ProjectileBoomToSet)
{
	AFPSGameDemoGameModeBase* GameMode = Cast<AFPSGameDemoGameModeBase>(UGameplayStatics::GetGameMode(this));
	if(GameMode == nullptr)
	{
		UE_LOG(LogTemp,Warning,TEXT("GameMode Error"));
		return;
	}
	if(!GameMode->bFireAbility)
	{
		UE_LOG(LogTemp,Warning,TEXT("Controller Can not Fire"));
		return;
	}
	
	APawn* Owner = Cast<APawn>(GetOwner());
	if(Owner == nullptr)return;
	
	FRotator InitRot = Owner->Controller->GetControlRotation();
	AProjectile_Boom* CreateProjectileBoom = GetWorld()->SpawnActor<AProjectile_Boom>(
		ProjectileBoomToSet,
		GunComponentToSet->GetSocketLocation("FirePoint"),
		InitRot
	);

	FVector InitLocalVelocity = FVector(1000.0f,.0f,1000.0f);

	// 防止空指针
	if(CreateProjectileBoom == nullptr)return;
	if(CreateProjectileBoom->ProjectileMovementComponent == nullptr)return;
	
	CreateProjectileBoom->ProjectileMovementComponent->SetVelocityInLocalSpace(InitLocalVelocity);
}

// 绘制投掷物投掷路线样条曲线
void UFireBoomComponent::DrawLine()
{
	APawn* Owner = Cast<APawn>(GetOwner());
	if(Owner == nullptr)
	{
		UE_LOG(LogTemp,Warning,TEXT("Owner miss necessary"));
		return;
	}

	FVector InitLocation = GunComponent->GetSocketLocation("FirePoint");
	FRotator InitRot = Owner->Controller->GetControlRotation();
	FVector InitLocalVelocity = FVector(1000.0f,.0f,1000.0f);

	// 计算手榴弹的变换组件
	FTransform InitTransform = UKismetMathLibrary::MakeTransform(InitLocation,InitRot,FVector::OneVector);
	FVector InitWorldVelocity = UKismetMathLibrary::TransformDirection(InitTransform,InitLocalVelocity);

	for(int i = 0;i < IndicatorCount;++i)
	{
		float Time1 = TimeInterval * i;
		float Time2 = Time1 + TimeInterval;
		bool bHit = false;
		bHit = DrawPartLine(InitLocation,InitWorldVelocity,Time1,Time2,i);
		if(bHit)
		{
			// 去掉TArray内没有使用到的样条曲线，防止从远往近拉的时候远的样条曲线部分未删除
			while(CreateBeamParticleArray.IsValidIndex(i))
			{
				CreateBeamParticleArray[i]->DestroyComponent();
				CreateBeamParticleArray.RemoveAt(i);
			}
			break;
		}
	}
}

bool UFireBoomComponent::DrawPartLine(FVector StartLocation,FVector InitVelocity,float Time1,float Time2,int Times)
{
	if(IndicatorBeamParticle == nullptr)
	{
		UE_LOG(LogTemp,Warning,TEXT("%s FireBoomComponent miss necessary"),*GetOwner()->GetName());
		return true;
	}
	
	FVector Point1,Point2;
	Point1 = GetPartLinePoint(StartLocation,InitVelocity,Time1);
	Point2 = GetPartLinePoint(StartLocation,InitVelocity,Time2);


	// 判断是否已经击中物体，击中就不再绘制样条曲线
	FHitResult OutHit;
	bool bHit;
	bHit = UKismetSystemLibrary::LineTraceSingle(
		this,
		Point1,
		Point2,
		ETraceTypeQuery::TraceTypeQuery1,// TODO 这个设置的意义
		true,
		TArray<AActor*>(),
		EDrawDebugTrace::None,
		OutHit,
		true
		);

	if(bHit)return bHit;

	UParticleSystemComponent* CreateBeamParticle;
	// 对于上次创建好的样条曲线直接拿来改变光源位置使用即可，如果TArray内没有足够的样条曲线就创建一个新的
	if(Times >= CreateBeamParticleArray.Num())
	{
		CreateBeamParticle= UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),// TODO 这里的GetWorld和This的区别会在哪里
			IndicatorBeamParticle,
			GetOwner()->GetActorLocation(),
			GetOwner()->GetActorRotation(),
			false
		);

		if(CreateBeamParticle == nullptr)
		{
			UE_LOG(LogTemp,Warning,TEXT("%s FireBoomComponent Error"),*GetOwner()->GetName());
			return true;
		}

		// 给做好的样条曲线增加粒子特效并添加到TArray中
		CreateBeamParticle->SetTemplate(IndicatorBeamParticle);
		CreateBeamParticleArray.Add(CreateBeamParticle);
	}
	else
	{
		CreateBeamParticle = CreateBeamParticleArray[Times];

		if(CreateBeamParticle == nullptr)
		{
			UE_LOG(LogTemp,Warning,TEXT("%s FireBoomComponent Error"),*GetOwner()->GetName());
			return true;
		}
	}

	// 设置样条曲线的粒子特效Beam的开始与结尾
	CreateBeamParticle->SetBeamSourcePoint(0,Point1,0);
	CreateBeamParticle->SetBeamTargetPoint(0,Point2,0);
	
	return bHit;
}

// 计算手榴弹的当前位置，依据初始位置+初始速度*时间+gt^2/2的公式计算
FVector UFireBoomComponent::GetPartLinePoint(FVector StartLocation, FVector InitVelocity, float Time)
{
	FVector PointLocation = StartLocation + InitVelocity * Time + 0.5f * Gravity * Time * Time;
	return PointLocation;
}

void UFireBoomComponent::ClearIndicatorLine()
{
	for (auto BeamParticle : CreateBeamParticleArray)
	{
		BeamParticle->DestroyComponent();
	}
	
	CreateBeamParticleArray.Empty();
}







