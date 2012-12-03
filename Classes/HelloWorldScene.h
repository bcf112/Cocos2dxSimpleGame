#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

#include "Box2D/Box2D.h"

#include "SimpleAudioEngine.h"

class HelloWorld : public cocos2d::CCLayerColor
{
public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  

    // there's no 'id' in cpp, so we recommand to return the exactly class pointer
    static cocos2d::CCScene* scene();
    
    // a selector callback
    void menuCloseCallback(CCObject* pSender);


	void addTarget();
	void spriteMoveFinished(CCNode *sender);
	void gameLogic(float dt);
	void ccTouchesEnded(cocos2d::CCSet *touches, cocos2d::CCEvent *event);
	void checkCollision(float dt);

	~HelloWorld();

    // implement the "static node()" method manually
    CREATE_FUNC(HelloWorld);

protected:
	cocos2d::CCArray *_targets;
	cocos2d::CCArray *_projectiles;
};

#endif  // __HELLOWORLD_SCENE_H__