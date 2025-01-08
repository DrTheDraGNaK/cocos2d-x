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

bool Player::init(const std::string& filename) 
{
    if (!Sprite::initWithFile(filename)) 
    {
        return false;
    }

    jumping = false;
    jumpForce = 600.0f;  
    gravity = -1200.0f;

    auto body = PhysicsBody::createBox(this->getContentSize());
    body->setDynamic(true);
    body->setGravityEnable(true);
    body->setRotationEnable(false);
    body->setCategoryBitmask(0x01);
    body->setContactTestBitmask(0x02 | 0x04);

    body->setMass(1.0f);

    this->setPhysicsBody(body);

    return true;
}

void Player::setGrounded(bool grounded) {
    this->getPhysicsBody()->setGravityEnable(!grounded);
    jumping = !grounded;
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