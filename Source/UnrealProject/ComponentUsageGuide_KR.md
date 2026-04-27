# 컴포넌트 역할 및 사용 가이드

## 목적

이 문서는 현재 프로젝트에서 사용하는 주요 액터와 컴포넌트의 역할, 소유 주체, 설정 위치, 실제 사용 흐름을 빠르게 확인하기 위한 참조 문서입니다.

구조 리팩토링 배경과 기술 부채는 `CodebaseRefactorGuide_KR.md`를 참고하고,
이 문서는 "어디를 만져야 하는가" 중심으로 정리합니다.

## 전체 구조 한눈에 보기

- `AP_Player`
  - 입력 수신과 상위 조정
- 플레이어 루트 컴포넌트
  - 락온, 레일 이동, 카메라, 메인 발사, 서브 자동사격
- `ASquadCraftActor`
  - 개별 기체 단위
- 기체 소유 컴포넌트
  - 공격 실행, 로드아웃, 스킬
- `ABulletBase`
  - 실제 탄환 처리
- `ABeamEffectActor`
  - 빔 이펙트 공통 부모
- `AFunnel`
  - 타겟 주변을 도는 판넬 액터
- `USkillSummonPanel`
  - 판넬을 소환하는 공격 스킬 효과

## 플레이어 루트

### `AP_Player`

#### 역할

- 입력 바인딩
- 각 기능 컴포넌트 호출 순서 조정
- 현재 활성 기체 기준 발사/스킬 사용 진입점 제공

#### 직접 처리하는 것

- `Fire()`
- `SwapSquadLeft()`, `SwapSquadRight()`
- `TriggerOffensiveSkill()`, `TriggerBuffSkill()`
- Tick에서 레일 이동, 카메라, 서브 자동사격 호출

#### UE에서 볼 값

- 플레이어 BP의 기본 컴포넌트 배치
- `SquadRuntimeComponent`의 `LeftCraftClass`, `CenterCraftClass`, `RightCraftClass`

## 플레이어 소유 컴포넌트

### `ULockOnComponent`

#### 역할

- 락온 on/off 상태 관리
- 현재 타겟 저장
- 가장 가까운 적 선택
- 다음 타겟 순환

#### 주요 함수

- `TraceTarget()`
- `ChangeTarget()`
- `IsLockOnEnabled()`
- `GetCurrentTarget()`

### `UPathFollowerComponent`

#### 역할

- 스플라인을 따라 기본 전진 이동 계산
- 기본 월드 트랜스폼 제공
- 순간 가속 처리

#### 주요 함수

- `SetTargetPath()`
- `SetAcceleration()`
- `GetBaseWorldTransform()`
- `IsAccelerationActive()`

### `URailOffsetComponent`

#### 역할

- X/Y 입력 기반 이격 이동 계산
- 입력이 없을 때 원위치 복귀 보간
- 시각용 뱅킹 회전 계산

#### 주요 함수

- `SetHorizontalInput()`
- `SetVerticalInput()`
- `GetCurrentOffset()`
- `GetVisualTiltRotation()`

### `USquadComponent`

#### 역할

- 활성 슬롯 관리
- 좌/중앙/우 슬롯 오프셋 계산

#### 주요 함수

- `ShiftActiveSlotLeft()`
- `ShiftActiveSlotRight()`
- `GetActiveSlot()`
- `GetSlotOffset()`
- `GetAllSlots()`

### `USquadRuntimeComponent`

#### 역할

- 좌/중앙/우 기체 스폰
- 스폰된 기체 참조 보관
- 활성 슬롯에 맞춰 기체 상태 갱신
- 편대 시각 회전 적용

#### UE에서 주로 만질 값

- `LeftCraftClass`
- `CenterCraftClass`
- `RightCraftClass`

### `UPlayerAimFireComponent`

#### 역할

- 메인 기체 발사 조준 판단
- 락온 중이면 락온 타겟 방향
- 아니면 마우스 방향

#### 주요 함수

- `FireActiveCraft()`

### `USupportFireComponent`

#### 역할

- 비활성 기체 자동사격 판단
- `CombatRole`에 따라 타겟 우선순위 차등 적용

#### 역할별 기준

- `MainGun`
  - 자동사격 안 함
- `SupportRapid`
  - 가까운 적 우선
- `SupportHeavy`
  - 락온 타겟 우선, 없으면 일반 적

### `UPlayerCameraRigComponent`

#### 역할

- 마우스 기반 카메라 팬
- 활성 기체 기준 카메라 앵커 추적
- 가속 중 줌 아웃 / 복귀 보간

#### 주요 함수

- `UpdateCameraPan()`
- `UpdateCameraAnchor()`
- `UpdateCameraZoom()`

### `UPlayerRailMovementComponent`

#### 역할

