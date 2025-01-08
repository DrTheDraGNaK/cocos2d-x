#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include "Player.h"
#include "Obstacle.h"
#include "ScoreManager.h"

class GameScene : public cocos2d::Scene {
public:
    static cocos2d::Scene* createScene();
    virtual bool init();

    // Update loop
    void update(float delta) override;

    // Touch input for jumping
    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);

    CREATE_FUNC(GameScene);

private:
    Player* player;
    std::vector<Obstacle*> obstacles;
    ScoreManager* scoreManager;

    float obstacleSpawnTimer;
    float obstacleSpawnInterval;

    void spawnObstacle();
    void checkCollisions();
};

#endif // __GAME_SCENE_H__


