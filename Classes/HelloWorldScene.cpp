
//
// Created by rsanpui on 6/27/2020.
//

#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "GLES-Render.h"
#include "DebugLayer.h"
#include "MyContactListener.h"
#include "../cocos2d/external/Box2D/include/Box2D/Dynamics/b2Body.h"
#include "../cocos2d/external/Box2D/include/Box2D/Dynamics/b2Fixture.h"
#include "../cocos2d/external/bullet/include/bullet/BulletDynamics/Character/btKinematicCharacterController.h"
#include "../cocos2d/cocos/math/Vec2.h"
#include "../cocos2d/external/Box2D/include/Box2D/Common/b2Math.h"

#define PTM_RATIO 32.0
#define FLOOR_HEIGHT 62.0f

USING_NS_CC;


Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object

    return HelloWorld::create();
    //return scene;
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));

    float x = origin.x + visibleSize.width - closeItem->getContentSize().width/2;
    float y = origin.y + closeItem->getContentSize().height/2;
    closeItem->setPosition(Vec2(x,y));


    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label

    //auto background = DrawNode::create();
    //background->drawSolidRect(origin, visibleSize, Color4F(0.6,0.6,0.6,1.0));
    //this->addChild(background, -1);

    auto backgroundSprite = Sprite::create("beach.png");
    backgroundSprite -> setPosition(origin.x + visibleSize.width / 2 , origin.y + visibleSize.height/2 + 35);
    backgroundSprite->setScale(0.6f);
    this->addChild(backgroundSprite, -10);

    releasingArm = false;
    mouseJoint = NULL;

    b2Vec2 gravity;
    gravity.Set(0.0f, -10.0f);
    m_world = new b2World(gravity);
    m_world->SetContinuousPhysics(true);

    m_debugLayer = new DebugLayer(m_world);
    this->addChild(m_debugLayer, 9999);

    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(0, 0);
    groundBody = m_world->CreateBody(&groundBodyDef);

    CCPoint myPosition = getPosition();
    CCSize screenSize = CCDirector::sharedDirector()->getWinSize();

    float widthInMeters = screenSize.width*1.0f / PTM_RATIO;
    float maxHeightInMeters = screenSize.height / PTM_RATIO;
    float minHeightInMeters = FLOOR_HEIGHT / PTM_RATIO;
    b2Vec2 lowerLeftCorner = b2Vec2(0, minHeightInMeters);
    b2Vec2 lowerRightCorner = b2Vec2(widthInMeters, minHeightInMeters);
    b2Vec2 upperLeftCorner = b2Vec2(0, maxHeightInMeters);
    b2Vec2 upperRightCorner = b2Vec2(widthInMeters, maxHeightInMeters);

    // Define the static container body, which will provide the collisions at screen borders.
    b2BodyDef screenBorderDef;
    screenBorderDef.position.Set(0, 0);
    b2Body* screenBorderBody = m_world->CreateBody(&screenBorderDef);
    b2EdgeShape screenBorderShape;

    // Create fixtures for the four borders (the border shape is re-used)
    screenBorderShape.Set(lowerLeftCorner, lowerRightCorner);
    screenBorderBody->CreateFixture(&screenBorderShape, 0);
    screenBorderShape.Set(lowerRightCorner, upperRightCorner);
    screenBorderBody->CreateFixture(&screenBorderShape, 0);
    screenBorderShape.Set(upperRightCorner, upperLeftCorner);
    screenBorderBody->CreateFixture(&screenBorderShape, 0);
    screenBorderShape.Set(upperLeftCorner, lowerLeftCorner);
    screenBorderBody->CreateFixture(&screenBorderShape, 0);


    initTouch();

    b2CircleShape circleShape;
    circleShape.m_p.Set(0, 0); //position, relative to body position
    circleShape.m_radius = 26.0/PTM_RATIO;
    //circleShape.m_radius = 1; //radius
    catapult = Sprite::create("catapult.png");
    this->addChild(catapult);


    // Ball2 initialization
    b2BodyDef armBodyDef;
    armBodyDef.type = b2_dynamicBody;
    armBodyDef.linearDamping = 1;
    armBodyDef.angularDamping = 1;
    //armBodyDef.position.Set(225.0f/PTM_RATIO,(FLOOR_HEIGHT+150.0f)/PTM_RATIO);
    armBodyDef.position.Set(100.0f/PTM_RATIO,(FLOOR_HEIGHT + 65.0f)/PTM_RATIO);
    armBodyDef.userData = catapult;
    armBody = m_world->CreateBody(&armBodyDef);
    armBody->SetGravityScale(0);

    b2PolygonShape armBox;
    b2FixtureDef armBoxDef;
    armBoxDef.shape = &armBox;
    armBoxDef.density = 0.3F;
    armBox.SetAsBox(22.0f/PTM_RATIO, 50.0f/PTM_RATIO);
    armFixture = armBody->CreateFixture(&armBoxDef);

    b2RevoluteJointDef armJointDef;

    //armJointDef.Initialize(groundBody,armBody,b2Vec2(225.0f/PTM_RATIO,(FLOOR_HEIGHT + 100)/PTM_RATIO));
    armJointDef.Initialize(groundBody,armBody,b2Vec2(100.0f/PTM_RATIO,(FLOOR_HEIGHT + 10)/PTM_RATIO));
    armJointDef.enableMotor = true;
    armJointDef.enableLimit = true;
    armJointDef.motorSpeed = -1260;
    armJointDef.lowerAngle = CC_DEGREES_TO_RADIANS(0);
    armJointDef.upperAngle = CC_DEGREES_TO_RADIANS(75);
    armJointDef.maxMotorTorque = 4800;

    armJoint = (b2RevoluteJoint*)m_world->CreateJoint(&armJointDef);
    bullets = new std::vector<b2Body*>;
    createBullets(7);
    this->attachBullet();

    attachTarget(385.0f, 165.0f);
    attachTarget(385.0f, 75.0f);
    attachTarget(275.0f, 150.0f);
    schedule( schedule_selector(HelloWorld::tick) );

    return true;
}



