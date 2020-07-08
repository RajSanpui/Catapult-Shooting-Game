//
// Created by rsanpui on 6/28/2020.
//

#ifndef PROJ_ANDROID_MYCONTACTLISTENER_H
#define PROJ_ANDROID_MYCONTACTLISTENER_H

#include "cocos2d.h"
#include "Box2D/Box2D.h"
#include <set>
#include <algorithm>

class MyContactListener : public b2ContactListener {

public:
    std::set<b2Body*>contacts;

    MyContactListener();
    ~MyContactListener();

    virtual void BeginContact(b2Contact* contact);
    virtual void EndContact(b2Contact* contact);
    virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
    virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);

};

#endif //PROJ_ANDROID_MYCONTACTLISTENER_H
