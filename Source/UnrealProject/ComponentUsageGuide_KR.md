# 컴포넌트 역할 및 사용 가이드

## 목적

이 문서는 현재 프로젝트에서 사용하는 주요 컴포넌트들의 역할, 소유 주체, 연결 방식, 실제 사용법을 빠르게 확인하기 위한 참조 문서입니다.

구조 리팩토링 배경과 기술부채는 `CodebaseRefactorGuide_KR.md`를 참고하고,
이 문서는 실제 작업 시 "어디를 수정해야 하는가"에 집중합니다.

## 전체 구조 한눈에 보기

현재 구조는 크게 다음 계층으로 나뉩니다.

- `AP_Player`
  - 입력 수신과 상위 조정자 역할
- 플레이어 루트 컴포넌트
  - 락온, 레일 이동, 카메라, 편대 런타임, 발사 판단
- `ASquadCraftActor`
  - 개별 기체 단위
- 기체 소유 컴포넌트
  - 공격 실행, 로드아웃, 스킬
- `ABulletBase`
  - 실제 탄환 판정과 결과 처리

즉 현재 프로젝트는 "플레이어 루트가 모든 기능을 직접 처리하는 구조"가 아니라,
기능별 컴포넌트를 붙이고 플레이어는 상위에서 호출과 연결만 담당하는 구조를 목표로 하고 있습니다.

## 플레이어 루트: `AP_Player`

### 역할

- 입력 바인딩
- 각 컴포넌트 호출 순서 조정
- 플레이 흐름 상위 조정

### 직접 처리하는 것

- 발사 입력 호출
- 편대 교체 입력 호출
- Tick에서 이동/카메라/자동사격 흐름 호출

### 직접 처리하지 않는 것

- 락온 탐색
- 카메라 팬/앵커 이동
- 편대 스폰/보관
- 레일 이동 최종 합성
- 메인 조준 계산
- 서브 자동사격 타겟 판단

### 수정할 때 기준

- 입력 자체를 바꿀 때는 `AP_Player`
- 입력 이후 실제 계산을 바꿀 때는 각 컴포넌트

## 플레이어 소유 컴포넌트

### `ULockOnComponent`

#### 역할

- 락온 on/off 상태 관리
- 현재 타겟 유지
- 가장 가까운 적 선택
- 다음 타겟 순환

#### 주 사용 위치

- `AP_Player` 입력 바인딩
- `UPlayerAimFireComponent`
- `USupportFireComponent`

#### 외부에서 사용하는 방식

- `TraceTarget()`
- `ChangeTarget()`
- `IsLockOnEnabled()`
- `GetCurrentTarget()`

#### 언제 수정해야 하는가

- 락온 규칙을 바꿀 때
- 타겟 변경 우선순위를 바꿀 때
- 보스 약점/저지 마커를 락온 대상으로 확장할 때

#### 주의점

- 다른 클래스에서 내부 상태 필드에 직접 접근하지 말고 getter를 통해 읽는 구조를 유지하는 것이 좋습니다.

### `UPathFollowerComponent`

#### 역할

- 스플라인을 따라 자동 전진
- 경로 기반 기본 위치/회전 계산
- 가속값 반영

#### 주 사용 위치

- `AP_Player`
- `UPlayerRailMovementComponent`

#### 외부에서 사용하는 방식

- `SetTargetPath()`
- `SetAcceleration()`
- `HasValidBaseTransform()`
- `GetBaseWorldTransform()`

#### 언제 수정해야 하는가

- 스플라인 이동 방식이 바뀔 때
- 가속/감속 규칙을 바꿀 때
- 레일 자체 회전 보정이나 banking 규칙을 바꿀 때

#### 주의점

- 이 컴포넌트는 "레일 기준 기본 이동"까지만 담당합니다.
- 실제 플레이어 최종 위치 조합은 `UPlayerRailMovementComponent`가 담당합니다.

### `URailOffsetComponent`

#### 역할

- 플레이어 입력 기반 XY 이격 이동 계산
- 입력이 없을 때 원위치 복귀 보간
- 기체 시각 기울기 계산

#### 주 사용 위치

- `AP_Player`
- `UPlayerRailMovementComponent`

