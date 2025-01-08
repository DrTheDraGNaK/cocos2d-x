#include "HelloWorldScene.h"
#include "ui/CocosGUI.h"
#include "audio/include/AudioEngine.h"

USING_NS_CC;

Scene* HelloWorld::createScene() {
    auto scene = Scene::createWithPhysics();
    scene->getPhysicsWorld()->setGravity(Vec2(0, -980));
    scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_NONE);

    auto layer = HelloWorld::create();
    layer->SetPhysicsWorld(scene->getPhysicsWorld());
    scene->addChild(layer);

    return scene;
}

bool HelloWorld::init() 
{
    if (!Layer::init()) 
    {
        return false;
    }

    restartButton = nullptr;
    groundHeight = 100.0f;

    createGround();

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    score = 0;
    gameRunning = true;
    obstacleSpawnTime = 2.0f;
    timeSinceLastSpawn = 0;

    loadHighScore();

    if (AudioEngine::getPlayingAudioCount() == 0) {
        AudioEngine::play2d("Sound/BackGroundMusic.mp3", true); 
    }

    // Create player
    player = Player::create("icons/000.png");  
    float playerSpawnY = groundHeight + (player->getContentSize().height / 2);
    player->setPosition(Vec2(visibleSize.width * 0.2f, playerSpawnY));
    player->setTag(1);
    this->addChild(player);;

    // Score labels
    scoreLabel = Label::createWithTTF("Score: 0", "fonts/arial.ttf", 24);
    scoreLabel->setPosition(Vec2(visibleSize.width * 0.5f, visibleSize.height * 0.9f));
    scoreLabel->setTextColor(Color4B::MAGENTA);
    this->addChild(scoreLabel);

    highScoreLabel = Label::createWithTTF("High Score: " + std::to_string(highScore), "fonts/arial.ttf", 24);
    highScoreLabel->setPosition(Vec2(visibleSize.width * 0.8f, visibleSize.height * 0.9f));
    highScoreLabel->setTextColor(Color4B::GREEN);
    this->addChild(highScoreLabel);

    auto sprite = Sprite::create("icons/backgroundColorGrass.png");

    // position the sprite on the center of the screen
    sprite->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));

    // add the sprite as a child to this layer
    this->addChild(sprite, -3);

    // Contact listener
    auto contactListener = EventListenerPhysicsContact::create();
    contactListener->onContactBegin = CC_CALLBACK_1(HelloWorld::onContactBegin, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);


    // Keyboard listener
    auto keyboardListener = EventListenerKeyboard::create();
    keyboardListener->onKeyPressed = CC_CALLBACK_2(HelloWorld::onKeyPressed, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

    this->scheduleUpdate();

    return true;
}

void HelloWorld::update(float dt) {
    if (!gameRunning) return;

    obstacleSpawnTime = std::max(2.0f - (score * 0.05f), MIN_OBSTACLE_SPAWN_TIME);

    timeSinceLastSpawn += dt;
    if (timeSinceLastSpawn >= obstacleSpawnTime) {
        spawnObstacle();
        timeSinceLastSpawn = 0;
    }

    for (Node* node : this->getChildren()) {
        Obstacle* obstacle = dynamic_cast<Obstacle*>(node);
        if (obstacle && !obstacle->isPassed()) {
            if (obstacle->getPositionX() < player->getPositionX()) {
                obstacle->setPassed(true);
                score++;
                scoreLabel->setString("Score: " + std::to_string(score));
                AudioEngine::play2d("Sound/collect-points-190037.mp3"); 
            }
        }
    }
}

float HelloWorld::calculateObstacleSpeed() const {
    float speed = 200.0f + score * 10.0f;
    return std::min(speed, 600.0f); // Limiter à 600
}

void HelloWorld::spawnObstacle() {
    Size visibleSize = Director::getInstance()->getVisibleSize();

    auto obstacle = Obstacle::create("icons/Runic Dagger.png");

    float randomOffsetY = CCRANDOM_0_1() * 50;
    float obstacleSpawnY = groundHeight + (obstacle->getContentSize().height / 2) + randomOffsetY;
    obstacle->setPosition(Vec2(
        visibleSize.width + obstacle->getContentSize().width / 2,
        obstacleSpawnY
    ));

    float obstacleSpeed = calculateObstacleSpeed(); 
    obstacle->startMoving(obstacleSpeed);          
    auto rotateAction = RotateBy::create(1.0f, 360.0f); // Rotation de 360° en 1 seconde
    auto repeatRotate = RepeatForever::create(rotateAction);
    obstacle->runAction(repeatRotate);

    this->addChild(obstacle);
}

