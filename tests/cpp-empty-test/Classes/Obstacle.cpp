#include "Obstacle.h"

USING_NS_CC;

Obstacle* Obstacle::create(const std::string& filename) {
    Obstacle* obstacle = new Obstacle();
    if (obstacle && obstacle->init(filename)) {
        obstacle->autorelease();
        return obstacle;
    }
    CC_SAFE_DELETE(obstacle);
    return nullptr;
}

bool Obstacle::init(const std::string& filename) {
    if (!Sprite::initWithFile(filename)) {
        return false;
    }

    auto body = PhysicsBody::createBox(this->getContentSize());
    body->setDynamic(false);
    body->setCategoryBitmask(0x02);    // Obstacle category
    body->setContactTestBitmask(0x01); // Player category
    this->setPhysicsBody(body);

    this->setTag(2); 
    moveSpeed = 300.0f;
    passed = false;

    return true;
}

void Obstacle::startMoving(float speed) {
    moveSpeed = speed;
    this->scheduleUpdate();
}

void Obstacle::update(float dt) {
    this->setPositionX(this->getPositionX() - moveSpeed * dt);

    if (this->getPositionX() < -this->getContentSize().width) {
        this->removeFromParent();
    }
}