#### 외부에서 사용하는 방식

- `SetHorizontalInput()`
- `SetVerticalInput()`
- `UpdateOffset()`
- `GetCurrentOffset()`
- `GetVisualTiltRotation()`

#### 언제 수정해야 하는가

- 회피 범위와 반응 속도를 조정할 때
- 기울기 연출을 바꿀 때
- X/Y 조작 감각을 바꿀 때

### `USquadComponent`

#### 역할

- 활성 슬롯 관리
- 슬롯 오프셋 계산

#### 주 사용 위치

- `AP_Player`
- `USquadRuntimeComponent`

#### 외부에서 사용하는 방식

- `ShiftActiveSlotLeft()`
- `ShiftActiveSlotRight()`
- `GetActiveSlot()`
- `GetSlotOffset()`
- `GetAllSlots()`

#### 언제 수정해야 하는가

- 편대 슬롯 구조가 바뀔 때
- 편대 형태가 바뀔 때
- V자 배치, 거리, 높이 차이를 바꿀 때

### `USquadRuntimeComponent`

#### 역할

- 좌/중앙/우 기체 스폰
- 각 슬롯별 기체 참조 보관
- 슬롯 상태에 따라 기체 활성 상태 갱신
- 활성 기체의 시각 회전 반영

#### 주 사용 위치

- `AP_Player`

#### 외부에서 사용하는 방식

- `SpawnCrafts()`
- `RefreshCraftStates()`
- `ApplyActiveCraftVisualRotation()`
- `GetActiveCraft()`
- `GetAllCrafts()`

#### 언제 수정해야 하는가

- 편대 스폰 규칙을 바꿀 때
- 슬롯별 다른 BP를 연결할 때
- 편대 액터의 생성/보관 방식을 바꿀 때

#### UE에서 주로 만지는 값

- `LeftCraftClass`
- `CenterCraftClass`
- `RightCraftClass`

즉 어떤 기체 BP를 좌/중앙/우에 쓸지는 이 컴포넌트에서 정합니다.

### `UPlayerAimFireComponent`

#### 역할

- 메인 기체 발사 조준 판단
- 록온 중이면 록온 타겟 기준 발사
- 아니면 마우스 방향 발사

#### 주 사용 위치

- `AP_Player::Fire()`

#### 외부에서 사용하는 방식

- `FireActiveCraft()`

#### 언제 수정해야 하는가

- 메인 기체 조준 규칙이 바뀔 때
- 마우스 발사 방식이 바뀔 때
- 락온 조준 보정이 필요할 때

### `USupportFireComponent`

#### 역할

- 비활성 기체 자동사격 판단
- `CombatRole`에 따른 타겟 선택

#### 주 사용 위치

- `AP_Player::HandleSupportAutoFire()`

#### 외부에서 사용하는 방식

- `HandleSupportAutoFire()`

#### 현재 역할별 동작

- `MainGun`
  - 자동사격하지 않음
- `SupportRapid`
  - 가까운 적 우선
- `SupportHeavy`
  - 가능하면 락온 타겟 우선

#### 언제 수정해야 하는가

- 서브 기체 AI 사격 성향을 바꿀 때
- 타겟 우선순위를 확장할 때
- 보스 약점, 마커, 저지 대상 우선 규칙을 붙일 때

### `UPlayerCameraRigComponent`

#### 역할

- 마우스 기반 카메라 팬
- 활성 기체 기준 카메라 앵커 추적

#### 주 사용 위치

- `AP_Player::Tick()`
- 편대 스왑 직후
- BeginPlay 초기화

#### 외부에서 사용하는 방식

- `UpdateCameraPan()`
- `UpdateCameraAnchor()`

#### 언제 수정해야 하는가

- 카메라 추적 감도를 조정할 때
- 팬 회전 범위를 바꿀 때
- 활성 기체 추적 보간을 바꿀 때

### `UPlayerRailMovementComponent`

#### 역할

- `UPathFollowerComponent`와 `URailOffsetComponent` 결과 조합
- 플레이어 최종 위치/회전 반영
- 활성 편대 기체 시각 기울기 반영

#### 주 사용 위치

- `AP_Player::Tick()`

