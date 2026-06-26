#pragma once
#include "cocos2d.h"
#include "GameDefs.h"

class GameOverScene : public cocos2d::Scene {
public:
    static cocos2d::Scene* createScene(int score, SceneType sceneType = SceneType::EARTH);
    bool init(int score, SceneType sceneType);

private:
    int       _score     = 0;
    SceneType _sceneType = SceneType::EARTH;

    void onReplay(cocos2d::Ref* sender);
    void onMainMenu(cocos2d::Ref* sender);
};
