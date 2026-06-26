#pragma once
#include "cocos2d.h"
#include <functional>

enum class WeaponType { SINGLE = 0, DOUBLE, TRIPLE };

class Player : public cocos2d::Node {
public:
    static Player* create();
    bool init() override;
    void update(float dt) override;

    void setMoveInput(cocos2d::Vec2 dir) { _moveInput = dir; }
    void setBounds(cocos2d::Rect b) { _bounds = b; }
    void takeDamage();
    bool isAlive() const { return _lives > 0; }
    bool isInvincible() const { return _invincibleTimer > 0; }
    int getLives() const { return _lives; }

    void applyDoubleShot(float dur);
    void applyTripleShot(float dur);
    void applySpeedBoost(float dur);
    void applyShield();
    bool hasShield() const { return _shielded; }
    WeaponType getWeaponType() const { return _weaponType; }

    void setShootCallback(std::function<void(cocos2d::Vec2, WeaponType)> cb);
    cocos2d::Rect getBoundingBox() const override;

private:
    int _lives = 3;
    float _speed = 220.0f;
    float _baseSpeed = 220.0f;
    cocos2d::Vec2 _moveInput;
    cocos2d::Rect _bounds;

    float _shootTimer = 0.0f;
    float _shootInterval = 0.28f;

    bool _shielded = false;
    float _invincibleTimer = 0.0f;
    WeaponType _weaponType = WeaponType::SINGLE;
    float _weaponTimer = 0.0f;
    float _speedTimer = 0.0f;

    cocos2d::DrawNode* _draw = nullptr;
    std::function<void(cocos2d::Vec2, WeaponType)> _shootCallback;

    void drawSelf();
};