#### 외부에서 사용하는 방식

- `ApplyRailMovement()`

#### 언제 수정해야 하는가

- 레일 기준 이동과 XY 이격 조합 규칙을 바꿀 때
- 활성 기체 기울기 적용 방식을 바꿀 때

## 기체 액터: `ASquadCraftActor`

### 역할

- 개별 기체 표현 단위
- 메쉬/발사 오리진 보유
- 자기 로드아웃과 공격/스킬 컴포넌트 소유

### 직접 소유하는 컴포넌트

- `UCraftAttackComponent`
- `UCraftLoadoutComponent`
- `USkillComponent`

### 외부에서 주로 사용하는 방식

- `FireAt()`
- `TryAutoFireAt()`
- `SetActiveCraft()`
- `SetDesiredRelativeTransform()`
- `GetCombatRole()`
- `GetCraftMesh()`

### 언제 수정해야 하는가

- 기체 비주얼 구조를 바꿀 때
- 기체별 개성을 더 강하게 넣을 때
- 피격 반응, 엔진 연출, 스킬 이펙트를 붙일 때

## 기체 소유 컴포넌트

### `UCraftLoadoutComponent`

#### 역할

- 기체 설정의 단일 편집 창구
- 공격/스킬 컴포넌트에 설정 전달

#### 현재 구조

- `Identity`
  - `LoadoutId`
  - `DisplayName`
  - `CombatRole`
- `AttackConfig`
  - 공격 패턴/탄환 관련 설정
- `SkillConfig`
  - 버프/공격 스킬 설정
- `PresentationConfig`
  - 표현 관련 식별 정보

#### 외부에서 사용하는 방식

- `ApplyLoadoutToAttackComponent()`
- `ApplyLoadoutToSkillComponent()`
- `GetLoadoutData()`
- `GetCombatRole()`

#### UE에서 주로 만지는 값

- `BP_SquadCraft_* > LoadoutComponent > LoadoutData`

#### 언제 수정해야 하는가

- 기체별 공격/스킬/역할 데이터를 바꿀 때
- BP에서 설정 위치를 정리할 때

#### 주의점

- 공격 수치를 `AttackComponent`에 직접 넣는 구조로 되돌아가지 않는 것이 좋습니다.
- 설정은 `LoadoutComponent`, 실행은 `AttackComponent`/`SkillComponent`가 맡습니다.

### `UCraftAttackComponent`

#### 역할

- 실제 탄환 발사 실행
- 단발/버스트/스프레드 처리
- 자동사격 쿨다운 유지

#### 외부에서 사용하는 방식

- `ApplyAttackConfig()`
- `FireFromOrigin()`
- `TryAutoFireFromOrigin()`

#### 현재 입력 데이터

- `FCraftAttackConfig`

#### 언제 수정해야 하는가

- 공격 패턴을 추가할 때
- 탄환 생성 방식을 바꿀 때
- 자동사격 간격 규칙을 바꿀 때

#### 주의점

- 이 컴포넌트는 "실행기"입니다.
- 어떤 설정을 쓸지는 `LoadoutComponent`가 결정합니다.

### `USkillComponent`

#### 역할

- 스킬 슬롯 보관
- 스킬 쿨다운 관리
- 실제 스킬 발동 진입점 제공

#### 외부에서 사용하는 방식

- `SetBuffSkill()`
- `SetOffensiveSkill()`
- `TryActivateBuffSkill()`
- `TryActivateOffensiveSkill()`

#### 언제 수정해야 하는가

- 스킬 슬롯 구조를 바꿀 때
- 스킬 발동 규칙을 바꿀 때
- 자동 발동/입력 발동을 나눌 때

## 스킬 관련 타입

### `USkillEffectBase`

#### 역할

- 실제 스킬 효과 실행 추상 베이스

#### 파생 구조

- `UBuffSkillEffect`
- `UOffensiveSkillEffect`

#### 외부에서 사용하는 방식

- `ActivateSkill()`
- `GetSkillType()`

#### 언제 수정해야 하는가

- 실제 버프/공격 스킬을 구현할 때
- 공통 스킬 컨텍스트를 확장할 때

### `FSkillSpec`

#### 역할

