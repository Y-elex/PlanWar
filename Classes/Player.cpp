#include "Player.h"
USING_NS_CC;

Player* Player::create() {
    auto p = new (std::nothrow) Player();
    if (p && p->init()) {
        p->autorelease();
        return p;
    }
    CC_SAFE_DELETE(p);
    return nullptr;
}

bool Player::init() {
    if (!Node::init()) return false;

    _draw = DrawNode::create();
    addChild(_draw);

    drawSelf();
    scheduleUpdate();
    return true;
}

void Player::drawSelf() {
    _draw->clear();

    // Engine flame
    Vec2 flame[] = { Vec2(-6, -22), Vec2(6, -22), Vec2(4, -36), Vec2(0, -42), Vec2(-4, -36) };
    _draw->drawSolidPoly(flame, 5, Color4F(1.0f, 0.5f, 0.0f, 0.9f));

    // Main fuselage
    Vec2 body[] = { Vec2(0, 30), Vec2(-15, -15), Vec2(15, -15) };
    _draw->drawSolidPoly(body, 3, Color4F(0.2f, 0.55f, 0.95f, 1.0f));

    // Left wing
    Vec2 lwing[] = { Vec2(-15, -15), Vec2(-32, -5), Vec2(-22, 8), Vec2(-10, 2) };
    _draw->drawSolidPoly(lwing, 4, Color4F(0.15f, 0.42f, 0.82f, 1.0f));

    // Right wing
    Vec2 rwing[] = { Vec2(15, -15), Vec2(32, -5), Vec2(22, 8), Vec2(10, 2) };
    _draw->drawSolidPoly(rwing, 4, Color4F(0.15f, 0.42f, 0.82f, 1.0f));

    // Cockpit highlight
    Vec2 cockpit[] = { Vec2(0, 22), Vec2(-7, 4), Vec2(7, 4) };
    _draw->drawSolidPoly(cockpit, 3, Color4F(0.75f, 0.92f, 1.0f, 0.85f));

    // Left gun barrel
    _draw->drawSolidRect(Vec2(-17, -8), Vec2(-13, 8), Color4F(0.55f, 0.55f, 0.65f, 1.0f));

    // Right gun barrel
    _draw->drawSolidRect(Vec2(13, -8), Vec2(17, 8), Color4F(0.55f, 0.55f, 0.65f, 1.0f));

    // Shield circle
    if (_shielded) {
        _draw->drawCircle(Vec2::ZERO, 40, 0.0f, 32, false, Color4F(0.3f, 1.0f, 0.3f, 0.55f));
        _draw->drawCircle(Vec2::ZERO, 38, 0.0f, 32, false, Color4F(0.5f, 1.0f, 0.5f, 0.3f));
    }
}

void Player::update(float dt) {
    // Movement
    Vec2 newPos = getPosition() + _moveInput * _speed * dt;
    if (_bounds.size.width > 0) {
        newPos.x = std::max(_bounds.getMinX() + 30.0f, std::min(_bounds.getMaxX() - 30.0f, newPos.x));
        newPos.y = std::max(_bounds.getMinY() + 42.0f, std::min(_bounds.getMaxY() - 42.0f, newPos.y));
    }
    setPosition(newPos);

    // Auto-shoot
    _shootTimer += dt;
    if (_shootTimer >= _shootInterval && _shootCallback) {
        _shootTimer = 0;
        _shootCallback(getPosition() + Vec2(0, 28), _weaponType);
    }

    // Weapon power-up timer
    if (_weaponTimer > 0) {
        _weaponTimer -= dt;
        if (_weaponTimer <= 0) {
            _weaponType = WeaponType::SINGLE;
            drawSelf();
        }
    }

    // Speed boost timer
    if (_speedTimer > 0) {
        _speedTimer -= dt;
        if (_speedTimer <= 0) {
            _speed = _baseSpeed;
        }
    }

    // Invincibility blink
    if (_invincibleTimer > 0) {
        _invincibleTimer -= dt;
        int frame = static_cast<int>(_invincibleTimer * 10);
        setOpacity(frame % 2 == 0 ? 255 : 70);
        if (_invincibleTimer <= 0) {
            setOpacity(255);
        }
    }
}

void Player::takeDamage() {
    if (_invincibleTimer > 0) return;

    if (_shielded) {
        _shielded = false;
        drawSelf();
        _invincibleTimer = 1.5f;
        return;
    }

    _lives--;
    _weaponType = WeaponType::SINGLE;
    _weaponTimer = 0;
    _invincibleTimer = 2.2f;
    drawSelf();
}

void Player::applyDoubleShot(float dur) {
    _weaponType = WeaponType::DOUBLE;
    _weaponTimer = dur;
}

void Player::applyTripleShot(float dur) {
    _weaponType = WeaponType::TRIPLE;
    _weaponTimer = dur;
}

void Player::applySpeedBoost(float dur) {
    _speed = _baseSpeed * 1.65f;
    _speedTimer = dur;
}

void Player::applyShield() {
    _shielded = true;
    drawSelf();
}

void Player::setShootCallback(std::function<void(Vec2, WeaponType)> cb) {
    _shootCallback = cb;
}

Rect Player::getBoundingBox() const {
    return Rect(getPositionX() - 22, getPositionY() - 15, 44, 45);
}
