#ifndef __OBSTACLE_H__
#define __OBSTACLE_H__

#include "cocos2d.h"

class Obstacle : public cocos2d::Sprite {
public:
    static Obstacle* create(const std::string& filename);
    virtual bool init(const std::string& filename);
    void startMoving(float speed);
    bool isPassed() const { return passed; }
    void setPassed(bool value) { passed = value; }

private:
    void update(float dt);
    float moveSpeed;
    bool passed;
};

#endif
