# ✨ GTween 插件使用文档

<div align="center">

## 🎯 让 Actor 动起来，就这么简单

**不写 Tick，不管状态，三行代码完成一个丝滑的路径动画。**  
GTween 接管一切：生命周期、循环逻辑、空间变换，你只需要告诉它「去哪、用多久」。

---

🔗 **链式调用** &nbsp;·&nbsp; 🛡️ **全托管生命周期** &nbsp;·&nbsp; ♾️ **三种循环模式** &nbsp;·&nbsp; 🎞️ **UE 原生缓动函数**

🌍 **世界/局部空间** &nbsp;·&nbsp; 🛤️ **多段路径** &nbsp;·&nbsp; 🧹 **自动清理** &nbsp;·&nbsp; ⚡ **零样板代码**

> 🎮 **兼容 Unreal Engine 5.0 · 5.1 · 5.2 · 5.3 · 5.4 · 5.5 · 5.6 · 5.7 全系列版本**

</div>

---

## 🚀 30 秒上手

```cpp
GTween->CreateTween(MyActor, Path)
    .SetDuration(2.0f)
    .SetEase(EEasingFunc::EaseInOut)
    .SetLoop(-1, ETweenLoopMode::PingPong)
    .Play();
```

> 💡 就这些。Actor 开始来回运动，永不停止。

---

GTween 是一个轻量级的 Actor 运动补间插件，基于 `UEngineSubsystem` 实现，通过链式 API 驱动 Actor 沿多段路径做平滑运动，支持缓动函数、世界/局部空间、循环播放等功能。

> 📦 当前版本：**v1.0.0**

---

## 目录

