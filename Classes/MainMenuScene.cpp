#include "MainMenuScene.h"
#include "GameScene.h"
USING_NS_CC;

Scene* MainMenuScene::createScene() {
    return MainMenuScene::create();
}

bool MainMenuScene::init() {
    if (!Scene::init()) return false;

    _visibleSize = Director::getInstance()->getVisibleSize();
    _origin      = Director::getInstance()->getVisibleOrigin();

    setupBackground();

    // ── Title ──────────────────────────────────────────────────────────────
    auto title = Label::createWithSystemFont("PLANE WAR", "Arial", 42);
    title->setTextColor(Color4B(80, 200, 255, 255));
    title->setPosition(Vec2(_visibleSize.width * 0.5f, _visibleSize.height * 0.88f));
    addChild(title, 5);

    auto subtitle = Label::createWithSystemFont("飞机大战", "Arial", 20);
    subtitle->setTextColor(Color4B(180, 230, 255, 210));
    subtitle->setPosition(Vec2(_visibleSize.width * 0.5f, _visibleSize.height * 0.80f));
    addChild(subtitle, 5);

    // ── "选择战场" label ────────────────────────────────────────────────────
    auto selectLabel = Label::createWithSystemFont("- 选择战场 -", "Arial", 18);
    selectLabel->setTextColor(Color4B(160, 210, 255, 220));
    selectLabel->setPosition(Vec2(_visibleSize.width * 0.5f, _visibleSize.height * 0.70f));
    addChild(selectLabel, 5);

    // ── Scene selection cards ─────────────────────────────────────────────
    buildSceneCards();

    // ── Instructions ──────────────────────────────────────────────────────
    auto ctrl = Label::createWithSystemFont(
        "Arrow Keys / WASD  to move\nAuto-fire  |  Collect power-ups!", "Arial", 14);
    ctrl->setTextColor(Color4B(190, 190, 190, 200));
    ctrl->setAlignment(TextHAlignment::CENTER);
    ctrl->setPosition(Vec2(_visibleSize.width * 0.5f, _visibleSize.height * 0.24f));
    addChild(ctrl, 5);

    // ── START button ───────────────────────────────────────────────────────
    auto startLabel = Label::createWithSystemFont("[ START GAME ]", "Arial", 28);
    startLabel->setTextColor(Color4B(50, 255, 120, 255));
    auto startItem = MenuItemLabel::create(startLabel,
        CC_CALLBACK_1(MainMenuScene::onStartGame, this));
    startItem->setPosition(Vec2(0, 0));

    auto menu = Menu::create(startItem, nullptr);
    menu->setPosition(Vec2(_visibleSize.width * 0.5f, _visibleSize.height * 0.12f));
    addChild(menu, 6);

    startItem->runAction(RepeatForever::create(Sequence::create(
        FadeTo::create(0.7f, 150), FadeTo::create(0.7f, 255), nullptr)));

    return true;
}

// ─────────────────────────────────────────────────────────────────────────────
// Background
// ─────────────────────────────────────────────────────────────────────────────

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

// ─────────────────────────────────────────────────────────────────────────────
// Scene cards
// ─────────────────────────────────────────────────────────────────────────────

