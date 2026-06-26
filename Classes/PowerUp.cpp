#include "PowerUp.h"
USING_NS_CC;

PowerUp* PowerUp::create(PowerUpType type, Vec2 pos) {
    auto p = new (std::nothrow) PowerUp();
    if (p && p->init(type, pos)) {
        p->autorelease();
        return p;
    }
    CC_SAFE_DELETE(p);
    return nullptr;
}

bool PowerUp::init(PowerUpType type, Vec2 pos) {
    if (!Node::init()) return false;

    _type = type;
    setPosition(pos);

    _draw = DrawNode::create();
    addChild(_draw, 0);

    Color4F fillColor;
    Color4F rimColor(1, 1, 1, 0.7f);
    std::string text;

    switch (type) {
        case PowerUpType::DOUBLE_SHOT:
            fillColor = Color4F(0.0f, 0.9f, 0.9f, 0.9f); text = "x2"; break;
        case PowerUpType::TRIPLE_SHOT:
            fillColor = Color4F(0.9f, 0.0f, 0.9f, 0.9f); text = "x3"; break;
        case PowerUpType::SPEED_BOOST:
            fillColor = Color4F(1.0f, 0.9f, 0.0f, 0.9f); text = "SP"; break;
        case PowerUpType::SHIELD:
            fillColor = Color4F(0.1f, 0.9f, 0.2f, 0.9f); text = "SH"; break;
        case PowerUpType::BOMB:
            fillColor = Color4F(1.0f, 0.38f, 0.0f, 0.9f); text = "BM"; break;
    }

    _draw->drawSolidCircle(Vec2::ZERO, 17, 0.0f, 24, fillColor);
    _draw->drawCircle(Vec2::ZERO, 17, 0.0f, 24, false, rimColor);

    _label = Label::createWithSystemFont(text, "Arial", 14);
    _label->setTextColor(Color4B::WHITE);
    _label->setPosition(Vec2::ZERO);
    addChild(_label, 1);

    runAction(RepeatForever::create(Sequence::create(
        ScaleTo::create(0.45f, 1.12f),
        ScaleTo::create(0.45f, 0.88f),
        nullptr
    )));

    scheduleUpdate();
    return true;
}

void PowerUp::update(float dt) {
    if (_collected) return;
    setPositionY(getPositionY() - _speed * dt);
    if (getPositionY() < -25) {
        _collected = true;
        setVisible(false);
    }
}

Rect PowerUp::getBoundingBox() const {
    return Rect(getPositionX() - 17, getPositionY() - 17, 34, 34);
}
