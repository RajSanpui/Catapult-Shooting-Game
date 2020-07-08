//
// Created by rsanpui on 6/27/2020.
//

#include "DebugLayer.h"
USING_NS_CC;

bool DebugLayer::init()
{
    if (!Layer::init())
    {
        return false;
    }

    // set up box2d ratio and debug flags
    _debugDraw = new GLESDebugDraw(32);
    w->SetDebugDraw(_debugDraw);
    uint32 flags = 0;
    flags += b2Draw::e_shapeBit;
    flags += b2Draw::e_jointBit;
    flags += b2Draw::e_centerOfMassBit;
    flags += b2Draw::e_aabbBit;
    flags += b2Draw::e_pairBit;
    _debugDraw->SetFlags(flags);

    return true;
}

void DebugLayer::draw(Renderer* renderer, const Mat4& transform, uint32_t flags)
{
    cocos2d::Layer::draw(renderer, transform, flags);

    GL::enableVertexAttribs(GL::VERTEX_ATTRIB_FLAG_POSITION);
    Director::getInstance()->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);

    w->DrawDebugData();

    Director::getInstance()->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);


}

DebugLayer::DebugLayer(b2World *world)
{
    w = world;
    init();
}

DebugLayer::~DebugLayer()
{
    delete _debugDraw;
}