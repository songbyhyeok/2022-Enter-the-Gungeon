#include "CollisionHandler.h"
#include "Bullet.h"
#include "MapTool.h"
#include "PlayerController.h"
#include "MonsterController.h"
#include "EffectManager.h"

unique_ptr<MapTool*>			CollisionHandler::MapToolPtr = nullptr;
unique_ptr<PlayerController*>	CollisionHandler::PlayerCtrPtr = nullptr;
unique_ptr<MonsterController*>	CollisionHandler::MonsterCtrPtr = nullptr;

bool CollisionHandler::checkWallCollision(const POINT& currPos, const POINT& userSize, const POINT& tileSize, POINT& moveCount)
{
	POINT mathSign{};
	if (moveCount.x < 0)
		mathSign.x = -1;
	else if (0 < moveCount.x)
		mathSign.x = 1;

	if (moveCount.y < 0)
		mathSign.y = -1;
	else if (0 < moveCount.y)
		mathSign.y = 1;
	
	const RECT userRelativeRect{ move(generateRect(currPos, userSize)) };
	const POINT destPos{ currPos.x + (mathSign.x * (userSize.x / 2)), currPos.y + (mathSign.y * (userSize.y / 2)) };
	const RECT destRect{ move(generateRect(destPos, tileSize)) };	
	POINT destIdx{ (*MapToolPtr)->convertPosToIndex(destPos, userSize) };

	//if (playerRect.right > destRect.left && playerRect.left < destRect.right && playerRect.top < destRect.bottom && playerRect.bottom > destRect.top)
	RECT tempRect{};
	if (IntersectRect(&tempRect, &userRelativeRect, &destRect))
	{
		if (((*MapToolPtr)->isCurrentPositionMaterialObject(destIdx)))
		{
			if (moveCount.x != 0 && moveCount.y != 0)
			{
				if (moveCount.y < 0)
					destIdx.y = destIdx.y + 1;
				else if (0 < moveCount.y)
					destIdx.y = destIdx.y - 1;
				if ((*MapToolPtr)->isCurrentPositionMaterialObject(destIdx))
				{
					if (moveCount.y < 0)
						destIdx.y = destIdx.y - 1;
					else if (0 < moveCount.y)
						destIdx.y = destIdx.y + 1;

					if (moveCount.x < 0)
						destIdx.x = destIdx.x + 1;
					else if (0 < moveCount.x)
						destIdx.x = destIdx.x - 1;

					if ((*MapToolPtr)->isCurrentPositionMaterialObject(destIdx))
						return true;

					moveCount.x = 0;
					return false;
				}

				moveCount.y = 0;
				return false;
			}

			if ((*MapToolPtr)->isGateObject(destIdx) && (*MonsterCtrPtr)->isMonstersDied((*MapToolPtr)->getCurrentUseZoneN()))
			{				
				(*MapToolPtr)->openGate();
			}

			return true;
		}
		else
		{
			// bug 2022-09-08 �ָ��� ��ġ���� ���� Ű�� ���� ��� target idx�� ��Ȯ�ϰ� ���� ���ؼ� �浹 ó���� ����� ���� ���ϴ� ����
			/*
			*	2X1
			*	XPX
			*	3X4
			*	
			*	N = �� ��и�, X = BLOCK, P = PLAYER			
			*/
			if (moveCount.x != 0 && moveCount.y != 0)
			{
				bool isBlockedBottom = false;
				// 1 or 2 case
				if (moveCount.y < 0)
				{
					// ���� �����°�?
					if (moveCount.x < 0)
						isBlockedBottom = ((*MapToolPtr)->isCurrentPositionMaterialObject({ destIdx.x + 1, destIdx.y }));
					else if (0 < moveCount.x)
						isBlockedBottom = ((*MapToolPtr)->isCurrentPositionMaterialObject({ destIdx.x - 1, destIdx.y }));

					if (isBlockedBottom)
					{
						// �� or �찡 �����°�? (��, �� or �찡 �� �����ִ�.)
						if ((*MapToolPtr)->isCurrentPositionMaterialObject({ destIdx.x, destIdx.y + 1 }))
						{
							return true;
						}
						// ���� ������.
						else 
						{
							moveCount.y = 0;
							return false;
						}
					}
					else
					{
						// �� or �찡 �����°�?
						if ((*MapToolPtr)->isCurrentPositionMaterialObject({ destIdx.x, destIdx.y + 1 }))
						{
							moveCount.x = 0;
						}

						// ������ �ʾҴٸ� ��, �� or �찡 �� �շ��ִ�.
						return false;
					}
				}
				// 3 or 4 case
				else if (moveCount.y > 0)
				{
					// �Ʒ��� �����°�?
					if (moveCount.x < 0)
						isBlockedBottom = (*MapToolPtr)->isCurrentPositionMaterialObject({ destIdx.x + 1, destIdx.y });
					else if (0 < moveCount.x)
						isBlockedBottom = (*MapToolPtr)->isCurrentPositionMaterialObject({ destIdx.x - 1, destIdx.y });

					if (isBlockedBottom)
					{
						// �� or �찡 �����°�? (�Ʒ�, �� or �찡 �� �����ִ�.)
						if ((*MapToolPtr)->isCurrentPositionMaterialObject({ destIdx.x, destIdx.y - 1 }))
						{
							return true;
						}
						// �Ʒ��� ������.
						else
						{
							moveCount.y = 0;
							return false;
						}
					}
					else
					{
						// �� or �찡 �����°�?
						if ((*MapToolPtr)->isCurrentPositionMaterialObject({ destIdx.x, destIdx.y - 1 }))
						{
							moveCount.x = 0;
						}

						// ������ �ʾҴٸ� ��, �� or �찡 �� �շ��ִ�.
						return false;
					}
				}
			}
		}
	}

	return false;
}

