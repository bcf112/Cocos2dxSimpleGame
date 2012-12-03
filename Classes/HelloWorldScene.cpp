#include "HelloWorldScene.h"

using namespace cocos2d;


CCScene* HelloWorld::scene()
{
	CCScene * scene = NULL;
	do 
	{
		// 'scene' is an autorelease object
		scene = CCScene::create();
		CC_BREAK_IF(! scene);

		// 'layer' is an autorelease object
		HelloWorld *layer = HelloWorld::create();
		CC_BREAK_IF(! layer);

		// add layer as a child to scene
		scene->addChild(layer);
	} while (0);

	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
	bool bRet = false;
	do 
	{
		//////////////////////////////////////////////////////////////////////////
		// super init first
		//////////////////////////////////////////////////////////////////////////

		CC_BREAK_IF(! CCLayerColor::initWithColor(ccc4(255,255,255,255)));

		//////////////////////////////////////////////////////////////////////////
		// add your codes below...
		//////////////////////////////////////////////////////////////////////////

		// 1. Add a menu item with "X" image, which is clicked to quit the program.

		// Create a "close" menu item with close icon, it's an auto release object.
		CCMenuItemImage *pCloseItem = CCMenuItemImage::create(
			"CloseNormal.png",
			"CloseSelected.png",
			this,
			menu_selector(HelloWorld::menuCloseCallback));
		CC_BREAK_IF(! pCloseItem);

		// Place the menu item bottom-right conner.
		pCloseItem->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width - 20, 20));

		// Create a menu with the "close" menu item, it's an auto release object.
		CCMenu* pMenu = CCMenu::create(pCloseItem, NULL);
		pMenu->setPosition(CCPointZero);
		CC_BREAK_IF(! pMenu);

		// Add the menu to HelloWorld layer as a child layer.
		this->addChild(pMenu, 1);

		CCSize winSize=CCDirector::sharedDirector()->getWinSize();
		CCSprite *player=CCSprite::create("Player.png", CCRectMake(0, 0, 27, 40));
		player->setPosition(ccp(player->getContentSize().width/2, winSize.height/2));
		this->addChild(player);


		bRet = true;
	} while (0);

	this->schedule(schedule_selector(HelloWorld::gameLogic), 1.0);
	this->schedule(schedule_selector(HelloWorld::checkCollision));
	this->setTouchEnabled(true);
	
	_targets=new CCArray();
	_projectiles=new CCArray();

	return bRet;
}


HelloWorld::~HelloWorld()
{
	if(_targets)
	{
		_targets->release();
		_targets=NULL;
	}

	if(_projectiles)
	{
		_projectiles->release();
		_projectiles=NULL;
	}

	//cpp don't need to call super dealloc
	//virtual destructor will do this
}


void HelloWorld::menuCloseCallback(CCObject* pSender)
{
	// "close" menu item clicked
	CCDirector::sharedDirector()->end();
}

void HelloWorld::addTarget()
{
	CCSprite *target=CCSprite::create("Target.png", CCRectMake(0,0,27,40));

	//Determine where to spawn the target along the Y axis
	CCSize winSize=CCDirector::sharedDirector()->getWinSize();
	int minY=target->getContentSize().height/2;
	int maxY=winSize.height-target->getContentSize().height/2;

	int rangeY=maxY-minY;
	int actualY=(rand()%rangeY)+minY;

	//Create the target slightly off-screen along the right edge,
	//and along a random position along the Y axis as calculated

	target->setPosition(
		ccp(winSize.width+(target->getContentSize().width/2), actualY));
	this->addChild(target);
	target->setTag(1);
	_targets->addObject(target);

	//Determine speed of the target
	int minDuration=(int)2.0;
	int maxDuration=(int)4.0;
	int rangeDuration=maxDuration-minDuration;
	int actualDuration=(rand()%rangeDuration+minDuration);

	//Create the actions
	CCFiniteTimeAction *actionMove=CCMoveTo::create((float)actualDuration, ccp(0-target->getContentSize().width/2, actualY));
	CCFiniteTimeAction *actionMoveDone=CCCallFuncN::create(this,
		callfuncN_selector(HelloWorld::spriteMoveFinished));
	target->runAction(CCSequence::createWithTwoActions(actionMove, actionMoveDone));
}

