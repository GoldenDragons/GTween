// Fill out your copyright notice in the Description page of Project Settings.


#include "GTweenSubsystem.h"
#include "Algo/Reverse.h"

UGTweenSubsystem::FTweenBuilder UGTweenSubsystem::CreateTween(AActor* InActor, const TArray<FTransform>& InPath)
{
    FTweenBuilder Builder;
    Builder.Owner = this;
    Builder.Data.TargetActor = InActor;
    Builder.Data.Path = InPath;
    Builder.Data.TotalDuration = 1.0f;
    Builder.Data.EaseType = EEasingFunc::Linear;
    Builder.Data.Space = ETweenSpace::World; // 默认为世界空间
    Builder.Data.OnComplete = nullptr;

    if (InActor)
    {
        // 默认初始化 StartTransform 为世界坐标，实际 Play 时 RegisterTween 会精细处理
        Builder.Data.StartTransform = InActor->GetActorTransform();
    }

    return Builder;
}

void UGTweenSubsystem::RegisterTween(const FActiveTween& NewTween)
{
    if (!NewTween.TargetActor.IsValid() || NewTween.Path.Num() == 0) return;

    FActiveTween FinalTween = NewTween;
    
    // 根据空间类型，校准初始的 StartTransform
    if (FinalTween.Space == ETweenSpace::Local)
    {
        if (USceneComponent* Root = FinalTween.TargetActor->GetRootComponent())
        {
            FinalTween.StartTransform = Root->GetRelativeTransform();
        }
    }
    else
    {
        FinalTween.StartTransform = FinalTween.TargetActor->GetActorTransform();
    }

    // 保存原始数据供循环重置使用
    FinalTween.OriginalStartTransform = FinalTween.StartTransform;
    FinalTween.OriginalPath = FinalTween.Path;

    ActiveTweens.RemoveAll([&](const FActiveTween& T) {
        return T.TargetActor == FinalTween.TargetActor;
    });

    ActiveTweens.Add(FinalTween);
}

void UGTweenSubsystem::Tick(float DeltaTime)
{
    for (int32 i = ActiveTweens.Num() - 1; i >= 0; --i)
    {
        FActiveTween& T = ActiveTweens[i];
        if (!T.TargetActor.IsValid()) { ActiveTweens.RemoveAt(i); continue; }

        T.ElapsedTime += DeltaTime;
        float SegmentDuration = T.TotalDuration / T.Path.Num();
        float Alpha = FMath::Clamp(T.ElapsedTime / SegmentDuration, 0.f, 1.0f);
        
        float EasedAlpha = UKismetMathLibrary::Ease(0.f, 1.f, Alpha, T.EaseType);
        
        FTransform CurrentTrans;
        CurrentTrans.Blend(T.StartTransform, T.Path[T.CurrentIndex], EasedAlpha);

        // --- 核心迭代：区分空间设置变换 ---
        if (T.Space == ETweenSpace::Local)
        {
            if (USceneComponent* Root = T.TargetActor->GetRootComponent())
            {
                Root->SetRelativeTransform(CurrentTrans);
            }
        }
        else
        {
            T.TargetActor->SetActorTransform(CurrentTrans);
        }

        if (Alpha >= 1.0f)
        {
            T.CurrentIndex++;
            if (T.CurrentIndex >= T.Path.Num())
            {
                // 判断是否还需要继续循环
                bool bShouldLoop = (T.LoopCount == -1) || (T.LoopCount > 0 && T.CompletedLoops < T.LoopCount - 1);

                if (bShouldLoop)
                {
                    T.CompletedLoops++;
                    T.ElapsedTime = 0.f;
                    T.CurrentIndex = 0;

                    if (T.LoopMode == ETweenLoopMode::Restart)
                    {
                        // 将 Actor 瞬间传送回原始起点，重新正向播放
                        T.Path = T.OriginalPath;
                        T.StartTransform = T.OriginalStartTransform;
                        if (T.Space == ETweenSpace::Local)
                        {
                            if (USceneComponent* Root = T.TargetActor->GetRootComponent())
                                Root->SetRelativeTransform(T.OriginalStartTransform);
                        }
                        else
                        {
                            T.TargetActor->SetActorTransform(T.OriginalStartTransform);
                        }
                    }
                    else if (T.LoopMode == ETweenLoopMode::PingPong)
                    {
                        // 翻转路径方向，从当前位置反向播放
                        T.bForwardPlay = !T.bForwardPlay;
                        Algo::Reverse(T.Path);
                        // 当前位置即为新一段的起点
                        if (T.Space == ETweenSpace::Local)
                        {
                            if (USceneComponent* Root = T.TargetActor->GetRootComponent())
                                T.StartTransform = Root->GetRelativeTransform();
                        }
                        else
                        {
                            T.StartTransform = T.TargetActor->GetActorTransform();
                        }
                    }
                    else if (T.LoopMode == ETweenLoopMode::Incremental)
                    {
                        // 计算本次路径的总偏移量，追加到新路径上
                        FTransform EndTransform = T.Path.Last();
                        FVector Offset = EndTransform.GetLocation() - T.OriginalPath[0].GetLocation();
                        FQuat RotOffset = EndTransform.GetRotation() * T.OriginalPath[0].GetRotation().Inverse();
                        FVector ScaleRatio = EndTransform.GetScale3D() / T.OriginalPath[0].GetScale3D();

                        T.Path.Empty();
                        for (const FTransform& OrigTF : T.OriginalPath)
                        {
                            FTransform NewTF;
                            NewTF.SetLocation(OrigTF.GetLocation() + Offset * (T.CompletedLoops));
                            NewTF.SetRotation(RotOffset * OrigTF.GetRotation());
                            NewTF.SetScale3D(OrigTF.GetScale3D());
                            T.Path.Add(NewTF);
                        }
                        // 当前位置作为新段起点
                        if (T.Space == ETweenSpace::Local)
                        {
                            if (USceneComponent* Root = T.TargetActor->GetRootComponent())
                                T.StartTransform = Root->GetRelativeTransform();
                        }
                        else
                        {
                            T.StartTransform = T.TargetActor->GetActorTransform();
                        }
                    }
                }
                else
                {
                    // 所有循环完成（含单次播放），触发回调并移除
                    if (T.OnComplete) T.OnComplete();
                    ActiveTweens.RemoveAt(i);
                }
            }
            else
            {
                // 当前段播完，更新下一段的起点
                if (T.Space == ETweenSpace::Local)
                {
                    T.StartTransform = T.TargetActor->GetRootComponent()->GetRelativeTransform();
                }
                else
                {
                    T.StartTransform = T.TargetActor->GetActorTransform();
                }
                T.ElapsedTime = 0.f;
            }
        }
    }
}