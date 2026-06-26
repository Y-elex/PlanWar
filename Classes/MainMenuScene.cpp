#include "MainMenuScene.h"
#include "GameScene.h"
USING_NS_CC;

Scene* MainMenuScene::createScene() {
    return MainMenuScene::create();
}

bool MainMenuScene::init() {
    if (!Scene::init()) return false;

    _visibleSize = Director::getInstance()->getVisibleSize();
    _origin = Director::getInstance()->getVisibleOrigin();

    setupBackground();

    // Title
    auto title = Label::createWithSystemFont("PLANE WAR", "Arial", 42);
    title->setTextColor(Color4B(80, 200, 255, 255));
    title->setPosition(Vec2(_visibleSize.width * 0.5f, _visibleSize.height * 0.72f));
    addChild(title, 5);

    auto subtitle = Label::createWithSystemFont("飞机大战", "Arial", 22);
    subtitle->setTextColor(Color4B(180, 230, 255, 220));
    subtitle->setPosition(Vec2(_visibleSize.width * 0.5f, _visibleSize.height * 0.62f));
    addChild(subtitle, 5);

    // Small decorative planes
    auto decoDraw = DrawNode::create();
    decoDraw->setPosition(Vec2(_visibleSize.width * 0.5f, _visibleSize.height * 0.50f));
    // Player-style triangle
    Vec2 bodyPts[] = { Vec2(0, 24), Vec2(-12, -12), Vec2(12, -12) };
    decoDraw->drawSolidPoly(bodyPts, 3, Color4F(0.2f, 0.55f, 0.95f, 0.85f));
    addChild(decoDraw, 4);

    // Instructions
    auto ctrl = Label::createWithSystemFont("Arrow Keys / WASD to move\nAuto-fire  |  Collect power-ups!", "Arial", 15);
    ctrl->setTextColor(Color4B(200, 200, 200, 220));
    ctrl->setAlignment(cocos2d::TextHAlignment::CENTER);
    ctrl->setPosition(Vec2(_visibleSize.width * 0.5f, _visibleSize.height * 0.36f));
    addChild(ctrl, 5);

    // Start button
    auto startLabel = Label::createWithSystemFont("[ START GAME ]", "Arial", 28);
    startLabel->setTextColor(Color4B(50, 255, 120, 255));
    auto startItem = MenuItemLabel::create(startLabel, CC_CALLBACK_1(MainMenuScene::onStartGame, this));
    startItem->setPosition(Vec2(0, -_visibleSize.height * 0.15f));

    auto menu = Menu::create(startItem, nullptr);
    menu->setPosition(Vec2(_visibleSize.width * 0.5f, _visibleSize.height * 0.5f));
    addChild(menu, 6);

    // Pulse animation on start button
    startItem->runAction(RepeatForever::create(Sequence::create(
        FadeTo::create(0.7f, 160),
        FadeTo::create(0.7f, 255),
        nullptr
    )));

    return true;
}

void MainMenuScene::setupBackground() {
    auto bg = DrawNode::create();
    bg->drawSolidRect(Vec2(0, 0), Vec2(_visibleSize.width, _visibleSize.height),
                      Color4F(0.02f, 0.02f, 0.09f, 1.0f));
    addChild(bg, -2);

    auto stars = DrawNode::create();
    for (int i = 0; i < 90; i++) {
        float x = CCRANDOM_0_1() * _visibleSize.width;
        float y = CCRANDOM_0_1() * _visibleSize.height;
        float r = 0.4f + CCRANDOM_0_1() * 1.5f;
        float b = 0.4f + CCRANDOM_0_1() * 0.6f;
        stars->drawDot(Vec2(x, y), r, Color4F(b, b, b * 0.85f, 1.0f));
    }
    addChild(stars, -1);
}

void MainMenuScene::onStartGame(Ref*) {
    Director::getInstance()->replaceScene(
        TransitionFade::create(0.4f, GameScene::createScene())
    );
}