bool HelloWorld::onContactBegin(PhysicsContact& contact) {
    auto nodeA = contact.getShapeA()->getBody()->getNode();
    auto nodeB = contact.getShapeB()->getBody()->getNode();

    if ((nodeA && nodeB) &&
        ((nodeA->getTag() == 1 && nodeB->getTag() == 2) ||
            (nodeA->getTag() == 2 && nodeB->getTag() == 1))) {
        gameOver();
        return true;
    }

    return false;
}


void HelloWorld::gameOver() 
{
    if (!gameRunning) return;

    gameRunning = false;

    player->getPhysicsBody()->setVelocity(Vec2::ZERO);

    // Arrêtez tous les obstacles
    for (Node* node : this->getChildren()) {
        Obstacle* obstacle = dynamic_cast<Obstacle*>(node);
        if (obstacle) {
            obstacle->unscheduleUpdate();
        }
    }

    if (score > highScore) 
    {
        highScore = score;
        saveHighScore();
        highScoreLabel->setString("High Score: " + std::to_string(highScore));
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();

    // Game Over Label
    auto gameOverLabel = Label::createWithTTF("Game Over!", "fonts/arial.ttf", 72);
    gameOverLabel->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 + 100));
    gameOverLabel->setTextColor(Color4B::RED);
    this->addChild(gameOverLabel, 10);

    // Score Final
    auto finalScoreLabel = Label::createWithTTF("Final Score: " + std::to_string(score),
        "fonts/arial.ttf", 48);
    finalScoreLabel->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
    finalScoreLabel->setTextColor(Color4B::GREEN);
    this->addChild(finalScoreLabel, 10);


    AudioEngine::play2d("Sound/pop-sound-effect-197846.mp3");
    createRestartButton();
}

void HelloWorld::createRestartButton() {
    auto visibleSize = Director::getInstance()->getVisibleSize();


     restartButton = ui::Button::create();
     restartButton->setTitleText("Restart");
     restartButton->setTitleFontSize(48);
     restartButton->setTitleColor(Color3B::WHITE);
     restartButton->setContentSize(Size(200, 80));
     restartButton->setColor(Color3B(51, 153, 255)); 

    restartButton->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 - 100));

    restartButton->addClickEventListener([this](Ref* sender) {
        restartGame();
        });

    restartButton->runAction(
        RepeatForever::create(
            Sequence::create(
                ScaleTo::create(0.5f, 1.1f),
                ScaleTo::create(0.5f, 1.0f),
                nullptr
            )
        )
    );

    this->addChild(restartButton, 10);
}

void HelloWorld::restartGame() {
    auto scene = HelloWorld::createScene();
    Director::getInstance()->replaceScene(
        TransitionFade::create(0.5f, scene, Color3B(0, 0, 0))
    );
}

void HelloWorld::saveHighScore() {
    UserDefault::getInstance()->setIntegerForKey("highscore", highScore);
    UserDefault::getInstance()->flush();
}

void HelloWorld::loadHighScore() {
    highScore = UserDefault::getInstance()->getIntegerForKey("highscore", 0);
}

void HelloWorld::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event) {
    if (keyCode == EventKeyboard::KeyCode::KEY_SPACE && gameRunning) {
        player->jump();
        AudioEngine::play2d("Sound/retro-jump-3-236683.mp3");
    }
}


void HelloWorld::createGround() {
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // Charger l'image pour le sol
    auto groundSprite = Sprite::create("icons/4.Ground.png");
    groundSprite->setAnchorPoint(Vec2(0.5f, 0.5f)); // Par défaut, centre l'image
    groundSprite->setScaleX(visibleSize.width / groundSprite->getContentSize().width); // Ajuster la largeur
    groundSprite->setScaleY(500.0f / groundSprite->getContentSize().height); // Ajuster la hauteur

    groundSprite->setPosition(Vec2(
        visibleSize.width / 2 + origin.x,
        groundHeight  + 120.0f
    ));

    // Créer un corps physique pour le sol
    auto groundBody = PhysicsBody::createBox(
        Size(visibleSize.width, groundHeight),
        PhysicsMaterial(0.1f, 0.0f, 1.0f)
    );
    groundBody->setDynamic(false);
    groundBody->setCategoryBitmask(0x04);
    groundBody->setContactTestBitmask(0x01);
    groundSprite->setPhysicsBody(groundBody);

    // Ajouter le sprite comme enfant
    this->addChild(groundSprite);
    
}

//void HelloWorld::onExit() {
//    Layer::onExit();
//    AudioEngine::end(); // Libère les ressources audio
//}