- [兼容性](#兼容性)
- [接入方式](#接入方式)
- [基本用法](#基本用法)
- [API 参考](#api-参考)
- [循环播放](#循环播放)
- [注意事项](#注意事项)
- [版本历史](#版本历史)

---

## 🎮 兼容性

> **GTween 兼容 Unreal Engine 5.0 至 5.7 的全部版本，无需任何适配即可直接使用。**

| Unreal Engine 版本 | 兼容状态 |
|-------------------|----------|
| UE 5.0 | ✅ 兼容 |
| UE 5.1 | ✅ 兼容 |
| UE 5.2 | ✅ 兼容 |
| UE 5.3 | ✅ 兼容 |
| UE 5.4 | ✅ 兼容 |
| UE 5.5 | ✅ 兼容 |
| UE 5.6 | ✅ 兼容 |
| UE 5.7 | ✅ 兼容 |

---

## 📥 接入方式

在需要使用 GTween 的模块 `Build.cs` 中添加依赖：

```csharp
PrivateDependencyModuleNames.AddRange(new string[] { "GTween" });
```

获取 Subsystem 实例：

```cpp
#include "GTweenSubsystem.h"

UGTweenSubsystem* GTween = GEngine->GetEngineSubsystem<UGTweenSubsystem>();
```

---

## 📖 基本用法

### 单段运动（从当前位置运动到目标点）

```cpp
TArray<FTransform> Path;
Path.Add(TargetTransform);

GTween->CreateTween(MyActor, Path)
    .SetDuration(2.0f)
    .SetEase(EEasingFunc::EaseInOut)
    .Play();
```

### 多段路径运动

路径数组中每个元素表示一个途经点，Actor 依次经过每个点。总时长由 `SetDuration` 设置，每段时长均分。

```cpp
TArray<FTransform> Path;
Path.Add(WaypointA);
Path.Add(WaypointB);
Path.Add(WaypointC);

GTween->CreateTween(MyActor, Path)
    .SetDuration(3.0f)
    .Play();
```

### 完成回调

```cpp
GTween->CreateTween(MyActor, Path)
    .SetDuration(2.0f)
    .OnComplete([this]()
    {
        UE_LOG(LogTemp, Log, TEXT("运动完成"));
    })
    .Play();
```

> ⚠️ **注意**：循环模式下，`OnComplete` 仅在**所有循环全部完成**后触发，不会在每次循环结束时触发。

---

## 📚 API 参考

### `CreateTween(AActor* InActor, const TArray<FTransform>& InPath)`

创建一个 Tween 构造器，返回 `FTweenBuilder`。

| 参数 | 说明 |
|------|------|
| `InActor` | 要运动的目标 Actor |
| `InPath` | 路径点数组（至少 1 个） |

---

### `FTweenBuilder` 链式方法

| 方法 | 默认值 | 说明 |
|------|--------|------|
| `SetDuration(float)` | `1.0f` | 整段路径的总时长（秒），各段均分 |
| `SetEase(EEasingFunc::Type)` | `Linear` | 缓动函数类型，使用 UE 内置 `EEasingFunc` |
| `SetSpace(ETweenSpace)` | `World` | 运动空间：`World`（世界坐标）/ `Local`（相对父组件坐标）|
| `SetLoop(int32, ETweenLoopMode)` | `0, Restart` | 循环设置，详见[循环播放](#循环播放) |
| `OnComplete(FTweenCompleteCallback)` | `nullptr` | 所有运动（含循环）完成后的回调 |
| `Play()` | — | 提交并开始播放，必须最后调用 |

---

### `ETweenSpace` 枚举

| 值 | 说明 |
|----|------|
| `World` | 路径坐标为世界空间，调用 `SetActorTransform` |
| `Local` | 路径坐标为相对父组件的局部空间，调用 `SetRelativeTransform` |

---

## 🔁 循环播放

通过 `SetLoop(int32 LoopCount, ETweenLoopMode Mode)` 设置。

### `LoopCount` 参数

| 值 | 行为 |
|----|------|
| `0` | 单次播放（默认） |
| `-1` | 无限循环 |
| `N > 0` | 循环 N 次（播放总次数为 N+1，即原始一次 + 循环 N 次）|

> **说明**：`LoopCount` 表示**额外重复的次数**，例如 `SetLoop(2)` 表示总共播放 3 遍。

### `ETweenLoopMode` 枚举

| 值 | 行为 |
|----|------|
| `Restart` | 每次循环将 Actor **瞬间传送回原始起点**，然后重新正向播放路径 |
| `PingPong` | 正向播完后**反向播放**，来回交替，Actor 不发生瞬移 |
| `Incremental` | 每次循环在上一次结束位置继续**叠加相同偏移量**向前运动，路径形状不变 |

### 示例

```cpp
// 无限循环，Restart 模式（瞬移回起点重播）
GTween->CreateTween(MyActor, Path)
    .SetDuration(2.0f)
    .SetLoop(-1, ETweenLoopMode::Restart)
    .Play();

// 循环 3 次，PingPong 来回模式
GTween->CreateTween(MyActor, Path)
    .SetDuration(2.0f)
    .SetLoop(3, ETweenLoopMode::PingPong)
    .Play();

// 循环 2 次，Incremental 叠加模式，播完回调
GTween->CreateTween(MyActor, Path)
    .SetDuration(1.5f)
    .SetLoop(2, ETweenLoopMode::Incremental)
    .OnComplete([this]()
    {
        UE_LOG(LogTemp, Log, TEXT("叠加运动全部完成"));
    })
    .Play();
```

---

## ⚠️ 注意事项

1. ✅ **同一 Actor 同时只能有一个 Tween**：对同一 Actor 调用 `Play()` 时，会自动取消其当前正在进行的 Tween 任务。
2. ✅ **Path 至少需要 1 个元素**：空路径会被直接忽略，Tween 不会注册。
3. ✅ **Actor 被销毁时自动清理**：Tick 中会检测 `TargetActor` 的有效性，无效时自动移除对应 Tween。
4. ⚠️ **Incremental 模式仅影响位置和旋转**：缩放在 Incremental 循环中保持原始路径值，不做累积叠加。
5. ⚠️ **LoopCount = 0 时 SetLoop 的第二个参数无效**：单次播放不进入循环逻辑，`LoopMode` 不起作用。

---

## 📋 版本历史

### v1.0.0 (2026-03-11)

**初始版本发布**

- 新增 `CreateTween(AActor*, TArray<FTransform>)` 基础补间创建接口
- 新增多段路径运动支持，路径各段时长均分
- 新增 `SetDuration` 设置总时长
- 新增 `SetEase` 支持 UE 内置所有 `EEasingFunc` 缓动函数
- 新增 `SetSpace` 支持世界坐标（`World`）与局部坐标（`Local`）两种运动空间
- 新增 `SetLoop` 循环播放，支持三种模式：
  - `Restart`：每次循环瞬移回起点重新正向播放
  - `PingPong`：正向播完后反向播放，来回交替，无瞬移
  - `Incremental`：每次循环在上次结束位置叠加相同偏移量继续运动
- 新增 `OnComplete` 回调，在全部循环完成后触发
- 同一 Actor 同时只保留一个 Tween，新建时自动取消旧任务
- Actor 被销毁时自动清理对应 Tween