void HelloWorld::spriteMoveFinished(CCNode *sender)
{
	CCSprite *sprite=(CCSprite *)sender;
	this->removeChild(sprite, true);

	if(sprite->getTag()==1)
	{
		_targets->removeObject(sprite);
	}
	else if(sprite->getTag()==2)
	{
		_projectiles->removeObject(sprite);
	}
}

void HelloWorld::gameLogic(float dt)
{
	this->addTarget();
}

void HelloWorld::ccTouchesEnded(CCSet *touches, CCEvent *event)
{
	//Choose one of the touches to work with
	CCTouch *touch=(CCTouch*)(touches->anyObject());
	CCPoint location=touch->getLocationInView();
	location=CCDirector::sharedDirector()->convertToGL(location);

	//Set up initial location of projectile
	CCSize winSize=CCDirector::sharedDirector()->getWinSize();
	CCSprite *projectile=CCSprite::create("Projectile.png");
	projectile->setPosition(ccp(20,winSize.height/2));

	//Determinie offset of location to projectile
	int offX = location.x - projectile->getPosition().x;
	int offY = location.y - projectile->getPosition().y;

	//Bail out if we are shooting down or backwards
	if(offX<=0)
		return;

	//Ok to add now - we've double checked position
	this->addChild(projectile);
	projectile->setTag(2);
	_projectiles->addObject(projectile);

	//Determine where we wish to shoot the projectile to
	int realX = winSize.width + (projectile->getContentSize().width/2);
	float ratio = (float)offY / (float)offX;
	int realY = (realX * ratio) + projectile->getPosition().y;
	CCPoint realDest = ccp(realX, realY);

	//Determine the lenght of how far we're shooting
	int offRealX = realX - projectile->getPosition().x;
	int offRealY = realY - projectile->getPosition().y;
	float length = sqrtf((offRealX * offRealY) + (offRealY * offRealY));
	float velocity = 480/1; //480pixels/1sec
	float realMoveDuration = length / velocity;

	//Move projectile to actual endpoint
	projectile->runAction(CCSequence::create(
		CCMoveTo::create(realMoveDuration, realDest),
		CCCallFuncN::create(this, 
		callfuncN_selector(HelloWorld::spriteMoveFinished)),NULL));
}

void HelloWorld::checkCollision(float dt)
{
	CCArray *projectilesToDelete=CCArray::create();

	CCObject *it;
    CCObject *jt;

	CCARRAY_FOREACH(_projectiles, it)
	{
		CCSprite *projectile=(CCSprite *)it;
		//CCRect projectileRect=projectile->boundingBox();
		CCRect projectileRect=CCRectMake(projectile->getPosition().x - (projectile->getContentSize().width/2), projectile->getPosition().y - (projectile->getContentSize().height/2), projectile->getContentSize().width, projectile->getContentSize().height);

		CCArray *targetsToDelete=CCArray::create();

		CCARRAY_FOREACH(_targets, jt)
		{
			CCSprite *target=(CCSprite *)jt;
			//CCRect targetRect=target->boundingBox();
			CCRect targetRect=CCRectMake(target->getPosition().x - (target->getContentSize().width/2), target->getPosition().y - (target->getContentSize().height/2), target->getContentSize().width, target->getContentSize().height);

			if(projectileRect.intersectsRect(targetRect))
			{
				targetsToDelete->addObject(target);
			}
		}

		CCARRAY_FOREACH(targetsToDelete, jt)
		{
			CCSprite *target=(CCSprite *)jt;
			_targets->removeObject(target);
			this->removeChild(target, true);
		}

		if(targetsToDelete->count() > 0)
		{
			projectilesToDelete->addObject(projectile);
		}
		targetsToDelete->release();
	}

	CCARRAY_FOREACH(projectilesToDelete, it)
	{
		CCSprite *projectile=(CCSprite *)it;
		_projectiles->removeObject(projectile);
		this->removeChild(projectile, true);
	}
	projectilesToDelete->release();
}



