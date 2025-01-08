#include "HelloWorldScene.h"
#include "ui/CocosGUI.h"

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

bool HelloWorld::init() {
    if (!Layer::init()) {
        return false;
    }

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // Initialize game state
    score = 0;
    gameRunning = true;
    obstacleSpawnTime = 2.0f;
    timeSinceLastSpawn = 0;

    // Load high score
    loadHighScore();

    // Create player
    player = Player::create("icons/000.png");  // Remplacez par votre sprite
    player->setPosition(Vec2(visibleSize.width * 0.2f, 100));
    this->addChild(player);

    // Score labels
    scoreLabel = Label::createWithTTF("Score: 0", "fonts/arial.ttf", 24);
    scoreLabel->setPosition(Vec2(visibleSize.width * 0.5f, visibleSize.height * 0.9f));
    scoreLabel->setTextColor(Color4B::MAGENTA);
    this->addChild(scoreLabel);

    highScoreLabel = Label::createWithTTF("High Score: " + std::to_string(highScore), "fonts/arial.ttf", 24);
    highScoreLabel->setPosition(Vec2(visibleSize.width * 0.8f, visibleSize.height * 0.9f));
    highScoreLabel->setTextColor(Color4B::GREEN);
    this->addChild(highScoreLabel);

    // add "HelloWorld" splash screen"
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

    timeSinceLastSpawn += dt;
    if (timeSinceLastSpawn >= obstacleSpawnTime) {
        spawnObstacle();
        timeSinceLastSpawn = 0;
    }
}

void HelloWorld::spawnObstacle() {
    Size visibleSize = Director::getInstance()->getVisibleSize();

    auto obstacle = Obstacle::create("icons/Runic Dagger.png");  // Remplacez par votre sprite
    obstacle->setPosition(Vec2(visibleSize.width + obstacle->getContentSize().width / 2, 100));
    obstacle->startMoving(300.0f);
    this->addChild(obstacle);
}

bool HelloWorld::onContactBegin(PhysicsContact& contact) {
    auto nodeA = contact.getShapeA()->getBody()->getNode();
    auto nodeB = contact.getShapeB()->getBody()->getNode();

    if ((nodeA->getTag() == 1 && nodeB->getTag() == 2) ||
        (nodeA->getTag() == 2 && nodeB->getTag() == 1)) {
        gameOver();
    }

    return true;
}

void HelloWorld::gameOver() {
    gameRunning = false;

    if (score > highScore) {
        highScore = score;
        saveHighScore();
        highScoreLabel->setString("High Score: " + std::to_string(highScore));
    }

    // Game over menu
    auto gameOverLabel = Label::createWithTTF("Game Over!", "fonts/arial.ttf", 48);
    gameOverLabel->setPosition(Director::getInstance()->getVisibleSize() / 2);
    this->addChild(gameOverLabel);

    auto restartButton = ui::Button::create("icons/Character1_face4.png");  // Remplacez par votre sprite
    restartButton->setPosition(Vec2(gameOverLabel->getPositionX(),
        gameOverLabel->getPositionY() - 100));
    restartButton->addClickEventListener([this](Ref*) {
        restartGame();
        });
    this->addChild(restartButton);
}

void HelloWorld::restartGame() {
    Director::getInstance()->replaceScene(HelloWorld::createScene());
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
    }
}