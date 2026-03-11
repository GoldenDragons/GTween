[English](README.md) | 中文
# ✨ GTween

<div align="center">

### 🎯 让 Actor 动起来，只需要几行代码

**无需 Tick，无需状态机，无需样板代码。**

只需三行代码，即可创建流畅的 Actor 运动动画。  
GTween 自动管理生命周期、循环逻辑与空间变换 ——  
你只需要告诉它 **去哪，以及多久到达**。

---

🔗 **链式 API** · 🛡 **全托管生命周期** · ♾ **多种循环模式** · 🎞 **UE 原生缓动函数**

🌍 **世界 / 局部空间** · 🛤 **多段路径** · 🧹 **自动清理** · ⚡ **零样板代码**

> 🎮 **兼容 Unreal Engine 5.0 – 5.7 全系列版本**

</div>

---

# 🚀 30 秒上手

```cpp
GTween->CreateTween(MyActor, Path)
    .SetDuration(2.0f)
    .SetEase(EEasingFunc::EaseInOut)
    .SetLoop(-1, ETweenLoopMode::PingPong)
    .Play();
````

💡 Actor 将开始来回运动，并持续循环。

---

# 📦 GTween 是什么

**GTween** 是一个轻量级的 **Unreal Engine Actor 补间动画插件**。

它基于 **`UEngineSubsystem`** 实现，通过链式 API 让 Actor 沿路径进行平滑运动，并提供：

* 缓动函数（Easing）
* 世界 / 局部空间控制
* 循环播放
* 自动生命周期管理

整个过程 **无需编写 Tick 逻辑**。

> 当前版本：**v1.0.0**

---

# 📚 目录

* [兼容性](#-兼容性)
* [安装方式](#-安装方式)
* [基本用法](#-基本用法)
* [API 参考](#-api-参考)
* [循环播放](#-循环播放)
* [注意事项](#-注意事项)
* [版本历史](#-版本历史)

---

# 🎮 兼容性

GTween 可直接在 **所有 Unreal Engine 5 版本**中使用。

| Unreal Engine | 状态   |
| ------------- | ---- |
| UE 5.0        | ✅ 支持 |
| UE 5.1        | ✅ 支持 |
| UE 5.2        | ✅ 支持 |
| UE 5.3        | ✅ 支持 |
| UE 5.4        | ✅ 支持 |
| UE 5.5        | ✅ 支持 |
| UE 5.6        | ✅ 支持 |
| UE 5.7        | ✅ 支持 |

---

# 📥 安装方式

在你的模块 `Build.cs` 中添加依赖：

```csharp
PrivateDependencyModuleNames.AddRange(new string[] { "GTween" });
```

然后获取 Subsystem：

```cpp
#include "GTweenSubsystem.h"

UGTweenSubsystem* GTween =
    GEngine->GetEngineSubsystem<UGTweenSubsystem>();
```

---

# 📖 基本用法

## 单段运动

将 Actor 从当前 Transform 移动到目标 Transform。

```cpp
TArray<FTransform> Path;
Path.Add(TargetTransform);

GTween->CreateTween(MyActor, Path)
    .SetDuration(2.0f)
    .SetEase(EEasingFunc::EaseInOut)
    .Play();
```

---

## 多段路径运动

路径数组中的每个元素代表一个途经点。

```cpp
TArray<FTransform> Path;

Path.Add(WaypointA);
Path.Add(WaypointB);
Path.Add(WaypointC);

GTween->CreateTween(MyActor, Path)
    .SetDuration(3.0f)
    .Play();
```

总时长会在各段路径之间 **平均分配**。

---

## 完成回调

```cpp
GTween->CreateTween(MyActor, Path)
    .SetDuration(2.0f)
    .OnComplete([this]()
    {
        UE_LOG(LogTemp, Log, TEXT("运动完成"));
    })
    .Play();
