#include "Bullet.h"
USING_NS_CC;

Bullet* Bullet::create(BulletOwner owner, Vec2 velocity, Vec2 startPos) {
    auto b = new (std::nothrow) Bullet();
    if (b && b->init(owner, velocity, startPos)) {
        b->autorelease();
        return b;
    }
    CC_SAFE_DELETE(b);
    return nullptr;
}

bool Bullet::init(BulletOwner owner, Vec2 velocity, Vec2 startPos) {
    if (!Node::init()) return false;

    _owner = owner;
    _velocity = velocity;
    setPosition(startPos);

    _draw = DrawNode::create();
    addChild(_draw);

    if (owner == BulletOwner::PLAYER) {
        _draw->drawSolidRect(Vec2(-3, -9), Vec2(3, 9), Color4F(1.0f, 1.0f, 0.2f, 1.0f));
        _draw->drawSolidRect(Vec2(-2, 9), Vec2(2, 14), Color4F(1.0f, 0.8f, 0.0f, 0.7f));
    } else {
        _draw->drawSolidRect(Vec2(-3, -9), Vec2(3, 9), Color4F(1.0f, 0.15f, 0.15f, 1.0f));
        _draw->drawSolidRect(Vec2(-2, -14), Vec2(2, -9), Color4F(1.0f, 0.5f, 0.0f, 0.7f));
    }

    scheduleUpdate();
    return true;
}

void Bullet::update(float dt) {
    if (!_active) return;
    setPosition(getPosition() + _velocity * dt);

    auto vs = Director::getInstance()->getVisibleSize();
    auto p = getPosition();
    if (p.y > vs.height + 20 || p.y < -20 || p.x < -20 || p.x > vs.width + 20) {
        deactivate();
    }
}

Rect Bullet::getBoundingBox() const {
    return Rect(getPositionX() - 3, getPositionY() - 9, 6, 18);
}
