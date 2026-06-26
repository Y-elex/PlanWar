#include "GameOverScene.h"
#include "GameScene.h"
#include "MainMenuScene.h"
USING_NS_CC;

Scene* GameOverScene::createScene(int score, SceneType sceneType) {
    auto scene = new (std::nothrow) GameOverScene();
    if (scene && scene->init(score, sceneType)) {
        scene->autorelease();
        return scene;
    }
    CC_SAFE_DELETE(scene);
    return nullptr;
}

bool GameOverScene::init(int score, SceneType sceneType) {
    if (!Scene::init()) return false;

    _score     = score;
    _sceneType = sceneType;

    auto vs = Director::getInstance()->getVisibleSize();

    // Background tinted by scene type
    Color4F bgColor = (sceneType == SceneType::MOON)  ? Color4F(0.04f, 0.04f, 0.04f, 1.0f) :
                      (sceneType == SceneType::MARS)  ? Color4F(0.10f, 0.03f, 0.02f, 1.0f) :
                      Color4F(0.03f, 0.02f, 0.08f, 1.0f);
    auto bg = DrawNode::create();
    bg->drawSolidRect(Vec2(0, 0), Vec2(vs.width, vs.height), bgColor);
    addChild(bg, 0);

    // Stars
    auto stars = DrawNode::create();
    for (int i = 0; i < 80; i++) {
        float x = CCRANDOM_0_1() * vs.width;
        float y = CCRANDOM_0_1() * vs.height;
        float r = 0.4f + CCRANDOM_0_1() * 1.4f;
        float b = 0.35f + CCRANDOM_0_1() * 0.5f;
        stars->drawDot(Vec2(x, y), r, Color4F(b, b, b, 1.0f));
    }
    addChild(stars, 1);

    // Scene name chip
    const char* sceneName = (sceneType == SceneType::MOON)  ? "MOON"  :
                            (sceneType == SceneType::MARS)  ? "MARS"  : "EARTH";
    auto chipLabel = Label::createWithSystemFont(sceneName, "Arial", 14);
    chipLabel->setTextColor(Color4B(160, 200, 255, 200));
    chipLabel->setPosition(Vec2(vs.width * 0.5f, vs.height * 0.80f));
    addChild(chipLabel, 5);

    // Game Over title
    auto gameOverLabel = Label::createWithSystemFont("GAME OVER", "Arial", 44);
    gameOverLabel->setTextColor(Color4B(255, 60, 60, 255));
    gameOverLabel->setPosition(Vec2(vs.width * 0.5f, vs.height * 0.70f));
    addChild(gameOverLabel, 5);
    gameOverLabel->runAction(RepeatForever::create(Sequence::create(
        FadeTo::create(0.8f, 180), FadeTo::create(0.8f, 255), nullptr)));

    // Score
    auto scoreLabel = Label::createWithSystemFont("Score: " + std::to_string(score), "Arial", 30);
    scoreLabel->setTextColor(Color4B(255, 240, 100, 255));
    scoreLabel->setPosition(Vec2(vs.width * 0.5f, vs.height * 0.56f));
    addChild(scoreLabel, 5);

    // Grade
    std::string grade = (score >= 10000) ? "S  RANK - LEGENDARY!" :
                        (score >= 6000)  ? "A  RANK - EXCELLENT!" :
                        (score >= 3000)  ? "B  RANK - GOOD"        :
                        (score >= 1000)  ? "C  RANK - KEEP TRYING" : "D  RANK - PRACTICE MORE";
    auto gradeLabel = Label::createWithSystemFont(grade, "Arial", 18);
    gradeLabel->setTextColor(Color4B(180, 220, 180, 255));
    gradeLabel->setPosition(Vec2(vs.width * 0.5f, vs.height * 0.46f));
    addChild(gradeLabel, 5);

    // Buttons
    auto replayLabel = Label::createWithSystemFont("[ PLAY AGAIN ]", "Arial", 26);
    replayLabel->setTextColor(Color4B(60, 220, 100, 255));
    auto replayItem = MenuItemLabel::create(replayLabel, CC_CALLBACK_1(GameOverScene::onReplay, this));

    auto menuLabel = Label::createWithSystemFont("[ MAIN MENU ]", "Arial", 22);
    menuLabel->setTextColor(Color4B(150, 200, 255, 255));
    auto menuItem = MenuItemLabel::create(menuLabel, CC_CALLBACK_1(GameOverScene::onMainMenu, this));

    replayItem->setPosition(Vec2(0,  vs.height * 0.10f));
    menuItem->setPosition(Vec2(0,    vs.height * 0.02f));

    auto menu = Menu::create(replayItem, menuItem, nullptr);
    menu->setPosition(Vec2(vs.width * 0.5f, vs.height * 0.25f));
    addChild(menu, 6);

    return true;
}

void GameOverScene::onReplay(Ref*) {
    Director::getInstance()->replaceScene(
        TransitionFade::create(0.4f, GameScene::createScene(_sceneType))
    );
}

void GameOverScene::onMainMenu(Ref*) {
    Director::getInstance()->replaceScene(
        TransitionFade::create(0.4f, MainMenuScene::createScene())
    );
}