bool CollisionHandler::checkWallCollisionForBullet(Bullet* firedBullet)
{
	if (firedBullet == nullptr)
		assert(false);

	const auto maxMapDistance { (*MapToolPtr)->getMaxMapDistance() };

	const POINT bCenterPos{ firedBullet->getCenterPosition() };
	const POINT& bRadius{ firedBullet->getRadius() };

	bool isOverTheWall{ bCenterPos.x < 0 || maxMapDistance.x <= bCenterPos.x
		|| bCenterPos.y <= 0 || maxMapDistance.y <= bCenterPos.y };
	if (isOverTheWall)
		return isOverTheWall;

	if ((*MapToolPtr)->isCurrentPositionMaterialObject(firedBullet->getCurrIdx()))
	{
		const RECT tRect{ (*MapToolPtr)->getTileRect(firedBullet->getCurrIdx()) };
		if ((tRect.left <= bCenterPos.x && bCenterPos.x <= tRect.right) ||
			(tRect.top <= bCenterPos.y && bCenterPos.y <= tRect.bottom))
		{
			// ��������ŭ Ȯ��� Rect
			const RECT etRect
			{
				tRect.left - bRadius.x,
				tRect.top - bRadius.y,
				tRect.right + bRadius.x,
				tRect.bottom + bRadius.y
			};

			// Ȯ���� �簢�� �ȿ� ���� �߽��� ����ִ��� �ľ�
			if ((etRect.left < bCenterPos.x && bCenterPos.x < etRect.right) && (etRect.top < bCenterPos.y && bCenterPos.y < etRect.bottom))
			{
				static auto& effectMgr = EffectManager::getInstance();
				effectMgr.stopSplashingSound();
				effectMgr.playSplashingSound();

				return true;
			}
		}
		else
		{}
	}

	return false;
}

bool CollisionHandler::checkBulletCollisionToMonsters(Bullet* firedBullet)
{
	const POINT& bCenterPos{ firedBullet->getCenterPosition() };
	const POINT& bRadius{ firedBullet->getRadius() };

	for (const auto& monsterPtr : (*MonsterCtrPtr)->getMonsterPtrsVec())
	{
		if (monsterPtr->getState() == MONSTER_STATE::DEATH)
			continue;

		const RECT mRect{ move(generateRect(monsterPtr->getCurrPos(), monsterPtr->getSize())) };
		if ((mRect.left <= bCenterPos.x && bCenterPos.x <= mRect.right) ||
			(mRect.top <= bCenterPos.y && bCenterPos.y <= mRect.bottom))
		{
			// ��������ŭ Ȯ��� Rect
			const RECT eRect
			{
				mRect.left - bRadius.x,
				mRect.top - bRadius.y,
				mRect.right + bRadius.x,
				mRect.bottom + bRadius.y
			};
			
			// Ȯ���� �簢�� �ȿ� ���� �߽��� ����ִ��� �ľ�
			if ((eRect.left < bCenterPos.x && bCenterPos.x < eRect.right) && (eRect.top < bCenterPos.y && bCenterPos.y < eRect.bottom))
			{
				auto& effectMgr = EffectManager::getInstance();
				effectMgr.setHitAniSP(firedBullet->getRelativePos());
				effectMgr.playHitAni();

				(*MonsterCtrPtr)->takeMonsterDamage(monsterPtr, 1, (*MapToolPtr)->getCurrentUseZoneN());
				
				return true;
			}
		}
		else
		{
			//// �簢���� �»�� ����Ʈ�� �� �ȿ� �ִ��� �ľ�
			//if (isPointInCircle(bCenterPos, bRadius, { (float)mRect.left, (float)mRect.top }));
			//	return true;
			//// �簢���� ���ϴ� ����Ʈ�� �� �ȿ� �ִ��� �ľ�
			//if (isPointInCircle(bCenterPos, bRadius, { (float)mRect.left, (float)mRect.bottom }));
			//	return true;
			//// �簢���� ���� ����Ʈ�� �� �ȿ� �ִ��� �ľ�
			//if (isPointInCircle(bCenterPos, bRadius, { (float)mRect.right, (float)mRect.top }));
			//	return true;
			//// �簢���� ���ϴ� ����Ʈ�� �� �ȿ� �ִ��� �ľ�
			//if (isPointInCircle(bCenterPos, bRadius, { (float)mRect.right, (float)mRect.bottom }));
			//	return true;
		}
	}

	return false;
}

