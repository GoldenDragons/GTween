
English | [中文](README.zh.md)
# ✨ GTween

<div align="center">

### 🎯 Make Actors Move — With Just a Few Lines of Code

**No Tick. No state machines. No boilerplate.**

Create smooth Actor motion with **three lines of code**.  
GTween manages lifecycle, looping, and transforms —  
you simply define **where to go and how long it takes**.

---

🔗 **Fluent API** · 🛡 **Fully Managed Lifecycle** · ♾ **Multiple Loop Modes** · 🎞 **UE Native Easing**

🌍 **World / Local Space** · 🛤 **Multi-Segment Paths** · 🧹 **Auto Cleanup** · ⚡ **Zero Boilerplate**

> 🎮 **Compatible with Unreal Engine 5.0 – 5.7**

</div>

---

# 🚀 Quick Start (30 seconds)

```cpp
GTween->CreateTween(MyActor, Path)
    .SetDuration(2.0f)
    .SetEase(EEasingFunc::EaseInOut)
    .SetLoop(-1, ETweenLoopMode::PingPong)
    .Play();
````

💡 Actor will now move back and forth forever.

---

# 📦 What is GTween

**GTween** is a lightweight **Actor tween animation plugin** for Unreal Engine.

It is built on **`UEngineSubsystem`**, providing a fluent API to move Actors smoothly along paths with:

* easing functions
* world/local space control
* loop playback
* lifecycle management

All without writing Tick logic.

> Current Version: **v1.0.0**

---

# 📚 Table of Contents

* [Compatibility](#-compatibility)
* [Installation](#-installation)
* [Basic Usage](#-basic-usage)
* [API Reference](#-api-reference)
* [Loop Playback](#-loop-playback)
* [Notes](#-notes)
* [Version History](#-version-history)

---

# 🎮 Compatibility

GTween works out-of-the-box with **all Unreal Engine 5 versions**.

| Unreal Engine | Status      |
| ------------- | ----------- |
| UE 5.0        | ✅ Supported |
| UE 5.1        | ✅ Supported |
| UE 5.2        | ✅ Supported |
| UE 5.3        | ✅ Supported |
| UE 5.4        | ✅ Supported |
| UE 5.5        | ✅ Supported |
| UE 5.6        | ✅ Supported |
| UE 5.7        | ✅ Supported |

---

# 📥 Installation

Add the module dependency in your `Build.cs`:

```csharp
PrivateDependencyModuleNames.AddRange(new string[] { "GTween" });
```

Then obtain the subsystem:

```cpp
#include "GTweenSubsystem.h"

UGTweenSubsystem* GTween =
    GEngine->GetEngineSubsystem<UGTweenSubsystem>();
```

---

# 📖 Basic Usage

## Single Segment Motion

Move an Actor from its current transform to a target transform.

```cpp
TArray<FTransform> Path;
Path.Add(TargetTransform);

GTween->CreateTween(MyActor, Path)
    .SetDuration(2.0f)
    .SetEase(EEasingFunc::EaseInOut)
    .Play();
```

---

## Multi-Segment Path

Each element in the path array represents a waypoint.

```cpp
TArray<FTransform> Path;

Path.Add(WaypointA);
Path.Add(WaypointB);
Path.Add(WaypointC);

GTween->CreateTween(MyActor, Path)
    .SetDuration(3.0f)
    .Play();
```

The total duration is evenly distributed across segments.

---

## Completion Callback

```cpp
GTween->CreateTween(MyActor, Path)
    .SetDuration(2.0f)
    .OnComplete([this]()
    {
        UE_LOG(LogTemp, Log, TEXT("Motion finished"));
    })
    .Play();
```

⚠️ When looping is enabled, `OnComplete` fires **only after all loops finish**.

---

# 📚 API Reference

## CreateTween

```cpp
CreateTween(AActor* InActor, const TArray<FTransform>& InPath)
```

| Parameter | Description                 |
| --------- | --------------------------- |
| `InActor` | Target Actor                |
| `InPath`  | Path transforms (minimum 1) |

Returns **`FTweenBuilder`**.

---

## FTweenBuilder

Fluent configuration API.

| Method                           | Default      | Description                    |
| -------------------------------- | ------------ | ------------------------------ |
| `SetDuration(float)`             | `1.0f`       | Total animation duration       |
| `SetEase(EEasingFunc::Type)`     | `Linear`     | Unreal Engine easing function  |
| `SetSpace(ETweenSpace)`          | `World`      | Motion space                   |
| `SetLoop(int32, ETweenLoopMode)` | `0, Restart` | Loop configuration             |
| `OnComplete(Callback)`           | `nullptr`    | Called when animation finishes |
| `Play()`                         | —            | Start tween (must be last)     |

---

## ETweenSpace

| Value   | Description                 |
| ------- | --------------------------- |
| `World` | Uses `SetActorTransform`    |
| `Local` | Uses `SetRelativeTransform` |

---

# 🔁 Loop Playback

Configure looping with:

```cpp
.SetLoop(int32 LoopCount, ETweenLoopMode Mode)
```

---

## LoopCount

| Value   | Behavior       |
| ------- | -------------- |
| `0`     | Play once      |
| `-1`    | Infinite loop  |
| `N > 0` | Repeat N times |

Example:

```cpp
SetLoop(2)
```

Plays **3 times total** (initial + 2 repeats).

---

## ETweenLoopMode

| Mode          | Behavior                         |
| ------------- | -------------------------------- |
| `Restart`     | Teleport back to start each loop |
| `PingPong`    | Reverse direction each loop      |
| `Incremental` | Apply the same offset repeatedly |

---

## Examples

### Infinite Restart Loop

```cpp
GTween->CreateTween(MyActor, Path)
    .SetDuration(2.0f)
    .SetLoop(-1, ETweenLoopMode::Restart)
    .Play();
```

---

### PingPong Loop

```cpp
GTween->CreateTween(MyActor, Path)
    .SetDuration(2.0f)
    .SetLoop(3, ETweenLoopMode::PingPong)
    .Play();
```

---

### Incremental Loop With Callback

```cpp
GTween->CreateTween(MyActor, Path)
    .SetDuration(1.5f)
    .SetLoop(2, ETweenLoopMode::Incremental)
    .OnComplete([this]()
    {
        UE_LOG(LogTemp, Log, TEXT("Incremental motion finished"));
    })
    .Play();
```

---

# ⚠️ Notes

1️⃣ **Only one Tween per Actor**

Creating a new Tween automatically cancels the previous one.

2️⃣ **Path must contain at least one element**

Empty paths are ignored.

3️⃣ **Automatic cleanup**

If the Actor is destroyed, the Tween is removed automatically.

4️⃣ **Incremental mode**

Only position and rotation accumulate.
Scale remains unchanged.

5️⃣ **LoopCount = 0**

Loop mode has no effect when no looping occurs.

---

# 📋 Version History

## v1.0.0 — 2026-03-11

Initial release.

### Features

* `CreateTween(AActor*, TArray<FTransform>)`
* Multi-segment path animation
* `SetDuration` for total duration
* `SetEase` supporting UE `EEasingFunc`
* `SetSpace` (World / Local)
* `SetLoop` with three modes:

  * `Restart`
  * `PingPong`
  * `Incremental`
* `OnComplete` callback
* Single Tween per Actor enforcement
* Automatic cleanup when Actor is destroyed
