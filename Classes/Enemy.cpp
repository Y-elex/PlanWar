#include "Enemy.h"
USING_NS_CC;

Enemy* Enemy::create(EnemyType type, Vec2 pos) {
    auto e = new (std::nothrow) Enemy();
    if (e && e->init(type, pos)) {
        e->autorelease();
        return e;
    }
    CC_SAFE_DELETE(e);
    return nullptr;
}

bool Enemy::init(EnemyType type, Vec2 pos) {
    if (!Node::init()) return false;

    _type = type;
    setPosition(pos);

    switch (type) {
        case EnemyType::SMALL:
            _hp = _maxHp = 1;
            _speed = 155.0f;
            _shootInterval = 3.8f;
            break;
        case EnemyType::MEDIUM:
            _hp = _maxHp = 3;
            _speed = 95.0f;
            _shootInterval = 2.2f;
            break;
        case EnemyType::LARGE:
            _hp = _maxHp = 8;
            _speed = 55.0f;
            _shootInterval = 2.8f;
            break;
    }
    _shootTimer = _shootInterval * (0.3f + CCRANDOM_0_1() * 0.4f);

    _draw = DrawNode::create();
    addChild(_draw);

    drawSelf();
    scheduleUpdate();
    return true;
}

void Enemy::drawSelf() {
    _draw->clear();

    switch (_type) {
        case EnemyType::SMALL: {
            Vec2 pts[] = { Vec2(0, 16), Vec2(13, 0), Vec2(0, -16), Vec2(-13, 0) };
            _draw->drawSolidPoly(pts, 4, Color4F(0.95f, 0.15f, 0.15f, 1.0f));
            Vec2 inner[] = { Vec2(0, 9), Vec2(7, 0), Vec2(0, -9), Vec2(-7, 0) };
            _draw->drawSolidPoly(inner, 4, Color4F(1.0f, 0.55f, 0.55f, 0.85f));
            break;
        }
        case EnemyType::MEDIUM: {
            Vec2 pts[5];
            for (int i = 0; i < 5; i++) {
                float a = (float)M_PI / 2 + i * (2.0f * (float)M_PI / 5);
                pts[i] = Vec2(24 * cosf(a), 24 * sinf(a));
            }
            _draw->drawSolidPoly(pts, 5, Color4F(1.0f, 0.48f, 0.08f, 1.0f));
            Vec2 inner[5];
            for (int i = 0; i < 5; i++) {
                float a = (float)M_PI / 2 + i * (2.0f * (float)M_PI / 5);
                inner[i] = Vec2(13 * cosf(a), 13 * sinf(a));
            }
            _draw->drawSolidPoly(inner, 5, Color4F(1.0f, 0.72f, 0.38f, 0.9f));
            break;
        }
        case EnemyType::LARGE: {
            Vec2 pts[6];
            for (int i = 0; i < 6; i++) {
                float a = i * ((float)M_PI / 3);
                pts[i] = Vec2(33 * cosf(a), 33 * sinf(a));
            }
            _draw->drawSolidPoly(pts, 6, Color4F(0.58f, 0.0f, 0.0f, 1.0f));

            Vec2 lwing[] = { Vec2(-33, 0), Vec2(-50, -12), Vec2(-42, 14) };
            _draw->drawSolidPoly(lwing, 3, Color4F(0.68f, 0.1f, 0.1f, 1.0f));
            Vec2 rwing[] = { Vec2(33, 0), Vec2(50, -12), Vec2(42, 14) };
            _draw->drawSolidPoly(rwing, 3, Color4F(0.68f, 0.1f, 0.1f, 1.0f));

            // HP bar background
            _draw->drawSolidRect(Vec2(-30, -44), Vec2(30, -38), Color4F(0.25f, 0.0f, 0.0f, 1.0f));
            // HP bar fill
            float ratio = static_cast<float>(_hp) / _maxHp;
            Color4F barColor = ratio > 0.5f ? Color4F(0.1f, 0.9f, 0.1f, 1.0f) :
                               ratio > 0.25f ? Color4F(0.9f, 0.8f, 0.1f, 1.0f) :
                               Color4F(0.9f, 0.1f, 0.1f, 1.0f);
            _draw->drawSolidRect(Vec2(-30, -44), Vec2(-30 + 60 * ratio, -38), barColor);
            break;
        }
    }
}

void Enemy::update(float dt) {
    if (_hp <= 0) return;

    setPositionY(getPositionY() - _speed * dt);

    _shootTimer += dt;
    if (_shootTimer >= _shootInterval && _shootCallback) {
        _shootTimer = 0;

        Vec2 toPlayer = (_playerPos - getPosition());
        float dist = toPlayer.length();
        if (dist < 1.0f) dist = 1.0f;
        Vec2 dir = toPlayer / dist;

        if (_type == EnemyType::SMALL) {
            _shootCallback(getPosition(), Vec2(0, -210.0f));
        } else if (_type == EnemyType::MEDIUM) {
            _shootCallback(getPosition(), dir * 210.0f);
        } else {
            float baseAngle = atan2f(dir.y, dir.x);
            float spread = (float)M_PI / 12.0f; // 15 degrees
            for (int i = -1; i <= 1; i++) {
                float a = baseAngle + i * spread;
                Vec2 v(cosf(a) * 185.0f, sinf(a) * 185.0f);
                _shootCallback(getPosition(), v);
            }
        }
    }
}

bool Enemy::takeDamage(int dmg) {
    if (_hp <= 0) return true;
    _hp -= dmg;
    if (_hp < 0) _hp = 0;

    // Hit flash: hide then show draw node
    _draw->runAction(Sequence::create(
        Hide::create(),
        DelayTime::create(0.05f),
        Show::create(),
        DelayTime::create(0.04f),
        Hide::create(),
        DelayTime::create(0.04f),
        Show::create(),
        nullptr
    ));

    if (_type == EnemyType::LARGE) {
        drawSelf(); // redraw HP bar
    }

    return _hp <= 0;
}

int Enemy::getScoreValue() const {
    switch (_type) {
        case EnemyType::SMALL:  return 100;
        case EnemyType::MEDIUM: return 300;
        case EnemyType::LARGE:  return 800;
    }
    return 0;
}

Rect Enemy::getBoundingBox() const {
    float r = (_type == EnemyType::SMALL) ? 13.0f :
              (_type == EnemyType::MEDIUM) ? 24.0f : 33.0f;
    return Rect(getPositionX() - r, getPositionY() - r, r * 2, r * 2);
}

void Enemy::setShootCallback(std::function<void(Vec2, Vec2)> cb) {
    _shootCallback = cb;
}
