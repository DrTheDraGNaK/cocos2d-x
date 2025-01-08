#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "Player.h"
#include "Obstacle.h"
#include "ui/CocosGUI.h"

class HelloWorld : public cocos2d::Layer {
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    void menuCloseCallback(cocos2d::Ref* pSender);
    CREATE_FUNC(HelloWorld);

private:
    void SetPhysicsWorld(cocos2d::PhysicsWorld* world) { sceneWorld = world; }
    bool onContactBegin(cocos2d::PhysicsContact& contact);
    void update(float dt);
    float calculateObstacleSpeed() const;
    void spawnObstacle();
    void gameOver();
    void restartGame();
    void saveHighScore();
    void loadHighScore();
    void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);

    void createRestartButton();
    cocos2d::ui::Button* restartButton;

    void createGround();
    float groundHeight;

    cocos2d::PhysicsWorld* sceneWorld;
    Player* player;
    int score;
    int highScore;
    cocos2d::Label* scoreLabel;
    cocos2d::Label* highScoreLabel;
    bool gameRunning;
    float obstacleSpawnTime;
    float timeSinceLastSpawn;
};

#endif