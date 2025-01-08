#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "cocos2d.h"

class Player : public cocos2d::Sprite {
public:
    static Player* create(const std::string& filename);
    virtual bool init(const std::string& filename);

    void jump();
    void update(float dt);
    bool isJumping() const { return jumping; }

private:
    bool jumping;
    float jumpForce;
    float gravity;
    float groundY;
};

#endif