void MainMenuScene::buildSceneCards() {
    _cardCenterY = _visibleSize.height * 0.52f;

    float totalW = 3 * CARD_W + 2 * CARD_GAP;
    float startX = (_visibleSize.width - totalW) * 0.5f;
    for (int i = 0; i < 3; i++) {
        _cardCenterX[i] = startX + CARD_W * 0.5f + i * (CARD_W + CARD_GAP);
    }

    // ─ Card visuals ─────────────────────────────────────────────────────────
    struct CardInfo {
        Color4F bgColor;
        Color4F starColor;
        const char* chineseName;
        const char* englishName;
    };
    CardInfo infos[3] = {
        // EARTH
        { Color4F(0.03f, 0.05f, 0.20f, 1.0f), Color4F(0.80f, 0.85f, 1.00f, 1.0f), "地球", "EARTH" },
        // MOON
        { Color4F(0.08f, 0.08f, 0.09f, 1.0f), Color4F(0.95f, 0.95f, 0.95f, 1.0f), "月球", "MOON"  },
        // MARS
        { Color4F(0.20f, 0.06f, 0.02f, 1.0f), Color4F(1.00f, 0.72f, 0.38f, 1.0f), "火星", "MARS"  },
    };

    for (int i = 0; i < 3; i++) {
        Vec2 center(_cardCenterX[i], _cardCenterY);
        const CardInfo& info = infos[i];

        // Card background
        auto card = DrawNode::create();
        card->setPosition(center);
        card->drawSolidRect(Vec2(-CARD_W * 0.5f, -CARD_H * 0.5f),
                            Vec2( CARD_W * 0.5f,  CARD_H * 0.5f), info.bgColor);

        // Stars inside card
        for (int s = 0; s < 18; s++) {
            float sx = (CCRANDOM_0_1() - 0.5f) * (CARD_W - 10);
            float sy = (CCRANDOM_0_1() - 0.5f) * (CARD_H - 28) + 8;
            float sr = 0.5f + CCRANDOM_0_1() * 1.2f;
            float sb = 0.45f + CCRANDOM_0_1() * 0.55f;
            card->drawDot(Vec2(sx, sy), sr, Color4F(info.starColor.r * sb,
                                                    info.starColor.g * sb,
                                                    info.starColor.b * sb, 1.0f));
        }

        // Planet icon (upper-centre of card)
        Vec2 planetPos(0, CARD_H * 0.5f - 28);
        if (i == 0) { // Earth: blue + green patches
            card->drawSolidCircle(planetPos, 18, 0.0f, 20, Color4F(0.10f, 0.30f, 0.80f, 0.90f));
            Vec2 land1[] = { Vec2(-8, 4), Vec2(-2, 12), Vec2(6, 8), Vec2(4, 0) };
            card->drawSolidPoly(land1, 4, Color4F(0.15f, 0.65f, 0.20f, 0.85f));
        } else if (i == 1) { // Moon: grey circle with crater
            card->drawSolidCircle(planetPos, 18, 0.0f, 20, Color4F(0.62f, 0.62f, 0.62f, 0.90f));
            card->drawCircle(planetPos + Vec2(5, 4),  7, 0.0f, 12, false, Color4F(0.42f,0.42f,0.42f,0.70f));
            card->drawCircle(planetPos + Vec2(-6, -3), 4, 0.0f, 10, false, Color4F(0.40f,0.40f,0.40f,0.60f));
        } else { // Mars: rust circle with polar cap
            card->drawSolidCircle(planetPos, 18, 0.0f, 20, Color4F(0.72f, 0.28f, 0.08f, 0.90f));
            card->drawSolidCircle(planetPos + Vec2(0, 14), 6, 0.0f, 12, Color4F(0.90f, 0.90f, 0.90f, 0.70f));
        }

        addChild(card, 4);

        // Scene name label (bottom of card)
        auto nameLabel = Label::createWithSystemFont(
            std::string(info.chineseName) + "  " + info.englishName, "Arial", 14);
        nameLabel->setTextColor(Color4B::WHITE);
        nameLabel->setPosition(Vec2(center.x, center.y - CARD_H * 0.5f + 12));
        addChild(nameLabel, 5);
    }

    // ─ Selection border (moves to selected card) ─────────────────────────────
    _selectionBorder = DrawNode::create();
    addChild(_selectionBorder, 7);
    refreshSelectionBorder();

    // ─ Touch listener for card clicks ────────────────────────────────────────
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(false);
    listener->onTouchBegan = [this](Touch* touch, Event*) -> bool {
        Vec2 pos = touch->getLocation();
        for (int i = 0; i < 3; i++) {
            Rect hitBox(_cardCenterX[i] - CARD_W * 0.5f,
                        _cardCenterY   - CARD_H * 0.5f,
                        CARD_W, CARD_H);
            if (hitBox.containsPoint(pos)) {
                selectScene(static_cast<SceneType>(i));
                return true;
            }
        }
        return false;
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void MainMenuScene::selectScene(SceneType type) {
    if (_selectedScene == type) return;
    _selectedScene = type;
    refreshSelectionBorder();

    // Brief bounce on the selected card
    int idx = static_cast<int>(type);
    auto card = getChildByTag(200 + idx); // not used — we draw everything in DrawNode
    // Animate the border instead
    _selectionBorder->runAction(Sequence::create(
        ScaleTo::create(0.08f, 1.05f),
        ScaleTo::create(0.08f, 1.00f),
        nullptr
    ));
}

void MainMenuScene::refreshSelectionBorder() {
    _selectionBorder->clear();

    int idx = static_cast<int>(_selectedScene);
    Vec2 center(_cardCenterX[idx], _cardCenterY);
    float hw = CARD_W * 0.5f + 4;
    float hh = CARD_H * 0.5f + 4;

    // Outer glow
    _selectionBorder->drawRect(center + Vec2(-hw - 2, -hh - 2),
                               center + Vec2( hw + 2,  hh + 2),
                               Color4F(1.0f, 1.0f, 1.0f, 0.15f));
    // Bright border
    _selectionBorder->drawRect(center + Vec2(-hw, -hh),
                               center + Vec2( hw,  hh),
                               Color4F(1.0f, 0.92f, 0.3f, 0.95f));

    // Scene label below cards
    // (re-set colour hints on name labels would require keeping refs;
    //  the yellow border is clear enough)
}

// ─────────────────────────────────────────────────────────────────────────────

void MainMenuScene::onStartGame(Ref*) {
    Director::getInstance()->replaceScene(
        TransitionFade::create(0.4f, GameScene::createScene(_selectedScene))
    );
}
