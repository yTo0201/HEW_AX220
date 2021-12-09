#include "Collision.h"

bool AABBCollision(XMFLOAT3 aPos, XMFLOAT3 bPos, XMFLOAT3 aSize, XMFLOAT3 bSize)
{
	aSize = XMFLOAT3(aSize.x / 2, aSize.y / 2, aSize.z / 2);
	bSize = XMFLOAT3(bSize.x / 2, bSize.y / 2, bSize.z / 2);

	if ((aPos.x - aSize.x <= bPos.x + bSize.x) && (bPos.x - bSize.x <= aPos.x + aSize.x) &&
		(aPos.y - aSize.y <= bPos.y + bSize.y) && (bPos.y - bSize.y <= aPos.y + aSize.y) &&
		(aPos.z - aSize.z <= bPos.z + bSize.z) && (bPos.z - bSize.z <= aPos.z + aSize.z))
	{
		// ÚG‚µ‚Ä‚¢‚é
		return true;
	}
	// ÚG‚µ‚Ä‚¢‚È‚¢
	return false;
}