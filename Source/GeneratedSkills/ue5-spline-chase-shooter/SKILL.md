---
name: ue5-spline-chase-shooter
description: Unreal Engine 5 C++ and Blueprint development guidance for the spline-based chase shooter project. Use when working on gameplay code, components, Blueprints, actors, pawns, or architecture in this project and you must preserve the existing class hierarchy while applying composition-first design and Unreal coding conventions.
---

# UE5 Spline Chase Shooter

Follow these project rules whenever you work in this codebase.

## Core Workflow

1. Inspect the existing actor and pawn structure before proposing changes.
2. Preserve the current class hierarchy unless the user explicitly requests a structural change.
3. Prefer adding isolated `UActorComponent` units over pushing more behavior into a single actor or pawn class.
4. Connect systems through interfaces, delegates, or clearly owned events instead of hard component-to-component coupling.
5. Verify Unreal naming, reflection macros, categories, and editor exposure before finishing.

## Architecture Rules

- Prefer composition over inheritance.
- Treat feature additions as `Has-a` design work first.
- Keep each component focused on one gameplay responsibility.
- Avoid direct dependency chains between sibling components.
- If coordination is required, route it through the owner, an interface, or delegates.

## Unreal Conventions

- Use Unreal Engine naming conventions, including `PascalCase` for reflected members in this project.
- Add `UPROPERTY` and `UFUNCTION` specifiers intentionally rather than by habit.
- Assign meaningful `Category` values for reflected properties and functions.
- Keep Blueprint exposure minimal and purposeful.

## Project Context

- The game is a spline-based chase shooter.
- Movement, targeting, combat, or encounter logic should respect spline-driven flow and pursuit gameplay.
- Favor reusable gameplay pieces that can be attached to multiple actors when that preserves the current hierarchy.

## Reference

Read [references/project-principles.md](references/project-principles.md) when you need the full project rules and the stored PDF companion document.
