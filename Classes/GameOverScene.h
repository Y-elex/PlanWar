#pragma once
#include "cocos2d.h"

class GameOverScene : public cocos2d::Scene {
public:
    static cocos2d::Scene* createScene(int score);
    bool init(int score);

private:
    int _score = 0;
    void onReplay(cocos2d::Ref* sender);
    void onMainMenu(cocos2d::Ref* sender);
};