void HelloWorld::initTouch()
{
    auto listener = EventListenerTouchOneByOne::create();

    listener -> onTouchBegan = CC_CALLBACK_2(HelloWorld::ccTouchesBegan, this);
    listener -> onTouchMoved = CC_CALLBACK_2(HelloWorld::ccTouchesMoved, this);
    listener -> onTouchEnded = CC_CALLBACK_2(HelloWorld::ccTouchesEnded, this);

    _eventDispatcher -> addEventListenerWithSceneGraphPriority(listener, this);
}

void HelloWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);
    
}

bool HelloWorld::ccTouchesBegan(Touch* touch, Event* evento)
{
    //if(mouseJoint != NULL) return true;

    CCPoint location = touch->getLocationInView();
    CCPoint convertedLocation = CCDirector::sharedDirector()->convertToGL(location);


    b2Vec2 locationWorld = b2Vec2(convertedLocation.x/PTM_RATIO,convertedLocation.y/PTM_RATIO);

    if(locationWorld.x<armBody->GetWorldCenter().x + 50.0f/PTM_RATIO)
    {
        b2MouseJointDef md;
        md.bodyA = groundBody;
        md.bodyB = armBody;
        md.target = locationWorld;
        md.maxForce = 2000;

        mouseJoint = (b2MouseJoint*)m_world->CreateJoint(&md);
    }
    return true;
}

void HelloWorld::ccTouchesMoved(Touch* touch, Event* evento)
{
    if(mouseJoint == NULL) return;

    CCPoint location = touch->getLocationInView();
    CCPoint convertedLocation = CCDirector::sharedDirector()->convertToGL(location);

    b2Vec2 locationWorld = b2Vec2(convertedLocation.x/PTM_RATIO,convertedLocation.y/PTM_RATIO);
    mouseJoint->SetTarget(locationWorld);

}

void HelloWorld::ccTouchesEnded(Touch* touch, Event* evento)
{
    if(mouseJoint != NULL)
    {
        if(armJoint->GetJointAngle()>=CC_DEGREES_TO_RADIANS(5))
        {
            releasingArm = true;
        }
        m_world->DestroyJoint(mouseJoint);
        mouseJoint = NULL;
    }
}

void HelloWorld::resetBullet()
{
    attachBullet();
}

void HelloWorld::attachTarget(float xCor, float yCor)
{
    auto target = Sprite::create("grass.png");
    target->setScaleX(0.3);
    target->setScaleY(0.05);
    this->addChild(target);

    // Ball2 initialization
    b2BodyDef targetBodyDef;
    targetBodyDef.type = b2_staticBody;

    targetBodyDef.position.Set(xCor/PTM_RATIO,(FLOOR_HEIGHT + yCor)/PTM_RATIO);
    targetBodyDef.userData = target;
    b2Body *targetBody = m_world->CreateBody(&targetBodyDef);
    targetBody->SetGravityScale(0);

    b2PolygonShape targetBox;
    b2FixtureDef targetBoxDef;
    targetBoxDef.shape = &targetBox;
    targetBoxDef.density = 0.3F;
    targetBox.SetAsBox(43.0f/PTM_RATIO, 3.0f/PTM_RATIO);
    b2Fixture *targetFixture = targetBody->CreateFixture(&targetBoxDef);

    // Add bottle
    auto bottle = Sprite::create("bottle_2.png");
    //bottle->setScale(0.4);
    //target->setScaleY(0.2);
    this->addChild(bottle);

    b2BodyDef bottleBodyDef;
    bottleBodyDef.type = b2_dynamicBody; //this will be a dynamic body
    bottleBodyDef.position.Set(xCor/PTM_RATIO,(FLOOR_HEIGHT + yCor + 5)/PTM_RATIO); //a little to the left
    bottleBodyDef.userData = bottle;

    b2Body* bottleBody = m_world->CreateBody(&bottleBodyDef);

    b2CircleShape circleShape;
    circleShape.m_p.Set(0, 0); //position, relative to body position
    circleShape.m_radius = 17.0/PTM_RATIO;; // This will make the balls bounce one on top of another

    b2FixtureDef bottleFixtureDef;
    bottleFixtureDef.shape = &circleShape; //this is a pointer to the shape above
    bottleFixtureDef.density = 1.0f;
    bottleFixtureDef.friction = 0.2f;
    bottleFixtureDef.restitution = 0.0f;
    bottleBody->CreateFixture(&bottleFixtureDef); //add a fixture to the body
}