- `UPathFollowerComponent`와 `URailOffsetComponent` 결과를 조합
- 플레이어 최종 위치/회전 적용
- 활성 기체의 시각 기울기 전달

#### 주요 함수

- `ApplyRailMovement()`

## 기체 액터

### `ASquadCraftActor`

#### 역할

- 개별 기체 표현 단위
- 메쉬, 파이어 오리진, 공격/로드아웃/스킬 컴포넌트 소유

#### 포함 컴포넌트

- `SceneRoot`
- `VisualRoot`
- `CraftMesh`
- `FireOrigin`
- `UCraftAttackComponent`
- `UCraftLoadoutComponent`
- `USkillComponent`

#### 주요 함수

- `FireAt()`
- `TryAutoFireAt()`
- `TryActivateOffensiveSkill()`
- `TryActivateBuffSkill()`
- `SetActiveCraft()`
- `SetDesiredRelativeTransform()`
- `SetVisualTiltRotation()`

## 기체 소유 컴포넌트

### `UCraftLoadoutComponent`

#### 역할

- 기체 데이터의 유일한 설정 창구
- 공격/스킬 설정을 실행 컴포넌트에 전달

#### 구조

- `Identity`
  - `LoadoutId`
  - `DisplayName`
  - `CombatRole`
- `AttackConfig`
  - 발사 방식, 탄환 클래스, 관통/폭발 설정
- `SkillConfig`
  - 버프 스킬
  - 공격 스킬
- `PresentationConfig`
  - 표현 관련 식별 정보

#### UE에서 주로 만질 값

- `LoadoutComponent > LoadoutData`

#### 원칙

- 공격 설정은 `AttackComponent`가 아니라 `LoadoutData.AttackConfig`에서 수정
- 스킬 설정은 `SkillComponent`가 아니라 `LoadoutData.SkillConfig`에서 수정

### `UCraftAttackComponent`

#### 역할

- 실제 탄환 발사 실행
- 단발 / 버스트 / 스프레드 처리
- 자동사격 쿨다운 관리

#### 주요 함수

- `ApplyAttackConfig()`
- `FireFromOrigin()`
- `TryAutoFireFromOrigin()`

### `USkillComponent`

#### 역할

- 버프/공격 스킬 슬롯 보관
- 스킬 쿨다운 관리
- 실제 스킬 발동 진입점 제공

#### 주요 함수

- `SetBuffSkill()`
- `SetOffensiveSkill()`
- `TryActivateBuffSkill()`
- `TryActivateOffensiveSkill()`

#### 현재 원칙

- 에디터에서 직접 세팅하는 컴포넌트가 아니라 런타임 실행용
- 값은 `LoadoutComponent`에서 내려주는 구조를 유지

## 스킬 시스템

### `USkillEffectBase`

#### 역할

- 스킬 효과 공통 베이스

#### 파생 구조

- `UBuffSkillEffect`
- `UOffensiveSkillEffect`

### `FSkillSpec`

#### 역할

- 스킬 데이터 정의

#### 주요 값

- `SkillId`
- `DisplayName`
- `SkillType`
- `Cooldown`
- `Duration`
- `EffectClass`

### `USkillSummonPanel`

#### 역할

- 공격 스킬 효과 객체
- 판넬(`AFunnel`)을 여러 개 소환하고 타겟을 결정

#### 타겟 우선순위

- 스킬 호출 시 직접 넘긴 `TargetActor`
- 현재 락온 타겟
- `EnemyManager`의 가장 가까운 적

#### 주요 설정값

- `FunnelClass`
- `FunnelCount`
- `FunnelSpawnRadius`
- `FunnelSpawnOffset`

#### UE에서 주로 만질 값

- `LoadoutComponent > LoadoutData > SkillConfig > OffensiveSkill > EffectClass`
- `SkillSummonPanel` 기반 BP 내부 설정값

## 탄환 / 빔

### `ABulletBase`

#### 역할

- 탄환 생명주기 관리
- 충돌/오버랩 처리
- 비관통/관통/폭발 공격 타입 처리
- 데미지 적용

### `ABeamEffectActor`

#### 역할

- 나이아가라 빔 이펙트 공통 부모
- 빔/히트 이펙트 컴포넌트 보관
- 빔 색상, 끝점, 크기 파라미터를 코드로 관리

#### 포함 컴포넌트

- `SceneRoot`
- `BeamComponent`
- `HitComponent`

#### 주요 설정값

- `MainColor`
- `BeamEnd`
- `BeamSize`
- `MainColorParameterName`
- `BeamEndParameterName`
- `BeamSizeParameterName`
- `bBeamEndUsesWorldSpace`
- `DefaultBeamSystem`
- `DefaultHitSystem`

#### 주요 함수