bool CollisionHandler::checkBulletCollisionToPlayer(Bullet* firedBullet)
{
	const POINT& bCenterPos{ firedBullet->getCenterPosition() };
	const POINT& bRadius{ firedBullet->getRadius() };


	const RECT pRect{ move(generateRect((*PlayerCtrPtr)->getCurrPos(), (*PlayerCtrPtr)->getSize())) };
	if ((pRect.left <= bCenterPos.x && bCenterPos.x <= pRect.right) ||
		(pRect.top <= bCenterPos.y && bCenterPos.y <= pRect.bottom))
	{
		// ��������ŭ Ȯ��� Rect
		const RECT eRect
		{
			pRect.left - bRadius.x,
			pRect.top - bRadius.y,
			pRect.right + bRadius.x,
			pRect.bottom + bRadius.y
		};

		// Ȯ���� �簢�� �ȿ� ���� �߽��� ����ִ��� �ľ�
		if ((eRect.left < bCenterPos.x && bCenterPos.x < eRect.right) && (eRect.top < bCenterPos.y && bCenterPos.y < eRect.bottom))
		{
			if ((*PlayerCtrPtr)->getPlayerCurrState() == PLAYER_STATE::DODGE)
				return false;

			auto& effectMgr = EffectManager::getInstance();
			effectMgr.setHitAniSP(firedBullet->getRelativePos());
			effectMgr.playHitAni();

			//(*PlayerCtrPtr)->takeDamage(1);

			return true;
		}
	}
	else
	{
		//// �簢���� �»�� ����Ʈ�� �� �ȿ� �ִ��� �ľ�
		//if (isPointInCircle(bCenterPos, bRadius, { (float)mRect.left, (float)mRect.top }));
		//	return true;
		//// �簢���� ���ϴ� ����Ʈ�� �� �ȿ� �ִ��� �ľ�
		//if (isPointInCircle(bCenterPos, bRadius, { (float)mRect.left, (float)mRect.bottom }));
		//	return true;
		//// �簢���� ���� ����Ʈ�� �� �ȿ� �ִ��� �ľ�
		//if (isPointInCircle(bCenterPos, bRadius, { (float)mRect.right, (float)mRect.top }));
		//	return true;
		//// �簢���� ���ϴ� ����Ʈ�� �� �ȿ� �ִ��� �ľ�
		//if (isPointInCircle(bCenterPos, bRadius, { (float)mRect.right, (float)mRect.bottom }));
		//	return true;
	}

	return false;
}

void CollisionHandler::release()
{
	MapToolPtr.reset();
	PlayerCtrPtr.reset();
	MonsterCtrPtr.reset();
}

// ���� ���ȿ� �ִ���?
bool CollisionHandler::isPointInCircle(const POINT& circleCenterPos, const POINT& circleRadius, const FPOINT& squarePos)
{
	// x ������
	const FPOINT deltaPos{ circleCenterPos.x - squarePos.x, circleCenterPos.y - squarePos.y };
	// ���� �߽ɰ� ������ �Ÿ�
	const double length{ sqrt(deltaPos.x * deltaPos.x + deltaPos.y * deltaPos.y) };
	// ���� �߽ɺ��� ������ �Ÿ��� ���� ���������� ũ�ٸ� �浹�� �ƴϴ�.
	if (length > circleRadius.x)
		return false;

	return true;
}