- 스킬 데이터 정의

#### 포함 값

- `SkillId`
- `DisplayName`
- `SkillType`
- `Cooldown`
- `Duration`
- `EffectClass`

#### 언제 수정해야 하는가

- 스킬 데이터 필드를 확장할 때
- `Range`, `Cost`, `TargetPolicy` 같은 필드가 필요할 때

## 탄환과 전투 결과

### `ABulletBase`

#### 역할

- 탄환 수명 관리
- 충돌/오버랩 진입 처리
- 데미지 적용
- 비관통/관통/폭발 처리
- 히트 이펙트 재생

#### 외부에서 사용하는 방식

- `ConfigureAttackType()`
- `SetTarget()`
- `GetTarget()`

#### 현재 공격 타입

- `NonPiercing`
- `Piercing`
- `Explosive`

#### 언제 수정해야 하는가

- 탄환 판정 규칙이 바뀔 때
- 폭발/관통 규칙을 확장할 때
- 유도탄/특수탄을 더 늘릴 때

### `UHomingMovementComponent`

#### 역할

- 선택적으로 유도 회전 보간
- 직진 이동 보조

#### 현재 위치

- 기본 탄환 필수 구조가 아니라 옵션형 보조 이동 컴포넌트

#### 언제 수정해야 하는가

- 특수 유도탄을 다시 적극적으로 사용할 때
- 유도 속도와 보간 규칙을 바꿀 때

## 월드/타겟 관리

### `UEnemyManager`

#### 역할

- 월드의 적 Pawn 목록 보관
- 적 추가
- 적 파괴 시 목록 정리

#### 외부에서 사용하는 방식

- `AddEnemy()`
- `GetEnemys()`

#### 주 사용 위치

- `ULockOnComponent`
- `USupportFireComponent`

#### 언제 수정해야 하는가

- 적 관리 기준이 Pawn 단위에서 바뀔 때
- 보스 약점/마커를 별도 타겟 풀로 분리할 때

## UE에서 주로 어디를 만져야 하는가

### 플레이어 BP

- 입력 바인딩 확인
- 카메라/스프링암 기본 값 확인
- 루트에 붙은 컴포넌트 구성 확인

### `BP_SquadCraft_*`

- `LoadoutComponent > LoadoutData > Identity`
- `LoadoutComponent > LoadoutData > AttackConfig`
- `LoadoutComponent > LoadoutData > SkillConfig`
- 필요 시 메쉬/파이어 오리진 확인

### 총알 BP

- `BP_Bullet`
- 충돌 프리셋
- 나이아가라 비주얼
- 히트 이펙트

## 수정 위치 빠른 가이드

- 락온이 이상하다
  - `ULockOnComponent`
- 메인 기체 발사가 이상하다
  - `UPlayerAimFireComponent`
- 서브 자동사격이 이상하다
  - `USupportFireComponent`
- 레일 이동이 이상하다
  - `UPathFollowerComponent`
  - `URailOffsetComponent`
  - `UPlayerRailMovementComponent`
- 카메라가 이상하다
  - `UPlayerCameraRigComponent`
- 편대 생성/활성 상태가 이상하다
  - `USquadComponent`
  - `USquadRuntimeComponent`
- 기체별 공격 설정이 이상하다
  - `UCraftLoadoutComponent > AttackConfig`
- 발사 자체가 이상하다
  - `UCraftAttackComponent`
- 탄환 판정이 이상하다
  - `ABulletBase`
- 스킬 슬롯/쿨타임이 이상하다
  - `USkillComponent`
- 실제 스킬 효과가 이상하다
  - `USkillEffectBase` 파생 클래스

## 현재 기준 추천 원칙

- 설정은 `LoadoutComponent`에 둡니다.
- 실행은 기능별 실행 컴포넌트가 맡습니다.
- `AP_Player`는 상위 호출과 연결만 담당하게 유지합니다.
- 기체별 개성은 `ASquadCraftActor`가 아니라 로드아웃 데이터 중심으로 키워가는 편이 좋습니다.
- 형제 컴포넌트끼리 직접 강하게 물기보다, 오너 액터를 통해 연결하는 현재 방향을 유지하는 것이 좋습니다.
