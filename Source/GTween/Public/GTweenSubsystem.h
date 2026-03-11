// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "Kismet/KismetMathLibrary.h"
#include "GTweenSubsystem.generated.h"

UENUM(BlueprintType)
enum class ETweenSpace : uint8 { World, Local };

/** 循环播放模式 */
UENUM(BlueprintType)
enum class ETweenLoopMode : uint8
{
	/** 每次循环从起始点瞬间传送并重新正向播放 */
	Restart,
	/** 来回往返：正向播完后反向播放，交替进行 */
	PingPong,
	/** 叠加模式：每次循环在上次结束位置继续追加相同偏移路径 */
	Incremental,
};

// 声明回调类型
typedef TFunction<void()> FTweenCompleteCallback;

/** 单个运动任务的数据结构 */
struct GTWEEN_API FActiveTween
{
	TWeakObjectPtr<AActor> TargetActor;
	TArray<FTransform> Path;
	float TotalDuration;
	EEasingFunc::Type EaseType;
	ETweenSpace Space;
	FTweenCompleteCallback OnComplete;

	/** 循环次数：0=单次播放（默认），-1=无限循环，>0=固定循环次数 */
	int32 LoopCount = 0;
	/** 循环模式（LoopCount != 0 时生效） */
	ETweenLoopMode LoopMode = ETweenLoopMode::Restart;

	// --- 运行时状态 ---
	float ElapsedTime = 0.f;
	int32 CurrentIndex = 0;
	FTransform StartTransform;
	/** 已完成的循环次数 */
	int32 CompletedLoops = 0;
	/** PingPong 模式：当前是否正向播放 */
	bool bForwardPlay = true;
	/** 原始路径（供 Restart/PingPong 重置用） */
	TArray<FTransform> OriginalPath;
	/** 注册时的原始起始 Transform（供 Restart 传送回起点用） */
	FTransform OriginalStartTransform;
};




/**
 * 
 */
UCLASS()
class GTWEEN_API UGTweenSubsystem : public UEngineSubsystem, public FTickableGameObject
{
	GENERATED_BODY()
	
	
public:
	// 链式构造器
	struct GTWEEN_API FTweenBuilder
	{
		UGTweenSubsystem* Owner;
		FActiveTween Data;

		FTweenBuilder& SetDuration(float InDuration) { Data.TotalDuration = InDuration; return *this; }
		FTweenBuilder& SetEase(EEasingFunc::Type InEase) { Data.EaseType = InEase; return *this; }
		FTweenBuilder& OnComplete(FTweenCompleteCallback InCallback) { Data.OnComplete = InCallback; return *this; }
		
		FTweenBuilder& SetSpace(ETweenSpace InSpace) { Data.Space = InSpace; return *this; }
		/**
		 * 设置循环播放
		 * @param InLoopCount  0=单次（默认），-1=无限循环，>0=固定循环次数
		 * @param InMode       循环模式：Restart / PingPong / Incremental
		 */
		FTweenBuilder& SetLoop(int32 InLoopCount, ETweenLoopMode InMode = ETweenLoopMode::Restart)
		{
			Data.LoopCount = InLoopCount;
			Data.LoopMode = InMode;
			return *this;
		}
		void Play() { Owner->RegisterTween(Data); }
	};

	/** 开始创建一个 Tween 任务 */
	FTweenBuilder CreateTween(AActor* InActor, const TArray<FTransform>& InPath);

	// FTickableGameObject 接口
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override { return ActiveTweens.Num() > 0; }
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UGTweenSubsystem, STATGROUP_Tickables); }

private:
	void RegisterTween(const FActiveTween& NewTween);
	TArray<FActiveTween> ActiveTweens;
	
	
};
