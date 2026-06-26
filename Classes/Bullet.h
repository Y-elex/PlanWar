#pragma once
#include "cocos2d.h"

enum class BulletOwner { PLAYER, ENEMY };

class Bullet : public cocos2d::Node {
public:
    static Bullet* create(BulletOwner owner, cocos2d::Vec2 velocity, cocos2d::Vec2 startPos);
    bool init(BulletOwner owner, cocos2d::Vec2 velocity, cocos2d::Vec2 startPos);
    void update(float dt) override;

    bool isActive() const { return _active; }
    void deactivate() { _active = false; setVisible(false); }
    BulletOwner getOwner() const { return _owner; }
    cocos2d::Rect getBoundingBox() const override;

private:
    BulletOwner _owner;
    cocos2d::Vec2 _velocity;
    bool _active = true;
    cocos2d::DrawNode* _draw = nullptr;
};
