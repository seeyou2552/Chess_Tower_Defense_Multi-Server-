//#include "Game/Skill/Projectile.h"
//#include "Game/Enemy/Enemy.h"
//#include "Game/Minion/Minion.h"
//
//#include "Core/Logger.h"
//
//Projectile::Projectile()
//{
//}
//
//void Projectile::Init(
//    uint32_t instanceId,
//    std::shared_ptr<Enemy>& target,
//    Vector3 startPos,
//    std::shared_ptr<Minion>& minion,
//    int damage,
//    float speed,
//    float activeDuration,
//    bool isChase,
//    ProjectileHitType hitType
//)
//{
//    m_instanceId = instanceId;
//    m_target = target;
//    m_ownerMinion = minion;
//    m_position = startPos;
//
//    m_damage = damage;
//    m_speed = speed;
//    m_activeDuration = activeDuration;
//    m_isChase = isChase;
//    m_active = true;
//    m_elapsedTime = 0.0f;
//    m_direction = { 0.0f, 0.0f, 0.0f };
//
//    m_hitType = hitType;
//    m_hitedEnemies.clear();
//
//    if (m_target)
//    {
//        Vector3 toTarget =
//        {
//            m_target->GetPosition().x - m_position.x,
//            m_target->GetPosition().y - m_position.y,
//            m_target->GetPosition().z - m_position.z
//        };
//
//        const float length = GetLength(toTarget);
//        if (length > 0.0001f)
//        {
//            m_direction =
//            {
//                toTarget.x / length,
//                toTarget.y / length,
//                toTarget.z / length
//            };
//        }
//    }
//}
//
//void Projectile::Reset()
//{
//    m_instanceId = 0;
//    m_position = { 0.0f, 0.0f, 0.0f };
//    m_damage = 0;
//    m_speed = 0.0f;
//    m_activeDuration = 0.0f;
//
//    m_elapsedTime = 0.0f;
//    m_isChase = false;
//    m_active = false;
//    m_target.reset();
//    m_ownerMinion.reset();
//    m_direction = { 0.0f, 0.0f, 0.0f };
//}
//
//void Projectile::Update(
//    float deltaTime,
//    std::vector<std::shared_ptr<Enemy>> enemies
//)
//{
//    if (!m_active)
//        return;
//
//    Movement(deltaTime, enemies);
//}
//
//void Projectile::Movement(
//    float deltaTime,
//    std::vector<std::shared_ptr<Enemy>> enemies
//)
//{
//    // 추적
//    if (m_isChase)
//    {
//       Chase(deltaTime, enemies);
//    }
//
//    // 직선
//    else
//    {
//        Direct(deltaTime, enemies);
//    }
//}
//
//void Projectile::Chase(
//    float deltaTime,
//    std::vector<std::shared_ptr<Enemy>> enemies
//)
//{
//    if (!m_target || !m_target->IsActive())
//    {
//        m_active = false;
//
//        if (m_onReturn)
//        {
//            Reset();
//            m_onReturn(m_instanceId);
//        }
//
//        return;
//    }
//
//    // 기존 타겟 대상 로직
//    const Vector3 targetPos = m_target->GetPosition();
//    Vector3 toTarget =
//    {
//        targetPos.x - m_position.x,
//        targetPos.y - m_position.y,
//        targetPos.z - m_position.z
//    };
//
//    const float distance = GetLength(toTarget);
//    if (distance <= 0.01f)
//    {
//        m_position = targetPos;
//        OnHit(m_target);
//        Logger::GetInstance().Info("Chase End");
//        return;
//    }
//
//    // 근접한 적 대상 로직
//    for (std::shared_ptr<Enemy> enemy : enemies)
//    {
//        OnHit(enemy);
//
//        if (!m_active)
//            return;
//    }
//
//    const float moveDistance = m_speed * deltaTime;
//    if (moveDistance >= distance)
//    {
//        m_position = targetPos;
//        return;
//    }
//
//    // Chase 이동 로직
//    m_direction =
//    {
//        toTarget.x / distance,
//        toTarget.y / distance,
//        toTarget.z / distance
//    };
//
//    m_position.x += m_direction.x * moveDistance;
//    m_position.y += m_direction.y * moveDistance;
//    m_position.z += m_direction.z * moveDistance;
//}
//
//void Projectile::Direct(
//    float deltaTime,
//    std::vector<std::shared_ptr<Enemy>> enemies
//)
//{
//    m_elapsedTime += deltaTime;
//    if (m_elapsedTime >= m_activeDuration)
//    {
//        if (m_onReturn)
//        {
//            Reset();
//            m_onReturn(m_instanceId);
//        }
//
//        return;
//    }
//
//    // 근접한 적 대상 로직
//    for (std::shared_ptr<Enemy> enemy : enemies)
//    {
//        OnHit(enemy);
//
//        if (!m_active)
//            return;
//    }
//    
//
//    const float moveDistance = m_speed * deltaTime;
//    m_position.x += m_direction.x * moveDistance;
//    m_position.y += m_direction.y * moveDistance;
//    m_position.z += m_direction.z * moveDistance;
//}
//
//void Projectile::OnHit(std::shared_ptr<Enemy> enemy)
//{
//    // 이미 피격된 적이 있는 경우
//    if (m_hitedEnemies.contains(enemy->GetInstanceId()))
//    {
//        Logger::GetInstance().Info("Piggggggggggg");
//        return;
//    }
//
//    else
//        m_hitedEnemies.emplace(
//            enemy->GetInstanceId(),
//            enemy->shared_from_this()
//        );
//    
//
//    switch (m_hitType)
//    {
//        case ProjectileHitType::Impact:
//
//            // Chase 상태인 경우
//            if (m_isChase)
//            {
//                // 기존 Target 대상과 다르면 return
//                if (m_target->GetInstanceId() != enemy->GetInstanceId())
//                    return;
//            }
//
//            if (m_onAttack)
//                m_onAttack(
//                    enemy,
//                    m_ownerMinion,
//                    m_damage
//                );
//
//            if (m_onReturn)
//            {
//                Reset();
//                m_onReturn(m_instanceId);
//            }
//            break;
//
//        case ProjectileHitType::Pierce:
//            if (m_onAttack)
//                m_onAttack(
//                    enemy,
//                    m_ownerMinion,
//                    m_damage
//                );
//
//            // Chase 상태인 경우
//            if (m_isChase)
//            {
//                // 기존 Target 대상이면 OnReturn 수행
//                if (m_target->GetInstanceId() == enemy->GetInstanceId())
//                {
//                    if (m_onReturn)
//                    {
//                        Reset();
//                        m_onReturn(m_instanceId);
//                    }
//                }
//            }
//            break;
//    }
//}
#include "Game/Skill/Projectile.h"