- `ApplyBeamParameters()`
- `ApplyBeamColor()`
- `ApplyBeamEnd()`
- `ApplyBeamSize()`
- `SetMainColor()`
- `SetBeamEnd()`
- `SetBeamSize()`
- `ActivateBeam()`
- `DeactivateBeam()`
- `ActivateHitEffect()`
- `DeactivateHitEffect()`

## 판넬 액터

### `AFunnel`

#### 역할

- 타겟 주변을 비행하는 판넬 액터
- `FireOrigin`에서 빔을 생성
- 일정 시간 동안 빔을 반복 발사
- 타겟이 죽거나 공격 시간이 끝나면 스스로 정리

#### 현재 동작 방식

- `TargetActor` 기준 공전
- `AttackActiveDuration` 동안 공격 상태 유지
- `BeamFireInterval` 간격으로 빔 반복 발사
- 각 빔은 `BeamDuration`만큼 유지
- 타겟이 무효가 되면 즉시 공격 종료 후 액터 삭제
- 공격 시간이 끝나면 액터 삭제

#### 주요 설정값

- `BeamActorClass`
- `AttackActiveDuration`
- `BeamFireInterval`
- `BeamDuration`
- `bAutoStartBeamAttackOnBeginPlay`
- `TargetActor`
- `FollowSpeed`
- `DesiredDistanceToTarget`
- `TargetOffset`
- `OrbitRadius`
- `OrbitHeight`
- `OrbitAngularSpeed`

#### 랜덤화 설정값

- `bRandomizeOrbitPerInstance`
- `OrbitRadiusRandomOffset`
- `OrbitHeightRandomOffset`
- `OrbitSpeedRandomOffset`
- `DistanceRandomOffset`

#### 주요 함수

- `StartBeamAttack()`
- `StopBeamAttack()`
- `SetBeamTarget()`

## 적/타겟 관리

### `UEnemyManager`

#### 역할

- 월드의 적 Pawn 목록 보관
- 적 등록
- 타겟 목록 제공

#### 주요 함수

- `AddEnemy()`
- `GetEnemys()`

## UE에서 어디를 만져야 하는가

### 플레이어 BP

- `SquadRuntimeComponent > LeftCraftClass / CenterCraftClass / RightCraftClass`
- 입력 테스트는 현재 `1 = 공격 스킬`, `2 = 버프 스킬`

### `BP_SquadCraft_*`

- `LoadoutComponent > LoadoutData > Identity`
- `LoadoutComponent > LoadoutData > AttackConfig`
- `LoadoutComponent > LoadoutData > SkillConfig`

### 판넬 관련 BP

- `Funnel` BP
  - `BeamActorClass`
  - `AttackActiveDuration`
  - `BeamFireInterval`
  - `BeamDuration`
  - 공전/랜덤화 값
- `BeamEffectActor` BP
  - `DefaultBeamSystem`
  - `DefaultHitSystem`
  - `MainColor`
  - `BeamSize`
  - `bBeamEndUsesWorldSpace`
- `SkillSummonPanel` BP
  - `FunnelClass`
  - `FunnelCount`
  - `FunnelSpawnRadius`
  - `FunnelSpawnOffset`

## 오늘 작업 흐름 요약

1. `BP_ky_beam_01` 자산 구조를 확인하고 빔/히트 나이아가라와 파라미터 흐름을 파악했다.
2. 빔 제어를 코드로 옮기기 위해 `ABeamEffectActor`를 추가했다.
3. 타겟 주변을 도는 판넬 액터 `AFunnel`을 만들고, `FireOrigin`에서 빔을 반복 발사하도록 연결했다.
4. 빔이 반대 방향으로 나가던 문제를 잡기 위해 `BeamEnd`를 기본적으로 로컬 좌표 기준으로 전달하도록 보정했다.
5. 여러 판넬이 동시에 소환될 때 같은 궤도를 돌지 않도록 공전 반경/높이/속도/방향 랜덤화를 추가했다.
6. 공격 스킬 효과 `USkillSummonPanel`을 만들어 판넬 여러 개를 소환하고 타겟을 직접 전달하거나 락온/최근접 적을 fallback으로 사용하게 했다.
7. 테스트용으로 `1` 키는 공격 스킬, `2` 키는 버프 스킬 발동으로 연결하고 메인 기체만 사용 가능하게 제한했다.
8. `BP_FireMegaBeam`를 `SkillSummonPanel`로 리네임하고 리다이렉트를 추가했다.
9. 빈 로드아웃 스킬 값이 `SkillComponent`를 덮어쓰지 않도록 보강했다.
10. 공격 종료 또는 타겟 소멸 시 판넬이 자동으로 삭제되게 정리했다.
11. 로드아웃에서 다뤄야 하는 값은 실행 컴포넌트 에디터에서 직접 수정하지 않도록 노출을 줄였다.
