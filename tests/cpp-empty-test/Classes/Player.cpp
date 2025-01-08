#include "Player.h"

USING_NS_CC;

Player* Player::create(const std::string& filename) {
    Player* player = new Player();
    if (player && player->init(filename)) {
        player->autorelease();
        return player;
    }
    CC_SAFE_DELETE(player);
    return nullptr;
}

bool Player::init(const std::string& filename) {
    if (!Sprite::initWithFile(filename)) {
        return false;
    }

    jumping = false;
    jumpForce = 300.0f;
    gravity = -980.0f;
    groundY = 100.0f;

    auto body = PhysicsBody::createBox(this->getContentSize());
    body->setDynamic(true);
    body->setGravityEnable(true);
    body->setCategoryBitmask(0x01);    // Player category
    body->setContactTestBitmask(0x02); // Obstacle category
    this->setPhysicsBody(body);

    this->scheduleUpdate();

    return true;
}

void Player::jump() {
    if (!jumping) {
        jumping = true;
        this->getPhysicsBody()->setVelocity(Vec2(0, jumpForce));
    }
}

void Player::update(float dt) {
    if (this->getPositionY() <= groundY) {
        this->setPositionY(groundY);
        jumping = false;
    }
}