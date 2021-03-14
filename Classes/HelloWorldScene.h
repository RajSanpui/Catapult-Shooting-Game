
//
// Created by rsanpui on 6/27/2020.
//

#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "Box2D/Box2D.h"
#include "GLES-Render.h"
#include "MyContactListener.h"
#include "DebugLayer.h"

enum class PhysicsCategory {
    None = 0,
    Bomba = (1 << 0),    // 1
    Box = (1 << 1), // 2
    All = PhysicsCategory::Bomba | PhysicsCategory::Box // 3
};

class HelloWorld : public cocos2d::Scene
//class HelloWorld : public cocos2d::CCLayer
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    b2World* m_world;

    void tick(float dt);
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);

    cocos2d::Mat4 _modelViewMV;

    b2Body* groundBody;
    b2Body* dynamicBody;
    cocos2d::Sprite *boulder, *catapult;
    b2FixtureDef boulderfixture;
    b2BodyDef boulderdef;

    b2Fixture *armFixture;
    b2Body *armBody;
    b2MouseJoint *mouseJoint;
    b2RevoluteJoint* armJoint;
    MyContactListener *contactListener;
    bool releasingArm;



    //MyContactListener *contactListener;
    void touch(cocos2d::Point location);

    // implement the "static create()" method manually
    void initTouch();
    void ccTouchesMoved(cocos2d::Touch* touch, cocos2d::Event* evento);
    void ccTouchesEnded(cocos2d::Touch* touch, cocos2d::Event* evento);
    bool ccTouchesBegan(cocos2d::Touch* touch, cocos2d::Event* evento);

    void attachTarget(float xCor, float yCor);

    void createBullets(int count);
    bool attachBullet();
    void resetBullet();
    std::vector<b2Body*> *bullets;
    int currentBullet;

    b2Body *bulletBody;
    b2WeldJoint *bulletJoint;

    DebugLayer *m_debugLayer;

    CREATE_FUNC(HelloWorld);

    //GLESDebugDraw m_debugDraw;
};

#endif // __HELLOWORLD_SCENE_H__
