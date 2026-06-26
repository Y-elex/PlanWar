#pragma once
#include "cocos2d.h"

enum class PowerUpType {
    DOUBLE_SHOT = 0,
    TRIPLE_SHOT,
    SPEED_BOOST,
    SHIELD,
    BOMB
};

class PowerUp : public cocos2d::Node {
public:
    static PowerUp* create(PowerUpType type, cocos2d::Vec2 pos);
    bool init(PowerUpType type, cocos2d::Vec2 pos);
    void update(float dt) override;

    bool isCollected() const { return _collected; }
    void collect() { _collected = true; setVisible(false); }
    PowerUpType getType() const { return _type; }
    cocos2d::Rect getBoundingBox() const override;

private:
    PowerUpType _type;
    float _speed = 75.0f;
    bool _collected = false;
    cocos2d::DrawNode* _draw = nullptr;
    cocos2d::Label* _label = nullptr;
};