#include "Game/Enemy/Enemy.h"
#include "Game/Enemy/EnemyManager.h"
#include "Game/Minion/Minion.h"

#include "Core/Logger.h"

#include <algorithm>
#include <cmath>

Projectile::Projectile()
{
}

void Projectile::Init(
    uint32_t instanceId,
    std::shared_ptr<Enemy>& target,
    Vector3 startPos,
    std::shared_ptr<Minion>& minion,
    int damage,
    float speed,
    float activeDuration,
    bool isChase,
    ProjectileHitType hitType,
    EnemyManager& enemyManager
)
{
    /*
     * Room이 소유한 EnemyManager의 주소만 저장한다.
     *
     * Projectile이 EnemyManager의 생명주기를
     * 관리하지 않는다.
     */
    m_enemyManager = &enemyManager;

    m_instanceId = instanceId;

    m_target = target;
    m_ownerMinion = minion;

    m_position = startPos;
    m_previousPosition = startPos;

    m_damage = damage;

    m_speed = speed;
    m_activeDuration = activeDuration;
    m_elapsedTime = 0.0f;

    m_isChase = isChase;
    m_active = true;

    m_hitType = hitType;

    m_direction =
    {
        0.0f,
        0.0f,
        0.0f
    };

    m_hitedEnemies.clear();

    /*
     * Target이 없는 경우
     *
     * Direct Projectile은 Target이 필요하지 않을 수 있으므로
     * 여기서는 바로 종료하지 않는다.
     */
    if (!m_target)
        return;

    /*
     * Target을 향한 초기 방향 계산
     */
    const Vector3 targetPos =
        m_target->GetPosition();

    Vector3 toTarget =
    {
        targetPos.x - m_position.x,
        targetPos.y - m_position.y,
        targetPos.z - m_position.z
    };

    const float length = std::sqrt(
        toTarget.x * toTarget.x +
        toTarget.y * toTarget.y +
        toTarget.z * toTarget.z
    );

    if (length > 0.0001f)
    {
        m_direction =
        {
            toTarget.x / length,
            toTarget.y / length,
            toTarget.z / length
        };
    }
}