```

⚠️ 如果启用了循环，`OnComplete` 只会在 **所有循环结束之后** 才触发。

---

# 📚 API 参考

## CreateTween

```cpp
CreateTween(AActor* InActor, const TArray<FTransform>& InPath)
```

| 参数        | 说明                      |
| --------- | ----------------------- |
| `InActor` | 目标 Actor                |
| `InPath`  | 路径 Transform 数组（至少 1 个） |

返回 **`FTweenBuilder`**。

---

## FTweenBuilder

链式配置 API。

| 方法                               | 默认值          | 说明                 |
| -------------------------------- | ------------ | ------------------ |
| `SetDuration(float)`             | `1.0f`       | 动画总时长              |
| `SetEase(EEasingFunc::Type)`     | `Linear`     | Unreal Engine 缓动函数 |
| `SetSpace(ETweenSpace)`          | `World`      | 运动空间               |
| `SetLoop(int32, ETweenLoopMode)` | `0, Restart` | 循环配置               |
| `OnComplete(Callback)`           | `nullptr`    | 动画全部结束时回调          |
| `Play()`                         | —            | 启动 Tween（必须最后调用）   |

---

## ETweenSpace

| 值       | 说明                        |
| ------- | ------------------------- |
| `World` | 使用 `SetActorTransform`    |
| `Local` | 使用 `SetRelativeTransform` |

---

# 🔁 循环播放

通过以下方法配置循环：

```cpp
.SetLoop(int32 LoopCount, ETweenLoopMode Mode)
```

---

## LoopCount

| 值       | 行为       |
| ------- | -------- |
| `0`     | 只播放一次    |
| `-1`    | 无限循环     |
| `N > 0` | 额外重复 N 次 |

例如：

```cpp
SetLoop(2)
```

表示 **总共播放 3 次**（原始播放 + 2 次循环）。

---

## ETweenLoopMode

| 模式            | 行为         |
| ------------- | ---------- |
| `Restart`     | 每次循环瞬移回起点  |
| `PingPong`    | 每次循环反向播放   |
| `Incremental` | 每次循环叠加相同偏移 |

---

## 示例

### 无限 Restart 循环

```cpp
GTween->CreateTween(MyActor, Path)
    .SetDuration(2.0f)
    .SetLoop(-1, ETweenLoopMode::Restart)
    .Play();
```

---

### PingPong 来回循环

```cpp
GTween->CreateTween(MyActor, Path)
    .SetDuration(2.0f)
    .SetLoop(3, ETweenLoopMode::PingPong)
    .Play();
```

---

### Incremental 叠加循环

```cpp
GTween->CreateTween(MyActor, Path)
    .SetDuration(1.5f)
    .SetLoop(2, ETweenLoopMode::Incremental)
    .OnComplete([this]()
    {
        UE_LOG(LogTemp, Log, TEXT("叠加运动完成"));
    })
    .Play();
```

---

# ⚠️ 注意事项

1️⃣ **同一 Actor 只能同时存在一个 Tween**

创建新的 Tween 时，会自动取消之前的 Tween。

2️⃣ **路径至少包含一个元素**

空路径会被忽略。

3️⃣ **自动清理**

如果 Actor 被销毁，对应的 Tween 会自动移除。

4️⃣ **Incremental 模式**

仅 **位置和旋转** 会累积叠加。
缩放不会累积。

5️⃣ **LoopCount = 0**

没有循环时，LoopMode 不会生效。

---

# 📋 版本历史

## v1.0.0 — 2026-03-11

初始版本发布。

### 功能

* `CreateTween(AActor*, TArray<FTransform>)`
* 支持多段路径运动
* `SetDuration` 设置动画总时长
* `SetEase` 支持 UE `EEasingFunc`
* `SetSpace` 支持 World / Local 空间
* `SetLoop` 循环播放（3 种模式）

  * `Restart`
  * `PingPong`
  * `Incremental`
* `OnComplete` 完成回调
* 同一 Actor 同时只允许一个 Tween
* Actor 销毁时自动清理 Tween

