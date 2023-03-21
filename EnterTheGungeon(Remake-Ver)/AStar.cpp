#include "AStar.h"
#include "MapTool.h"

void AStar::init()
{	
	const auto& tileSize{ (*_mapToolPtr)->getTileSize() };
	_pathColor = ImageManager::makeImage("Images/Scene/Maptool/Palette/InstallationColor.bmp", {}, { 1.0f, 1.0f }, { tileSize.x, tileSize.y * InstallationColorCount }, { 1, InstallationColorCount }, false);
	_pathColor.setTransparency(80);

	_directions =
	{
		POINT_FOR_ASTAR{ 0, -1 },		// UP
		POINT_FOR_ASTAR{ -1, 0 },		// LEFT
		POINT_FOR_ASTAR{ 0, 1 },		// DOWN
		POINT_FOR_ASTAR{ 1, 0 },		// RIGHT
		POINT_FOR_ASTAR{ -1, -1 },		// UP_LEFT
		POINT_FOR_ASTAR{ -1, 1 },		// DOWN_LEFT
		POINT_FOR_ASTAR{ 1, 1 },		// DOWN_RIGHT
		POINT_FOR_ASTAR{ 1, -1 }		// UP_RIGHT
	};

	_directionCosts =
	{
		10, // UP
		10, // LEFT
		10, // DOWN
		10, // RIGHT
		14,
		14,
		14,
		14
	};
}

void AStar::renderPath(const HDC& hdc, const vector<POINT>& pathVec) const
{	
	for (const auto& pathIdx : pathVec)
	{		
		_pathColor.renderTransparentFrame(hdc, (*_mapToolPtr)->convertIndexToCalcPos(pathIdx), {});
	}
}

	// ���� �ű��
	// F = G + H
	// F = ���� ���� (���� ���� ����, ��ο� ���� �޶���)
	// G = ���������� �ش� ��ǥ���� �̵��ϴµ� ��� ��� (���� ���� ����, ��ο� ���� �޶���)
	// H = ���������� �󸶳� ������� (���� ���� ����, ����)
vector<POINT> AStar::findPath(const POINT& sIdx, const POINT& dIdx) const
{
	const POINT& startTile{ (*_mapToolPtr)->getStartTile() };
	POINT		 calcSIdx{ sIdx.x - startTile.x, sIdx.y - startTile.y };

	POINT addition{};
	if (calcSIdx.x < 0)
	{
		addition.x = -calcSIdx.x;
		calcSIdx.x = calcSIdx.x + addition.x;
	}

	if (calcSIdx.y < 0)
	{
		addition.y = -calcSIdx.y;
		calcSIdx.y = calcSIdx.y + addition.y;
	}

	POINT calcDIdx{ (dIdx.x - startTile.x) + addition.x, (dIdx.y - startTile.y) + addition.y };
	
	// ClosedList
	// close[y][x] -> (y, x)�� �湮�� �ߴ��� ����
	vector<vector<bool>> closed(100, vector<bool>(100, false));

	// best[y][x] -> ���ݱ��� (y, x)�� ���� ���� ���� ��� (���� ���� ����)
	vector<vector<int32>> best(100, vector<int32>(100, INT32_MAX));

	// �θ� ���� �뵵
	map<POINT_FOR_ASTAR, POINT_FOR_ASTAR> parent;

	// OpenList
	priority_queue<PQ_NODE, vector<PQ_NODE>, greater<PQ_NODE>> pq;

	// 1) ����(�߰�) �ý��� ����
	// - �̹� �� ���� ��θ� ã�Ҵٸ� ��ŵ
	// 2) �ڴʰ� �� ���� ��ΰ� �߰ߵ� �� ���� -> ���� ó�� �ʼ�
	// - openList���� ã�Ƽ� �����Ѵٰų�.
	// - pq���� pop�� ������ �����Ѵٰų�.

	// �ʱⰪ
	{
		const int32 g = 0;
		const int32 h = 10 * (abs(dIdx.y - sIdx.y) + abs(dIdx.x - sIdx.x));
		const int32 f = g + h;
		pq.emplace(move(PQ_NODE{ {calcSIdx.x, calcSIdx.y}, g, f }));
		best[calcSIdx.y][calcSIdx.x] = f;
		parent[calcSIdx] = { calcSIdx.x, calcSIdx.y };
	}

	while (pq.empty() == false)
	{
		// ���� ���� �ĺ��� ã�´�
		PQ_NODE calcNode = pq.top();
		pq.pop();

		// ������ ��ǥ�� ���� ��η� ã�Ƽ�
		// �� ���� ��η� ���ؼ� �̹� �湮(closed)�� ��� ��ŵ
		// [����]
		if (closed[calcNode.calcIdx.y][calcNode.calcIdx.x])
			continue;
		if (best[calcNode.calcIdx.y][calcNode.calcIdx.x] < calcNode.f)
			continue;

		// �湮
		closed[calcNode.calcIdx.y][calcNode.calcIdx.x] = true;

		// �������� ���������� �ٷ� ����
		if (POINT_FOR_ASTAR{ (calcNode.calcIdx.x - addition.x) + startTile.x, (calcNode.calcIdx.y - addition.y) + startTile.y } == dIdx)
			break;

		for (int dir = 0; dir < DirectionCount; dir++)
		{
			const POINT_FOR_ASTAR calcNIdx = { calcNode.calcIdx + _directions[dir] };
			const POINT_FOR_ASTAR nIdx = { (calcNIdx.x - addition.x) + startTile.x, (calcNIdx.y - addition.y) + startTile.y };
			if (calcNIdx.x < 0 || calcNIdx.y < 0)
				continue;

			// �� �� �ִ� ������ �´��� Ȯ��
			if ((*_mapToolPtr)->canGo( move(POINT{ nIdx.x, nIdx.y })) == false)
				continue;
			// [����] �̹� �湮�� ���̸� ��ŵ
			if (closed[calcNIdx.y][calcNIdx.x])
				continue;

			// ��� ���
			const int32 g = calcNode.g + _directionCosts[dir];
			const int32 h = 10 * (abs(dIdx.y - nIdx.y) + abs(dIdx.x - nIdx.x));
			const int32 f = g + h;

			// �ٸ� ��ο��� �� ���� ���� ã������ ��ŵ
			if (best[calcNIdx.y][calcNIdx.x] <= f)
				continue;

			// ���� ����
			best[calcNIdx.y][calcNIdx.x] = f;

			pq.emplace( move(PQ_NODE{ calcNIdx, g, f }));
			parent[calcNIdx] = calcNode.calcIdx;
		}
	}

	// �Ųٷ� �Ž��� �ö󰣴�
	vector<POINT> path;
	POINT_FOR_ASTAR revCalcIdx = calcDIdx;
	while (true)
	{		
		path.emplace_back(move(POINT{ (revCalcIdx.x - addition.x) + startTile.x, (revCalcIdx.y - addition.y) + startTile.y }));

		// �������� �ڽ��� �� �θ��̴�
		if (revCalcIdx == parent[revCalcIdx])
			break;

		revCalcIdx = parent[revCalcIdx];
	}
	//std::reverse(path.begin(), path.end());
	return path;
}