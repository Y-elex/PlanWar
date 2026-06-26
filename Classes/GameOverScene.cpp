#include "GameOverScene.h"
#include "GameScene.h"
#include "MainMenuScene.h"
USING_NS_CC;

Scene* GameOverScene::createScene(int score) {
    auto scene = new (std::nothrow) GameOverScene();
    if (scene && scene->init(score)) {
        scene->autorelease();
        return scene;
    }
    CC_SAFE_DELETE(scene);
    return nullptr;
}

bool GameOverScene::init(int score) {
    if (!Scene::init()) return false;

    _score = score;
    auto vs = Director::getInstance()->getVisibleSize();

    // Background
    auto bg = DrawNode::create();
    bg->drawSolidRect(Vec2(0, 0), Vec2(vs.width, vs.height), Color4F(0.04f, 0.0f, 0.04f, 1.0f));
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

    // Game Over title
    auto gameOverLabel = Label::createWithSystemFont("GAME OVER", "Arial", 44);
    gameOverLabel->setTextColor(Color4B(255, 60, 60, 255));
    gameOverLabel->setPosition(Vec2(vs.width * 0.5f, vs.height * 0.70f));
    addChild(gameOverLabel, 5);

    // Score
    auto scoreLabel = Label::createWithSystemFont("Score: " + std::to_string(score), "Arial", 30);
    scoreLabel->setTextColor(Color4B(255, 240, 100, 255));
    scoreLabel->setPosition(Vec2(vs.width * 0.5f, vs.height * 0.55f));
    addChild(scoreLabel, 5);

    // Grade message
    std::string grade;
    if      (score >= 10000) grade = "S  RANK - LEGENDARY!";
    else if (score >= 6000)  grade = "A  RANK - EXCELLENT!";
    else if (score >= 3000)  grade = "B  RANK - GOOD";
    else if (score >= 1000)  grade = "C  RANK - KEEP TRYING";
    else                     grade = "D  RANK - PRACTICE MORE";

    auto gradeLabel = Label::createWithSystemFont(grade, "Arial", 18);
    gradeLabel->setTextColor(Color4B(180, 220, 180, 255));
    gradeLabel->setPosition(Vec2(vs.width * 0.5f, vs.height * 0.45f));
    addChild(gradeLabel, 5);

    // Replay button
    auto replayLabel = Label::createWithSystemFont("[ PLAY AGAIN ]", "Arial", 26);
    replayLabel->setTextColor(Color4B(60, 220, 100, 255));
    auto replayItem = MenuItemLabel::create(replayLabel, CC_CALLBACK_1(GameOverScene::onReplay, this));

    // Main menu button
    auto menuLabel = Label::createWithSystemFont("[ MAIN MENU ]", "Arial", 22);
    menuLabel->setTextColor(Color4B(150, 200, 255, 255));
    auto menuItem = MenuItemLabel::create(menuLabel, CC_CALLBACK_1(GameOverScene::onMainMenu, this));

    replayItem->setPosition(Vec2(0, vs.height * 0.10f));
    menuItem->setPosition(Vec2(0, vs.height * 0.02f));

    auto menu = Menu::create(replayItem, menuItem, nullptr);
    menu->setPosition(Vec2(vs.width * 0.5f, vs.height * 0.25f));
    addChild(menu, 6);

    // Animate title
    gameOverLabel->runAction(RepeatForever::create(Sequence::create(
        FadeTo::create(0.8f, 180),
        FadeTo::create(0.8f, 255),
        nullptr
    )));

    return true;
}

void GameOverScene::onReplay(Ref*) {
    Director::getInstance()->replaceScene(
        TransitionFade::create(0.4f, GameScene::createScene())
    );
}

void GameOverScene::onMainMenu(Ref*) {
    Director::getInstance()->replaceScene(
        TransitionFade::create(0.4f, MainMenuScene::createScene())
    );
}