void Projectile::Reset()
{
    m_instanceId = 0;

    m_position =
    {
        0.0f,
        0.0f,
        0.0f
    };

    m_previousPosition =
    {
        0.0f,
        0.0f,
        0.0f
    };

    m_direction =
    {
        0.0f,
        0.0f,
        0.0f
    };

    m_damage = 0;

    m_speed = 0.0f;
    m_activeDuration = 0.0f;
    m_elapsedTime = 0.0f;

    m_isChase = false;
    m_active = false;

    m_target.reset();
    m_ownerMinion.reset();

    m_hitedEnemies.clear();

    /*
     * EnemyManager는 Room이 소유한다.
     *
     * Projectile이 Reset된다고 해서
     * EnemyManager 자체가 사라지는 것이 아니므로
     * nullptr로 초기화해도 된다.
     *
     * 다음 Init()에서 다시 설정된다.
     */
    m_enemyManager = nullptr;
}

void Projectile::Update(float deltaTime)
{
    if (!m_active)
        return;

    /*
     * EnemyManager가 설정되지 않았다면
     * 충돌 검사를 할 수 없다.
     */
    if (!m_enemyManager)
    {
        ReturnToPool();
        return;
    }

    Movement(deltaTime);
}

void Projectile::Movement(float deltaTime)
{
    if (m_isChase)
    {
        Chase(deltaTime);
    }
    else
    {
        Direct(deltaTime);
    }
}

void Projectile::Chase(float deltaTime)
{
    /*
     * Target이 사라졌거나 비활성화된 경우
     * Projectile도 종료한다.
     */
    if (!m_target || !m_target->IsActive())
    {
        ReturnToPool();
        return;
    }

    /*
     * 이번 프레임의 시작 위치를 저장한다.
     *
     * 이후
     *
     * previousPosition -> currentPosition
     *
     * 전체 구간을 충돌 검사한다.
     */
    m_previousPosition = m_position;

    const Vector3 targetPos =
        m_target->GetPosition();

    Vector3 toTarget =
    {
        targetPos.x - m_position.x,
        targetPos.y - m_position.y,
        targetPos.z - m_position.z
    };

    const float distance = std::sqrt(
        toTarget.x * toTarget.x +
        toTarget.y * toTarget.y +
        toTarget.z * toTarget.z
    );

    /*
     * Target과 거의 겹쳐있는 경우
     */
    if (distance <= 0.0001f)
    {
        m_position = targetPos;

        CheckCollision(
            m_previousPosition,
            m_position
        );

        return;
    }

    const float moveDistance =
        m_speed * deltaTime;

    /*
     * 이번 프레임에 Target까지 도착할 수 있는 경우
     */
    if (moveDistance >= distance)
    {
        m_position = targetPos;
    }
    else
    {
        /*
         * Target을 향하는 방향을 다시 계산한다.
         *
         * Chase이므로 매 프레임 Target의
         * 현재 위치를 기준으로 방향을 갱신한다.
         */
        m_direction =
        {
            toTarget.x / distance,
            toTarget.y / distance,
            toTarget.z / distance
        };

        m_position.x +=
            m_direction.x * moveDistance;

        m_position.y +=
            m_direction.y * moveDistance;

        m_position.z +=
            m_direction.z * moveDistance;
    }

    /*
     * 이동하기 전 위치부터
     * 이동한 위치까지 전체 경로를 검사한다.
     *
     * 이렇게 해야 Projectile이 빠르게 움직일 때
     * Enemy를 뚫고 지나가는 현상(Tunneling)을 방지할 수 있다.
     */
    CheckCollision(
        m_previousPosition,
        m_position
    );
}

