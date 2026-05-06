# 남은 구현 슬라이스 정리

## 목적

이 문서는 현재 프로젝트 코드 기준으로, 앞으로 남은 기능 작업을 다시 정리한 기록이다.

초기 기획 문서만 기준으로 보지 않고, 지금 이미 들어와 있는 시스템까지 반영해서 다음 구현 순서를 빠르게 잡는 데 목적이 있다.

## 현재까지 구현된 큰 축

### 1. 플레이어 이동과 편대 비행

- `AP_Player`
- `UPathFollowerComponent`
- `URailOffsetComponent`
- `USquadComponent`
- `USquadRuntimeComponent`
- `UPlayerCameraRigComponent`
- `UPlayerRailMovementComponent`

현재 상태:

- 스플라인 기반 자동 전진
- X/Y 회피 이동
- 편대 스폰과 슬롯 스왑
- 편대 뱅킹 연출
- 카메라와 레일 이동 책임 분리

### 2. 공격과 로드아웃

- `UPlayerAimFireComponent`
- `USupportFireComponent`
- `UCraftAttackComponent`
- `UCraftLoadoutComponent`
- `ASquadCraftActor`
- `ABulletBase`

현재 상태:

- 메인 기체 수동 사격
- 서브 기체 자동 사격
- `Single`, `Burst`, `Spread`
- `비관통`, `관통`, `폭발`
- 기체별 로드아웃 기반 공격 설정

### 3. 스킬 기반

- `USkillComponent`
- `USkillEffectBase`
- `USkillSummonPanel`
- `USkillSpawnShield`
- `USkillEffect_MultiHomingMissile`
- `AFunnel`
- `ABeamEffectActor`

현재 상태:

- 공격 스킬과 버프 스킬 골격 있음
- 판넬 소환형 공격 스킬 있음
- 쉴드 생성형 버프 있음
- 다중 유도 미사일 스킬 있음
- 빔 연출 부모 액터와 판넬 공전 로직 있음

### 4. 보스와 패턴 기반

- `ABossCore`
- `UBossPhaseComponent`
- `ASnake_CompositeMaster`
- `USnakePathController`
- `USnakeSkillManager`
- `USnakeBodyChargeComponent`
- `ULaserAttackComponent`
- `ALaserCannon`
- `ABeamObjcet`
- `ABeamSpawner`

현재 상태:

- 보스 코어가 스네이크 보스 소환
- 스네이크 공전 이동
- 바디 차지 패턴
- 레이저 패턴
- 보스 HP 기반 페이즈 컴포넌트 추가 완료

### 5. UI와 테스트용 보조 시스템

- `AHUDManager`
- `UTargetButtonWidget`

현재 상태:

- 타겟 버튼 미니게임 구조 있음
- 현재는 테스트용 진입점으로 유지
- 추후 보스 패턴용으로 재사용 예정

## 결정된 전제

- 약점 시스템은 현재 범위에서 사용하지 않는다.
- `WeakPointTarget` 기반 확장은 지금 우선순위에서 제외한다.
- UI 작업은 기능 축을 더 만든 뒤 나중에 정리한다.
- 버프 스킬은 수동 입력형이 아니라 자동 발동형으로 유지한다.

## 남은 슬라이스

## 슬라이스 1. 플레이어 스킬 입력과 자동 버프 규칙 정리

### 현재 상태

- `1번 키`는 액티브 스킬 발동
- 버프 스킬은 자동 발동
- `2번 키` 입력은 메인 스킬 루프에서 분리됨

### 남은 작업

1. 버프 자동 발동 간격과 범위를 실제 의도에 맞게 확정
2. 자동 버프를 모든 기체가 쓰는지, 특정 기체만 쓰는지 정리
3. 테스트용 스킬 발동 흔적이 더 남아 있으면 제거

---

## 슬라이스 2. 코드 정리와 경계 보강

### 목표

기능을 더 붙이기 전에 테스트 코드와 실제 기능 코드의 경계를 최소한으로 정리한다.

### 작업 방향

1. 플레이어 입력과 테스트 진입점 분리
2. 보스 패턴과 무관한 임시 호출 최소화
3. 빔/레이저 계열에서 연출용과 공격용 역할 구분 보강

---

## 슬라이스 3. 스네이크 보스 레이드 규칙 확장

### 현재 상태

- `BossCore`에 `UBossPhaseComponent`가 붙어 있음
- 페이즈에 따라 스네이크 이동/스킬 설정이 달라지기 시작함

### 남은 작업

1. 차지 패턴과 레이저 패턴의 시작/종료 규칙을 더 명확히 정리
2. 페이즈별 빈도, 준비 시간, 공격 지속 시간을 더 강하게 차등화
3. 보스 전투 상태를 `Idle`, `Preparing`, `Executing`, `Recovering`처럼 더 명시적으로 분리할지 결정
4. 패턴 종료 후 다음 패턴까지의 회복 구간을 넣을지 검토

---

## 슬라이스 4. 레이저/빔 계열 역할 정리

### 현재 문제

- `ABeamEffectActor`
- `ULaserAttackComponent`
- `ALaserCannon`
- `ABeamObjcet`
- `ABeamSpawner`

가 공존해서 역할 경계가 완전히 명확하진 않다.

### 목표

- 어떤 클래스가 연출용인지
- 어떤 클래스가 공격 실행기인지
- 어떤 클래스가 맵/보스 패턴 배치용인지

를 분명하게 만든다.

### 추천 작업

1. `ABeamEffectActor`는 연출 부모로 유지
2. `ULaserAttackComponent`는 지속형/추적형 공격 실행기로 유지
3. `ALaserCannon`은 사용자 액터로 유지
4. `ABeamObjcet`, `ABeamSpawner`는 고정 레이저 배치 계열로 명확히 정리

---

## 슬라이스 5. 스킬 장착과 발동 흐름 마감

### 목표

- `LoadoutData -> SkillComponent -> 실제 발동` 흐름을 안정화

### 추천 작업

1. 스킬은 `LoadoutData.SkillConfig` 기준으로만 authoring
2. `SkillComponent`는 런타임 상태 표시와 발동만 담당
3. 스킬별 쿨다운, 유지 시간, 실패 조건 정리
4. 판넬/쉴드/미사일 스킬을 실제 기체 개성과 연결

---

## 슬라이스 6. UI 정리

### 현재 전제

지금은 기능을 먼저 만들고, UI는 나중에 정리한다.

### 나중에 할 작업

1. 락온 UI
2. 보스 패턴 표시
3. 스킬 상태 표시
4. 미니게임 UI와 전투 UI 분리

---

## 슬라이스 7. 레일 구간과 맵 기믹

### 아직 거의 남아 있는 영역

1. 추격 구간
2. 회피 구간
3. 결전 구간
4. 속도 변화
5. 장애물과 압박 기믹

## 바로 다음 추천 순서

1. 슬라이스 1 남은 자동 버프 규칙 정리
2. 슬라이스 3 스네이크 보스 레이드 규칙 확장
3. 슬라이스 4 레이저/빔 계열 역할 정리
4. 슬라이스 5 스킬 장착 흐름 마감

## 메모

- 약점 시스템은 현재 우선순위에서 제외
- 미니게임은 추후 보스 패턴용으로 재사용 예정
- UI는 기능 완성 후 정리
