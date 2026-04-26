# Loadout Authoring Guide

## Purpose

This guide explains where combat settings should live after the loadout refactor.

Use this as the single reference when editing `BP_SquadCraft_*` Blueprints.

## Main Rule

Combat behavior should be configured in `LoadoutComponent`, not directly in `AttackComponent`.

Think of the current setup like this:

- `SquadCraftActor`
  - owns the craft actor itself
  - controls transform, active state, and visual presentation
- `AttackComponent`
  - executes projectile spawning at runtime
  - should not be treated as the authoring surface for craft tuning
- `LoadoutComponent`
  - is the authoring surface for combat identity
  - stores projectile, attack pattern, attack type, and skill slot data

## Where To Edit Values

### Edit Here: `LoadoutComponent > LoadoutData`

These values should be edited in the craft Blueprint loadout:

- `LoadoutId`
- `DisplayName`
- `CombatRole`
- `ProjectileClass`
- `AttackType`
- `AttackPattern`
- `MaxPenetrationCount`
- `ExplosionRadius`
- `BurstCount`
- `SpreadCount`
- `SpreadAngle`
- `MultiShotSpacing`
- `AutoFireInterval`
- `BuffSkill`
- `OffensiveSkill`

### Do Not Edit Here: `AttackComponent`

`AttackComponent` now holds runtime values only.

Its job is:

- receive loadout data
- spawn projectiles
- apply burst and spread logic
- manage runtime auto-fire cooldown

If combat tuning appears wrong, fix the loadout instead of trying to tune `AttackComponent`.

## What Still Belongs On `SquadCraftActor`

These values are still actor presentation settings and can stay on the craft actor:

- `TransformInterpSpeed`
- `ActiveScale`
- `InactiveScale`

These are not part of weapon identity.
They control how the craft moves and looks inside the squad.

## Combat Role Behavior

`CombatRole` now affects support-fire behavior.

### `MainGun`

- intended for the directly controlled craft
- does not support-fire while inactive

### `SupportRapid`

- prefers the nearest available enemy
- works well for frequent chip damage and cleanup

### `SupportHeavy`

- prefers the current lock-on target when possible
- falls back to the general preferred target if no lock-on target exists
- works well for focused burst support on priority targets

## Recommended Blueprint Workflow

When making a new squad craft Blueprint:

1. Set mesh and visual values on the actor.
2. Open `LoadoutComponent`.
3. Fill out `LoadoutData`.
4. Leave `AttackComponent` alone unless debugging runtime behavior.

## Recommended Naming

Use loadout IDs that describe the craft role, not only the mesh.

Examples:

- `Main_Balanced`
- `Support_Rapid`
- `Support_HeavyExplosive`
- `Support_Piercing`

This makes it easier to reuse loadouts later across different craft visuals.