void Projectile::Direct(float deltaTime)
{
    m_elapsedTime += deltaTime;

    /*
     * Projectile의 최대 유지시간 초과
     */
    if (m_elapsedTime >= m_activeDuration)
    {
        ReturnToPool();
        return;
    }

    /*
     * 이번 프레임 시작 위치
     */
    m_previousPosition = m_position;

    const float moveDistance =
        m_speed * deltaTime;

    /*
     * 직선 방향으로 이동
     */
    m_position.x +=
        m_direction.x * moveDistance;

    m_position.y +=
        m_direction.y * moveDistance;

    m_position.z +=
        m_direction.z * moveDistance;

    /*
     * 이전 위치 -> 현재 위치 전체를 검사
     */
    CheckCollision(
        m_previousPosition,
        m_position
    );
}

void Projectile::CheckCollision(
    const Vector3& previousPosition,
    const Vector3& currentPosition
)
{
    if (!m_enemyManager)
        return;

    /*
     * 현재 활성화된 Enemy 목록을 가져온다.
     *
     * 이건 FindEnemyInRange()가 아니다.
     *
     * FindEnemyInRange()
     *  -> Minion의 공격 대상 선정
     *
     * GetActiveEnemiesSnapshot()
     *  -> Projectile 충돌 검사 대상
     */
    auto enemies =
        m_enemyManager->GetActiveEnemiesSnapshot();

    /*
     * unordered_map 순서에 의존하지 않도록
     * InstanceId 기준으로 정렬한다.
     *
     * 동일 프레임에 여러 Enemy와 충돌할 경우에도
     * 처리 순서를 일정하게 유지할 수 있다.
     */
    std::sort(
        enemies.begin(),
        enemies.end(),
        [](const std::shared_ptr<Enemy>& lhs,
            const std::shared_ptr<Enemy>& rhs)
        {
            if (!lhs)
                return false;

            if (!rhs)
                return true;

            return lhs->GetInstanceId()
                < rhs->GetInstanceId();
        }
    );

    for (auto& enemy : enemies)
    {
        if (!enemy)
            continue;

        if (!enemy->IsActive())
            continue;

        /*
         * 이미 이 Projectile에 맞은 Enemy는
         * 다시 Hit하지 않는다.
         */
        if (m_hitedEnemies.contains(
            enemy->GetInstanceId()))
        {
            continue;
        }

        /*
         * 실제 이동 경로와 Enemy Collider가
         * 충돌했는지 검사한다.
         */
        if (!CheckEnemyCollision(
            previousPosition,
            currentPosition,
            enemy))
        {
            continue;
        }

        /*
         * 실제 Hit 처리
         */
        OnHit(enemy);

        /*
         * Impact Projectile은
         * Hit 즉시 비활성화된다.
         */
        if (!m_active)
            return;
    }
}

