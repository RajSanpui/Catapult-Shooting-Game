//
// Created by rsanpui on 6/27/2020.
//

#ifndef PROJ_ANDROID_DEBUGLAYER_H
#define PROJ_ANDROID_DEBUGLAYER_H

#include "GLES-Render.h"
#include "Box2D/Box2D.h"
class DebugLayer : public cocos2d::Layer
{
private:
    GLESDebugDraw* _debugDraw;
    b2World *w;

public:

    virtual bool init();
    DebugLayer(b2World*);
    ~DebugLayer();
    virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4& transform, uint32_t flags);
};
#endif //PROJ_ANDROID_DEBUGLAYER_H
