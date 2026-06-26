#pragma once
#include "cocos2d.h"
#include <functional>

enum class EnemyType { SMALL = 0, MEDIUM, LARGE };

class Enemy : public cocos2d::Node {
public:
    static Enemy* create(EnemyType type, cocos2d::Vec2 pos);
    bool init(EnemyType type, cocos2d::Vec2 pos);
    void update(float dt) override;

    bool takeDamage(int dmg = 1);
    void kill() { _hp = 0; }
    bool isDead() const { return _hp <= 0; }
    EnemyType getType() const { return _type; }
    int getScoreValue() const;
    void setPlayerPosition(cocos2d::Vec2 pos) { _playerPos = pos; }
    cocos2d::Rect getBoundingBox() const override;
    void setShootCallback(std::function<void(cocos2d::Vec2, cocos2d::Vec2)> cb);

private:
    EnemyType _type;
    int _hp = 1;
    int _maxHp = 1;
    float _speed = 100.0f;
    float _shootTimer = 0.0f;
    float _shootInterval = 3.0f;
    cocos2d::Vec2 _playerPos;
    cocos2d::DrawNode* _draw = nullptr;
    std::function<void(cocos2d::Vec2, cocos2d::Vec2)> _shootCallback;

    void drawSelf();
};