bool Projectile::CheckEnemyCollision(
    const Vector3& previousPosition,
    const Vector3& currentPosition,
    const std::shared_ptr<Enemy>& enemy
) const
{
    if (!enemy)
        return false;

    const Vector3 enemyPosition =
        enemy->GetPosition();

    /*
     * Projectile + Enemy의 충돌 반경
     *
     * 두 원이 겹치면 충돌한 것으로 판단한다.
     */
    const float collisionRadius =
        ProjectileRadius + EnemyRadius;

    const float collisionRadiusSquared =
        collisionRadius * collisionRadius;

    /*
     * Projectile 이동 방향
     *
     * 2D 게임이므로 X/Y만 사용한다.
     */
    const float dx =
        currentPosition.x -
        previousPosition.x;

    const float dy =
        currentPosition.y -
        previousPosition.y;

    const float lengthSquared =
        dx * dx +
        dy * dy;

    /*
     * Projectile이 이번 프레임에
     * 거의 움직이지 않은 경우
     *
     * 현재 위치와 Enemy 사이의 거리만 검사한다.
     */
    if (lengthSquared <= 0.000001f)
    {
        const float diffX =
            enemyPosition.x -
            currentPosition.x;

        const float diffY =
            enemyPosition.y -
            currentPosition.y;

        const float distanceSquared =
            diffX * diffX +
            diffY * diffY;

        return distanceSquared <=
            collisionRadiusSquared;
    }

    /*
     * Enemy 위치를
     *
     * previousPosition -> currentPosition
     *
     * 선분에 투영한다.
     *
     * t = 0
     *   -> previousPosition
     *
     * t = 1
     *   -> currentPosition
     */
    float t =
        (
            (enemyPosition.x -
                previousPosition.x) * dx
            +
            (enemyPosition.y -
                previousPosition.y) * dy
            )
        / lengthSquared;

    /*
     * 선분 밖으로 나가지 않도록 제한한다.
     */
    t = std::clamp(
        t,
        0.0f,
        1.0f
    );

    /*
     * Enemy와 가장 가까운
     * Projectile 위치
     */
    const float closestX =
        previousPosition.x +
        dx * t;

    const float closestY =
        previousPosition.y +
        dy * t;

    /*
     * 가장 가까운 위치와 Enemy 사이의 거리
     */
    const float diffX =
        enemyPosition.x -
        closestX;

    const float diffY =
        enemyPosition.y -
        closestY;

    const float distanceSquared =
        diffX * diffX +
        diffY * diffY;

    /*
     * Projectile + Enemy의 Collider가
     * 서로 겹쳤는지 확인한다.
     */
    return distanceSquared <=
        collisionRadiusSquared;
}

void Projectile::OnHit(
    std::shared_ptr<Enemy>& enemy
)
{
    if (!enemy)
        return;

    if (!enemy->IsActive())
        return;

    const uint32_t enemyId =
        enemy->GetInstanceId();

    /*
     * Chase + Impact
     *
     * Target 이외의 Enemy는 맞지 않는다.
     *
     * 예:
     *
     * Projectile ---- Enemy B ---- Target A
     *
     * Enemy B는 무시하고
     * Target A만 Hit한다.
     */
    if (m_hitType == ProjectileHitType::Impact)
    {
        if (m_isChase)
        {
            if (!m_target)
                return;

            if (m_target->GetInstanceId() != enemyId)
                return;
        }
    }

    /*
     * 실제 Hit가 확정된 후에만
     * 중복 Hit 목록에 추가한다.
     *
     * 기존 코드에서는 이보다 먼저 추가해서
     * Chase + Impact에서 Target이 아닌 Enemy도
     * Hit 목록에 들어가는 문제가 있었다.
     */
    if (m_hitedEnemies.contains(enemyId))
        return;

    m_hitedEnemies.emplace(
        enemyId,
        enemy
    );

    /*
     * 실제 데미지 처리
     */
    if (m_onAttack)
    {
        m_onAttack(
            enemy,
            m_ownerMinion,
            m_damage
        );
    }

    /*
     * Impact
     *
     * 하나의 Enemy에게 Hit하면 종료한다.
     */
    if (m_hitType == ProjectileHitType::Impact)
    {
        ReturnToPool();
        return;
    }

    /*
     * Pierce
     *
     * 기본적으로 Hit 이후에도 계속 이동한다.
     */
    if (m_hitType == ProjectileHitType::Pierce)
    {
        /*
         * Chase + Pierce의 경우
         * 원래 Target에 도착하면 Projectile 종료.
         */
        if (m_isChase)
        {
            if (m_target &&
                m_target->GetInstanceId() == enemyId)
            {
                ReturnToPool();
            }
        }
    }
}

void Projectile::ReturnToPool()
{
    /*
     * 이미 비활성화된 Projectile이면
     * 중복 반환하지 않는다.
     */
    if (!m_active)
        return;

    m_active = false;

    /*
     * Reset()을 호출하면 m_instanceId가 0이 되므로
     * callback에 전달할 ID를 먼저 저장한다.
     */
    const uint32_t instanceId =
        m_instanceId;

    /*
     * ProjectileManager에 반환 요청
     */
    if (m_onReturn)
    {
        m_onReturn(instanceId);
    }

    /*
     * 마지막에 초기화한다.
     */
    Reset();
}