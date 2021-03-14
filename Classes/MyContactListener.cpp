//
// Created by rsanpui on 6/28/2020.
//

#include "MyContactListener.h"
#include "HelloWorldScene.h"
#include "../cocos2d/external/Box2D/include/Box2D/Dynamics/b2Fixture.h"

USING_NS_CC;

MyContactListener::MyContactListener() : contacts()
{
}

MyContactListener::~MyContactListener()
{
}

void MyContactListener::BeginContact(b2Contact* contact)
{
    b2Filter filterA = contact->GetFixtureA()->GetFilterData();
    b2Filter filterB = contact->GetFixtureB()->GetFilterData();

    CCLOG("********************** Contact Made! Fixture A: %s", contact->GetFixtureA()->GetBody()->GetUserData());
    CCLOG("********************** Contact Made! Fixture B: %s", contact->GetFixtureB()->GetBody()->GetUserData());

    if ((filterA.categoryBits == (uint) PhysicsCategory::Bomba) &&
        (filterB.categoryBits == (uint) PhysicsCategory::Bomba))
    {
        CCLOG("************* BeginContact - Collision happened ********************* \n");
        //Director::getInstance()->end();
    }
}
// https://discuss.cocos2d-x.org/t/solved-problem-with-box2d-collision-detection/22029/6
// 
void MyContactListener::EndContact(b2Contact* contact)
{
    b2Filter filterA = contact->GetFixtureA()->GetFilterData();
    b2Filter filterB = contact->GetFixtureB()->GetFilterData();

    CCLOG("********************** Contact Made! Fixture A: %s", contact->GetFixtureA()->GetBody()->GetUserData());
    CCLOG("********************** Contact Made! Fixture B: %s", contact->GetFixtureB()->GetBody()->GetUserData());

    if ((filterA.categoryBits == (uint) PhysicsCategory::Bomba) &&
        (filterB.categoryBits == (uint) PhysicsCategory::Bomba))
    {
        CCLOG("************* EndContact - Collision happened ********************* \n");
    }
}

void MyContactListener::PreSolve(b2Contact* contact,
                                 const b2Manifold* oldManifold)
{
}

void MyContactListener::PostSolve(b2Contact* contact,
                                  const b2ContactImpulse* impulse)
{
    bool isAEnemy = (contact->GetFixtureA()->GetUserData() != NULL);
    bool isBEnemy = (contact->GetFixtureB()->GetUserData() != NULL);
    if (isAEnemy || isBEnemy)
    {
        // Should the body break?
        int32 count = contact->GetManifold()->pointCount;

        float32 maxImpulse = 0.0f;
        for (int32 i = 0; i < count; ++i)
        {
            maxImpulse = b2Max(maxImpulse, impulse->normalImpulses[i]);
        }

        if (maxImpulse > 1.0f)
        {
            // Flag the enemy(ies) for breaking.
            if (isAEnemy)
                contacts.insert(contact->GetFixtureA()->GetBody());
            if (isBEnemy)
                contacts.insert(contact->GetFixtureB()->GetBody());
        }
    }
}