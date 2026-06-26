#pragma once
#include "cocos2d.h"
#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"
#include "PowerUp.h"
#include <vector>
#include <set>

class GameScene : public cocos2d::Scene {
public:
    static cocos2d::Scene* createScene();
    bool init() override;
    void update(float dt) override;
    CREATE_FUNC(GameScene);

private:
    Player* _player = nullptr;
    std::vector<Enemy*>   _enemies;
    std::vector<Bullet*>  _playerBullets;
    std::vector<Bullet*>  _enemyBullets;
    std::vector<PowerUp*> _powerUps;

    cocos2d::Label* _scoreLabel = nullptr;
    cocos2d::Label* _livesLabel = nullptr;
    cocos2d::Label* _weaponLabel = nullptr;

    int   _score = 0;
    float _spawnTimer = 0.0f;
    float _baseSpawnInterval = 2.0f;
    bool  _gameOver = false;

    std::set<cocos2d::EventKeyboard::KeyCode> _keysHeld;

    cocos2d::Size _visibleSize;
    cocos2d::Vec2 _origin;

    void setupBackground();
    void setupUI();
    void setupInput();
    void updateBackground(float dt);
    void spawnEnemy();
    void onPlayerShoot(cocos2d::Vec2 pos, WeaponType weapon);
    void onEnemyShoot(cocos2d::Vec2 from, cocos2d::Vec2 vel);
    void checkCollisions();
    void cleanupDeadObjects();
    void applyPowerUp(PowerUpType type);
    void addScore(int pts);
    void updateUI();
    void showMessage(const std::string& msg, cocos2d::Color4B color);
    void spawnExplosion(cocos2d::Vec2 pos, float radius);
    void triggerGameOver();
    void triggerBomb();
};
