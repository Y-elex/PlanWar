#include "GameScene.h"
#include "GameOverScene.h"
#include <algorithm>
USING_NS_CC;

// ── Factory ──────────────────────────────────────────────────────────────────

GameScene* GameScene::create(SceneType type) {
    auto g = new (std::nothrow) GameScene();
    if (g) {
        g->_sceneType = type;
        if (g->init()) {
            g->autorelease();
            return g;
        }
    }
    CC_SAFE_DELETE(g);
    return nullptr;
}

Scene* GameScene::createScene(SceneType type) {
    return GameScene::create(type);
}

// ── Init ─────────────────────────────────────────────────────────────────────

bool GameScene::init() {
    if (!Scene::init()) return false;

    _visibleSize = Director::getInstance()->getVisibleSize();
    _origin      = Director::getInstance()->getVisibleOrigin();

    setupBackground();
    setupUI();

    _player = Player::create();
    _player->setPosition(_visibleSize.width * 0.5f, 110);
    _player->setBounds(Rect(_origin.x, _origin.y, _visibleSize.width, _visibleSize.height));
    _player->setShootCallback([this](Vec2 pos, WeaponType wt) {
        onPlayerShoot(pos, wt);
    });
    addChild(_player, 5);

    // Show scene name banner for 2 s
    const char* bannerText = (_sceneType == SceneType::EARTH) ? "EARTH" :
                             (_sceneType == SceneType::MOON)  ? "MOON"  : "MARS";
    auto banner = Label::createWithSystemFont(bannerText, "Arial", 22);
    banner->setTextColor(Color4B(200, 220, 255, 220));
    banner->setPosition(Vec2(_visibleSize.width * 0.5f, _visibleSize.height - 48));
    addChild(banner, 10);
    banner->runAction(Sequence::create(
        DelayTime::create(2.0f),
        FadeOut::create(0.5f),
        RemoveSelf::create(),
        nullptr
    ));

    setupInput();
    scheduleUpdate();
    return true;
}

// ── Backgrounds ───────────────────────────────────────────────────────────────

void GameScene::setupBackground() {
    switch (_sceneType) {
        case SceneType::EARTH: setupEarthBg(); break;
        case SceneType::MOON:  setupMoonBg();  break;
        case SceneType::MARS:  setupMarsBg();  break;
    }
}

// Load a planet image as two tiled sprite layers (tags 100 & 101) that scroll.
void GameScene::loadPlanetSprites(const std::string& filename) {
    for (int layer = 0; layer < 2; layer++) {
        auto sprite = Sprite::create(filename);
        if (!sprite) return;
        // Scale to cover the screen
        sprite->setScaleX(_visibleSize.width  / sprite->getContentSize().width);
        sprite->setScaleY(_visibleSize.height / sprite->getContentSize().height);
        // Anchor bottom-left so positioning is straightforward
        sprite->setAnchorPoint(Vec2(0, 0));
        sprite->setPosition(Vec2(0, static_cast<float>(layer) * _visibleSize.height));
        sprite->setTag(100 + layer);
        addChild(sprite, -10);
    }
}

// Overlay scrolling star dots on top of the planet image.
void GameScene::addStarOverlay(int count, Color4F starColor) {
    for (int layer = 0; layer < 2; layer++) {
        auto stars = DrawNode::create();
        stars->setTag(200 + layer);
        stars->setPositionY(static_cast<float>(layer) * _visibleSize.height);
        for (int i = 0; i < count; i++) {
            float x = CCRANDOM_0_1() * _visibleSize.width;
            float y = CCRANDOM_0_1() * _visibleSize.height;
            float r = 0.5f + CCRANDOM_0_1() * 1.4f;
            float b = 0.35f + CCRANDOM_0_1() * 0.65f;
            stars->drawDot(Vec2(x, y), r,
                Color4F(starColor.r * b, starColor.g * b, starColor.b * b, 0.75f));
        }
        addChild(stars, -8);
    }
}

void GameScene::setupEarthBg() {
    loadPlanetSprites("res/bg_earth.jpg");
    addStarOverlay(50, Color4F(0.85f, 0.90f, 1.0f, 1.0f));
}

void GameScene::setupMoonBg() {
    loadPlanetSprites("res/bg_moon.jpg");
    addStarOverlay(70, Color4F(1.0f, 1.0f, 1.0f, 1.0f));
}

void GameScene::setupMarsBg() {
    loadPlanetSprites("res/bg_mars.jpg");
    addStarOverlay(45, Color4F(1.0f, 0.80f, 0.50f, 1.0f));
}

// ── UI & Input ────────────────────────────────────────────────────────────────

