#pragma once
#include "cocos2d.h"
#include "GameDefs.h"

class MainMenuScene : public cocos2d::Scene {
public:
    static cocos2d::Scene* createScene();
    bool init() override;
    CREATE_FUNC(MainMenuScene);

private:
    cocos2d::Size _visibleSize;
    cocos2d::Vec2 _origin;

    SceneType           _selectedScene = SceneType::EARTH;
    cocos2d::DrawNode*  _selectionBorder = nullptr;

    // Card layout constants
    static constexpr float CARD_W   = 132.0f;
    static constexpr float CARD_H   = 110.0f;
    static constexpr float CARD_GAP = 12.0f;

    float _cardCenterX[3];
    float _cardCenterY = 0.0f;

    void setupBackground();
    void buildSceneCards();
    void selectScene(SceneType type);
    void refreshSelectionBorder();
    void onStartGame(cocos2d::Ref* sender);
};