void HelloWorld::tick(float dt)
{
    m_world->Step(dt, 10, 10);

    for(b2Body *b = m_world->GetBodyList(); b; b=b->GetNext()) {
        if (b->GetUserData() != NULL) {
            CCSprite *ballData = (CCSprite *)b->GetUserData();
            ballData->setPosition( CCPointMake( b->GetPosition().x * PTM_RATIO, b->GetPosition().y * PTM_RATIO) );
            ballData->setRotation( -1 * CC_RADIANS_TO_DEGREES(b->GetAngle()) );

        }
    }

    // Arm is being released.
    if (releasingArm && bulletJoint)
    {
        // Check if the arm reached the end so we can return the limits
        if (armJoint->GetJointAngle() <= CC_DEGREES_TO_RADIANS(10))
        {
            releasingArm = false;

            // Destroy joint so the bullet will be free
            m_world->DestroyJoint(bulletJoint);

            runAction(CCSequence::create(CCDelayTime::create(3.0f),
                    CCCallFunc::create(this, callfunc_selector(HelloWorld::resetBullet)),NULL));
        }
    }
}


void HelloWorld::createBullets(int count)
{
    currentBullet = 0;
    float pos = 15.0f;

    if (count > 0)
    {
        // delta is the spacing between corns
        // 62 is the position o the screen where we want the corns to start appearing
        // 165 is the position on the screen where we want the corns to stop appearing
        // 30 is the size of the corn
        float delta = (count > 1)?((165.0f - 62.0f - 30.0f) / (count - 1)):0.0f;

        bullets->reserve(count);
        for (int i=0; i<count; i++, pos+=delta)
        {
            // Create the bullet
            //
            CCSprite *sprite = CCSprite::create("basketball.png");
            addChild(sprite, 1);

            b2BodyDef bulletBodyDef;
            bulletBodyDef.type = b2_dynamicBody;
            bulletBodyDef.bullet = true;
            //bulletBodyDef.position.Set(pos/PTM_RATIO,(FLOOR_HEIGHT+10.0f)/PTM_RATIO);
            bulletBodyDef.position.Set(pos/PTM_RATIO,(FLOOR_HEIGHT+150.0f)/PTM_RATIO);
            bulletBodyDef.userData = sprite;
            b2Body *bullet = m_world->CreateBody(&bulletBodyDef);
            bullet->SetActive(false);

            b2CircleShape circle;
            circle.m_radius = 8.0/PTM_RATIO;

            b2FixtureDef ballShapeDef;
            ballShapeDef.shape = &circle;
            ballShapeDef.density = 0.8f;
            ballShapeDef.restitution = 0.2f;
            ballShapeDef.friction = 0.99f;
            bullet->CreateFixture(&ballShapeDef);

            bullets->push_back(bullet);
        }
    }
}

bool HelloWorld::attachBullet()
{
    if(currentBullet < bullets->capacity())
    {

        //bulletBody = (b2Body*)[[bullets objectAtIndex:currentBullet++] pointerValue];
        bulletBody = bullets->at(currentBullet++);
        bulletBody->SetTransform(b2Vec2(100.0f/PTM_RATIO,(100.0f+FLOOR_HEIGHT)/PTM_RATIO), 0.0f);
        bulletBody->SetActive(true);

        b2WeldJointDef weldJointDef;
        weldJointDef.Initialize(bulletBody, armBody, b2Vec2(240.0f/PTM_RATIO,(155.0f+FLOOR_HEIGHT)/PTM_RATIO));
        weldJointDef.collideConnected = false;

        bulletJoint = (b2WeldJoint*)m_world->CreateJoint(&weldJointDef);
        return true;
    }
    return false;
}