void GameScene::setupUI() {
    _scoreLabel = Label::createWithSystemFont("Score: 0", "Arial", 20);
    _scoreLabel->setAnchorPoint(Vec2(0, 0.5f));
    _scoreLabel->setPosition(Vec2(8, _visibleSize.height - 22));
    _scoreLabel->setTextColor(Color4B::WHITE);
    addChild(_scoreLabel, 10);

    _livesLabel = Label::createWithSystemFont("Lives: 3", "Arial", 20);
    _livesLabel->setAnchorPoint(Vec2(1, 0.5f));
    _livesLabel->setPosition(Vec2(_visibleSize.width - 8, _visibleSize.height - 22));
    _livesLabel->setTextColor(Color4B(255, 100, 100, 255));
    addChild(_livesLabel, 10);

    _weaponLabel = Label::createWithSystemFont("", "Arial", 16);
    _weaponLabel->setAnchorPoint(Vec2(0.5f, 0));
    _weaponLabel->setPosition(Vec2(_visibleSize.width * 0.5f, 8));
    _weaponLabel->setTextColor(Color4B(100, 255, 220, 255));
    addChild(_weaponLabel, 10);
}

void GameScene::setupInput() {
    auto listener = EventListenerKeyboard::create();
    listener->onKeyPressed = [this](EventKeyboard::KeyCode key, Event*) {
        _keysHeld.insert(key);
    };
    listener->onKeyReleased = [this](EventKeyboard::KeyCode key, Event*) {
        _keysHeld.erase(key);
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

// ── Update ────────────────────────────────────────────────────────────────────

void GameScene::update(float dt) {
    if (_gameOver) return;

    updateBackground(dt);

    Vec2 dir;
    using KC = EventKeyboard::KeyCode;
    if (_keysHeld.count(KC::KEY_LEFT_ARROW)  || _keysHeld.count(KC::KEY_A)) dir.x -= 1;
    if (_keysHeld.count(KC::KEY_RIGHT_ARROW) || _keysHeld.count(KC::KEY_D)) dir.x += 1;
    if (_keysHeld.count(KC::KEY_UP_ARROW)    || _keysHeld.count(KC::KEY_W)) dir.y += 1;
    if (_keysHeld.count(KC::KEY_DOWN_ARROW)  || _keysHeld.count(KC::KEY_S)) dir.y -= 1;
    if (dir.length() > 0) dir.normalize();
    _player->setMoveInput(dir);

    Vec2 playerPos = _player->getPosition();
    for (auto enemy : _enemies) {
        if (!enemy->isDead()) enemy->setPlayerPosition(playerPos);
    }

    _spawnTimer += dt;
    float spawnInterval = std::max(0.75f, _baseSpawnInterval - _score / 6000.0f);
    if (_spawnTimer >= spawnInterval) {
        _spawnTimer = 0;
        spawnEnemy();
    }

    checkCollisions();
    cleanupDeadObjects();
    updateUI();
}

void GameScene::updateBackground(float dt) {
    // Planet image layers scroll slowly
    float bgSpeed = (_sceneType == SceneType::MOON) ? 18.0f :
                    (_sceneType == SceneType::MARS) ? 35.0f : 26.0f;
    // Star overlay layers scroll a bit faster (parallax)
    float starSpeed = bgSpeed * 1.6f;

    for (int i = 0; i < 2; i++) {
        auto bg = getChildByTag(100 + i);
        if (bg) {
            bg->setPositionY(bg->getPositionY() - bgSpeed * dt);
            if (bg->getPositionY() < -_visibleSize.height)
                bg->setPositionY(bg->getPositionY() + _visibleSize.height * 2);
        }
        auto stars = getChildByTag(200 + i);
        if (stars) {
            stars->setPositionY(stars->getPositionY() - starSpeed * dt);
            if (stars->getPositionY() < -_visibleSize.height)
                stars->setPositionY(stars->getPositionY() + _visibleSize.height * 2);
        }
    }
}

// ── Gameplay ──────────────────────────────────────────────────────────────────

void GameScene::spawnEnemy() {
    float roll = CCRANDOM_0_1();
    EnemyType type;
    if (_score < 1000) {
        type = EnemyType::SMALL;
    } else if (_score < 3000) {
        type = (roll < 0.58f) ? EnemyType::SMALL : EnemyType::MEDIUM;
    } else {
        if      (roll < 0.38f) type = EnemyType::SMALL;
        else if (roll < 0.72f) type = EnemyType::MEDIUM;
        else                   type = EnemyType::LARGE;
    }

    float margin = 55;
    float x = margin + CCRANDOM_0_1() * (_visibleSize.width - 2 * margin);
    float y = _visibleSize.height + 45;

    auto enemy = Enemy::create(type, Vec2(x, y));
    enemy->setShootCallback([this](Vec2 from, Vec2 vel) { onEnemyShoot(from, vel); });
    enemy->setPlayerPosition(_player->getPosition());
    addChild(enemy, 3);
    _enemies.push_back(enemy);
}

void GameScene::onPlayerShoot(Vec2 pos, WeaponType weapon) {
    auto makeBullet = [this](Vec2 startPos, Vec2 vel) {
        auto b = Bullet::create(BulletOwner::PLAYER, vel, startPos);
        addChild(b, 4);
        _playerBullets.push_back(b);
    };
    switch (weapon) {
        case WeaponType::SINGLE: makeBullet(pos, Vec2(0, 460)); break;
        case WeaponType::DOUBLE:
            makeBullet(pos + Vec2(-14, 0), Vec2(0, 460));
            makeBullet(pos + Vec2(14, 0),  Vec2(0, 460));
            break;
        case WeaponType::TRIPLE:
            makeBullet(pos,                Vec2(0,   460));
            makeBullet(pos + Vec2(-16, 0), Vec2(-85, 445));
            makeBullet(pos + Vec2(16, 0),  Vec2(85,  445));
            break;
    }
}

void GameScene::onEnemyShoot(Vec2 from, Vec2 vel) {
    auto b = Bullet::create(BulletOwner::ENEMY, vel, from);
    addChild(b, 4);
    _enemyBullets.push_back(b);
}

void GameScene::checkCollisions() {
    Rect playerBox = _player->getBoundingBox();

    for (auto bullet : _playerBullets) {
        if (!bullet->isActive()) continue;
        Rect bb = bullet->getBoundingBox();
        for (auto enemy : _enemies) {
            if (enemy->isDead()) continue;
            if (!bb.intersectsRect(enemy->getBoundingBox())) continue;
            bullet->deactivate();
            if (enemy->takeDamage()) {
                addScore(enemy->getScoreValue());
                spawnExplosion(enemy->getPosition(),
                               enemy->getType() == EnemyType::LARGE  ? 42.0f :
                               enemy->getType() == EnemyType::MEDIUM ? 26.0f : 15.0f);
                float dropChance = (enemy->getType() == EnemyType::SMALL)  ? 0.12f :
                                   (enemy->getType() == EnemyType::MEDIUM) ? 0.35f : 0.60f;
                if (CCRANDOM_0_1() < dropChance) {
                    float r = CCRANDOM_0_1();
                    PowerUpType pt = (r < 0.25f) ? PowerUpType::DOUBLE_SHOT :
                                    (r < 0.40f) ? PowerUpType::TRIPLE_SHOT  :
                                    (r < 0.62f) ? PowerUpType::SPEED_BOOST  :
                                    (r < 0.82f) ? PowerUpType::SHIELD : PowerUpType::BOMB;
                    auto item = PowerUp::create(pt, enemy->getPosition());
                    addChild(item, 4);
                    _powerUps.push_back(item);
                }
            }
            break;
        }
    }

    if (!_player->isInvincible()) {
        for (auto bullet : _enemyBullets) {
            if (!bullet->isActive()) continue;
            if (!bullet->getBoundingBox().intersectsRect(playerBox)) continue;
            bullet->deactivate();
            _player->takeDamage();
            if (!_player->isAlive()) { triggerGameOver(); return; }
            playerBox = _player->getBoundingBox();
        }
    }

    if (!_player->isInvincible()) {
        for (auto enemy : _enemies) {
            if (enemy->isDead()) continue;
            if (!enemy->getBoundingBox().intersectsRect(playerBox)) continue;
            enemy->kill();
            spawnExplosion(enemy->getPosition(), 20.0f);
            _player->takeDamage();
            if (!_player->isAlive()) { triggerGameOver(); return; }
            playerBox = _player->getBoundingBox();
        }
    }

    for (auto item : _powerUps) {
        if (item->isCollected()) continue;
        if (!item->getBoundingBox().intersectsRect(playerBox)) continue;
        item->collect();
        applyPowerUp(item->getType());
    }
}

void GameScene::cleanupDeadObjects() {
    auto removeIf = [this](auto& vec, auto pred) {
        vec.erase(std::remove_if(vec.begin(), vec.end(), [&](auto* obj) {
            if (pred(obj)) { removeChild(obj); return true; }
            return false;
        }), vec.end());
    };
    removeIf(_enemies,       [](Enemy* e)   { return e->isDead() || e->getPositionY() < -65; });
    removeIf(_playerBullets, [](Bullet* b)  { return !b->isActive(); });
    removeIf(_enemyBullets,  [](Bullet* b)  { return !b->isActive(); });
    removeIf(_powerUps,      [](PowerUp* p) { return p->isCollected(); });
}

void GameScene::applyPowerUp(PowerUpType type) {
    switch (type) {
        case PowerUpType::DOUBLE_SHOT: _player->applyDoubleShot(9.0f); showMessage("DOUBLE FIRE!", Color4B(0,230,230,255)); break;
        case PowerUpType::TRIPLE_SHOT: _player->applyTripleShot(9.0f); showMessage("TRIPLE FIRE!", Color4B(230,0,230,255)); break;
        case PowerUpType::SPEED_BOOST: _player->applySpeedBoost(7.0f); showMessage("SPEED UP!",    Color4B(255,240,0,255)); break;
        case PowerUpType::SHIELD:      _player->applyShield();          showMessage("SHIELD!",      Color4B(40,255,80,255)); break;
        case PowerUpType::BOMB:        triggerBomb();                   showMessage("BOMB!",         Color4B(255,110,0,255)); break;
    }
}

void GameScene::triggerBomb() {
    for (auto enemy : _enemies) {
        if (!enemy->isDead()) {
            addScore(enemy->getScoreValue() / 2);
            spawnExplosion(enemy->getPosition(),
                           enemy->getType() == EnemyType::LARGE  ? 42.0f :
                           enemy->getType() == EnemyType::MEDIUM ? 26.0f : 15.0f);
            enemy->kill();
        }
    }
    for (auto b : _enemyBullets) b->deactivate();
    auto flash = DrawNode::create();
    flash->drawSolidRect(Vec2(0,0), Vec2(_visibleSize.width, _visibleSize.height),
                         Color4F(1.0f, 0.48f, 0.0f, 0.55f));
    addChild(flash, 20);
    flash->runAction(Sequence::create(FadeOut::create(0.45f), RemoveSelf::create(), nullptr));
}

void GameScene::addScore(int pts) { _score += pts; }

void GameScene::updateUI() {
    _scoreLabel->setString("Score: " + std::to_string(_score));
    _livesLabel->setString("Lives: "  + std::to_string(_player->getLives()));
    std::string wStr;
    switch (_player->getWeaponType()) {
        case WeaponType::DOUBLE: wStr = "[DOUBLE FIRE]"; break;
        case WeaponType::TRIPLE: wStr = "[TRIPLE FIRE]"; break;
        default: break;
    }
    _weaponLabel->setString(wStr);
}

void GameScene::showMessage(const std::string& msg, Color4B color) {
    auto label = Label::createWithSystemFont(msg, "Arial", 26);
    label->setTextColor(color);
    label->setPosition(Vec2(_visibleSize.width * 0.5f, _visibleSize.height * 0.5f));
    addChild(label, 15);
    label->runAction(Sequence::create(
        MoveBy::create(1.1f, Vec2(0, 65)),
        FadeOut::create(0.3f),
        RemoveSelf::create(),
        nullptr
    ));
}

void GameScene::spawnExplosion(Vec2 pos, float radius) {
    auto expl = DrawNode::create();
    expl->setPosition(pos);
    expl->drawSolidCircle(Vec2::ZERO, radius,         0.0f, 20, Color4F(1.0f, 0.8f, 0.15f, 0.92f));
    expl->drawSolidCircle(Vec2::ZERO, radius * 0.55f, 0.0f, 16, Color4F(1.0f, 0.4f, 0.0f,  0.85f));
    addChild(expl, 6);
    expl->runAction(Sequence::create(
        ScaleTo::create(0.18f, 1.8f),
        FadeOut::create(0.12f),
        RemoveSelf::create(),
        nullptr
    ));
}

void GameScene::triggerGameOver() {
    if (_gameOver) return;
    _gameOver = true;

    int finalScore  = _score;
    SceneType stype = _sceneType;

    auto flash = DrawNode::create();
    flash->drawSolidRect(Vec2(0,0), Vec2(_visibleSize.width, _visibleSize.height),
                         Color4F(1.0f, 0.0f, 0.0f, 0.45f));
    addChild(flash, 20);
    flash->runAction(Sequence::create(
        FadeOut::create(0.4f),
        DelayTime::create(0.6f),
        CallFunc::create([finalScore, stype]() {
            Director::getInstance()->replaceScene(
                TransitionFade::create(0.5f, GameOverScene::createScene(finalScore, stype))
            );
        }),
        nullptr
    ));
}
