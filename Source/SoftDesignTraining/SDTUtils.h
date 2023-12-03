// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AI/Navigation/NavigationTypes.h"
#include "NavMesh/NavMeshPath.h"

#define COLLISION_DEATH_OBJECT		ECollisionChannel::ECC_GameTraceChannel3
#define COLLISION_PLAYER        	ECollisionChannel::ECC_GameTraceChannel4
#define COLLISION_COLLECTIBLE     	ECollisionChannel::ECC_GameTraceChannel5

class SOFTDESIGNTRAINING_API SDTUtils
{
public:
    static bool Raycast(UWorld* uWorld, FVector sourcePoint, FVector targetPoint);
    static bool IsPlayerPoweredUp(UWorld* uWorld);

    enum NavType
    {
        Default,
        Jump
    };

    static bool IsNavTypeFlagSet(uint16 flags, NavType type) { return (flags & (1 << type)) != 0; }
    static void SetNavTypeFlag(uint16& flags, NavType type) { flags |= (1 << type); }

    static bool IsNavLink(const FNavPathPoint& PathVert) { return (FNavMeshNodeFlags(PathVert.Flags).PathFlags & RECAST_STRAIGHTPATH_OFFMESH_CONNECTION) != 0; }
    static bool HasJumpFlag(const FNavPathPoint& PathVert) { return     IsNavTypeFlagSet(FNavMeshNodeFlags(PathVert.Flags).AreaFlags, NavType::Jump); }

    /// Call function and measure its execution time.
    ///
    /// The function is called with std::invoke, passing forwarded args as the arguments.
    /// \return A pair where the first element is the elapsed time in milliseconds and the second element is the result of the call to function.
    template<class Functor, class... Args, typename = std::enable_if_t<!std::is_void<std::invoke_result_t<Functor, Args...>>::value>>
    static std::pair<double, std::invoke_result_t<Functor, Args...>> MeasureExecutionTime(Functor function, Args&&... args)
    {
        double beginSearchCollectible = FPlatformTime::Seconds();
        auto result = std::invoke(function, std::forward<Args>(args)...);
        double endSearchCollectible = FPlatformTime::Seconds();
        return std::make_pair((endSearchCollectible - beginSearchCollectible) * 1000, result);
    }

    /// Call function and measure its execution time.
    ///
    /// The function is called with std::invoke, passing forwarded args as the arguments.
    /// \return The time elapsed in milliseconds during the call to function.
    template<class Functor, class... Args, typename = std::enable_if_t<std::is_void<std::invoke_result_t<Functor, Args...>>::value>>
    static double MeasureExecutionTime(Functor function, Args&&... args)
    {
        double beginSearchCollectible = FPlatformTime::Seconds();
        std::invoke(function, std::forward<Args>(args)...);
        double endSearchCollectible = FPlatformTime::Seconds();
        return (endSearchCollectible - beginSearchCollectible) * 1000;
    }
};
