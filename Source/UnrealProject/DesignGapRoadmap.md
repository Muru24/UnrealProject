# Spline Chase Design Gap Roadmap

## Goal

Align the current prototype with the project pitch without changing the existing `AActor` and `APawn` inheritance structure.

## Current Prototype Summary

- `AP_Player` is a single controllable pawn with camera, lock-on, weapon, and spline follower components.
- `AP_Enemy` is a generic spline-following enemy pawn.
- `UPathFollowerComponent` drives direct spline movement.
- `ULockOnComponent` supports simple target selection and `UCraftAttackComponent` handles per-craft firing.
- There is no squad system, swap flow, boss phase logic, or lane-offset dodge layer yet.

## Target Gameplay Pillars

1. Spline auto chase
2. XY dodge and positioning on top of spline motion
3. Three-unit squad with live swap
4. Boss raid phase flow
5. Section-based rail gimmicks

## Recommended Architecture Direction

Keep `AP_Player` as the player-controlled root pawn and extend it through composition.

### 1. Rail Motion Layer

Keep `UPathFollowerComponent`, but narrow its responsibility:

- Own only forward travel along spline distance
- Expose current spline transform
- Support speed modifiers from gimmicks
- Stop writing the final actor transform directly once lateral control is added

Add a new component:

- `URailOffsetComponent`
  - Own local X and Y dodge offset
  - Clamp playable movement bounds
  - Apply smoothing
  - Combine spline transform with local offset

Recommended runtime flow:

1. `UPathFollowerComponent` computes spline distance and base transform.
2. `URailOffsetComponent` computes player dodge offset from input.
3. A small coordinator component or the owner pawn combines both into final transform.

This preserves the rail-shooter feel while enabling bullet hell avoidance.

### 2. Squad and Swap Layer

Do not replace `AP_Player` with a different inheritance tree.

Add:

- `USquadComponent`
  - Own squad member references
  - Track current active slot
  - Expose swap requests
  - Broadcast active member changes via delegate

- `USquadFormationComponent`
  - Maintain left, center, right slot offsets
  - Reposition sub units relative to the squad root
  - Support pattern-driven formation changes later

- `USquadMemberComponent`
  - Identify each craft as Main, LeftSub, or RightSub
  - Hold per-unit metadata for UI and ability routing

- `USquadAutoFireComponent`
  - Run support-fire logic for inactive sub craft
  - Query available targets through interface or lock service

Recommended ownership:

- `AP_Player` remains the squad root and main input receiver.
- Sub craft should be separate attached actors if you want distinct HP, weak-point targeting, death, or visual independence.
- If production scope is tighter, sub craft can begin as child scene components and later be promoted to actors.

Swap flow:

1. `AP_Player` receives `Q/E`.
2. `USquadComponent` validates target slot.
3. `USquadComponent` updates active member.
4. Delegates notify camera, weapon routing, HUD, and formation visuals.

### 3. Firing Model Split

Current firing is too generic for the design.

Split responsibilities:

- Keep `UCraftAttackComponent` as low-level per-craft projectile spawn logic.
- Add `UAimProviderComponent` or an interface such as `IAimTargetProvider`.
- Add `UPlayerAimFireComponent`
  - Main craft fires at mouse-driven aim point
  - Can optionally override to lock-on target

- Add `USupportFireComponent`
  - Sub craft chooses target automatically
  - Can prioritize boss weak points or nearest threats

This lets main and sub craft follow different targeting rules without duplicating weapon code.

### 4. Boss Raid Layer

Current enemy structure is too flat for raid gameplay.

Add boss-specific components instead of overloading `AP_Enemy` directly:

- `UBossPhaseComponent`
  - Track phase state
  - Advance phase by HP thresholds or scripted triggers

- `UBossPatternComponent`
  - Trigger base attacks, wide-area checks, and wipe patterns
  - Expose pattern start and end delegates

- `UBossWeakPointComponent`
  - Manage weak point exposure windows
  - Report interruption progress

- `UInterruptSequenceComponent`
  - Track ordered marker destruction
  - Validate fail/success conditions

- `URailHazardComponent`
  - Spawn or manage spline occupation attacks, broken-track events, and lane denial

Recommended rule:

- Treat boss mechanics as data-driven state machines attached to a boss pawn.
- Keep `AP_Enemy` for standard enemies.
- Create a dedicated boss pawn subclass only when boss-only actor behavior is truly different.

### 5. Section and Gimmick Layer

`ASplinePathManager` is the right place conceptually, but it needs responsibility.

Refactor toward:

- `ASplinePathManager`
  - Own active path references
  - Expose section metadata
  - Notify listeners when entering chase, evade, or intercept segments

- `URailSectionListenerComponent`
  - Attach to player or boss
  - React to section changes

- `UGimmickSpeedModifierComponent`
  - Apply slipstream, gate boosts, slow zones, or scripted chase compression

Section examples:

- Chase section: boost gates, slipstream bonuses, distance closing
- Survival section: lane denial, wall shifts, heavy projectile density
- Intercept section: weak point windows, anchors, interrupt markers

## Immediate Gap List

These are the highest-value missing systems in order.

1. Local XY dodge offset on top of spline travel
2. Squad member ownership model
3. Active member swap input and event flow
4. Main-fire and support-fire separation
5. Damage application from bullets into `UStatComponent`
6. Boss phase and interrupt state model
7. Section/gimmick manager hooked to spline progression

## Suggested Build Order

### Phase 1: Make the current prototype playable

1. Add bullet damage application
2. Add XY rail offset movement
3. Feed a valid spline path into `UPathFollowerComponent`

### Phase 2: Add the core identity

1. Add `USquadComponent`
2. Add swap input for `Q/E`
3. Add sub craft support-fire behavior

### Phase 3: Add raid gameplay

1. Add boss pawn with phase component
2. Add weak point exposure and interrupt logic
3. Add section-based gimmick events

## Concrete Mapping From Existing Code

### Keep

- `APawn_Template`
- `AP_Player`
- `AP_Enemy`
- `UPathFollowerComponent`
- `UCraftAttackComponent`
- `UStatComponent`

### Change Responsibility

- `UPathFollowerComponent`
  - From final full transform owner
  - To spline-distance and base-transform provider

- `ULockOnComponent`
  - From simple global nearest-target selector
  - To reusable target selection service with boss priority support

- `UCraftAttackComponent`
  - From all-in-one firing behavior
  - To projectile spawn executor

### Add

- `URailOffsetComponent`
- `USquadComponent`
- `USquadFormationComponent`
- `USupportFireComponent`
- `UBossPhaseComponent`
- `UBossWeakPointComponent`
- `UInterruptSequenceComponent`
- `URailHazardComponent`

## Risks In The Current Code

- `UPathFollowerComponent::SetTargetPath()` is not wired into runtime usage.
- Player input currently has no XY movement path.
- Bullet collision destroys bullets but does not apply gameplay damage.
- `ULockOnComponent` is built around a flat enemy list, not boss weak points or tactical priorities.
- `ASplinePathManager` currently has no gameplay role.

## Best Next Implementation Slice

If we start coding next, the best first slice is:

1. Add `URailOffsetComponent`
2. Refactor `UPathFollowerComponent` to expose base spline transform
3. Combine spline transform plus offset inside `AP_Player`
4. Add projectile-to-`UStatComponent` damage

That slice creates the minimum gameplay loop that matches the rail shooter pitch before introducing squad swap complexity.
