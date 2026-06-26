#pragma once
#include "cocos2d.h"

class MainMenuScene : public cocos2d::Scene {
public:
    static cocos2d::Scene* createScene();
    bool init() override;
    CREATE_FUNC(MainMenuScene);

private:
    cocos2d::Size _visibleSize;
    cocos2d::Vec2 _origin;

    void setupBackground();
    void onStartGame(cocos2d::Ref* sender);
};
