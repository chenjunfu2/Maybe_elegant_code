#include <queue>
#include <unordered_set>
#include <stdio.h>

//九宫格类
class NineSquares
{
private:
	unsigned long long ullData;//内存布局：数组0~9放入低~高位，用于存储九宫格实际数据，以压缩格式，每4bit存储一个值
	int iZeroPos;//用于指示当前空位的下标
public:
	//默认无参构造
	NineSquares(void) :ullData(0), iZeroPos(-1)
	{}

	//从uchar数组中构造
	NineSquares(const unsigned char ucSquares[9])
	{
		Set(ucSquares);
	}

	//拷贝构造函数
	NineSquares(const NineSquares &_NineSquares) :ullData(_NineSquares.ullData), iZeroPos(_NineSquares.iZeroPos)
	{}

	//默认析构函数
	~NineSquares(void) = default;

	//用于显示转换为类内部实际存储的压缩二进制数据
	explicit operator unsigned long long(void) const
	{
		return ullData;
	}

	//从uchar数组中存储的九宫格压缩到二进制格式
	void Set(const unsigned char ucSquares[9])
	{
		ullData = 0;
		iZeroPos = -1;
		for (int i = 8; i >= 0; --i)
		{
			ullData <<= 4;
			ullData |= ucSquares[i] & 0b1111;

			if (iZeroPos == -1 && ucSquares[i] == 0)
			{
				iZeroPos = i;
			}
		}
	}

	//从二进制压缩格式的九宫格释放到uchar数组中
	void Get(unsigned char ucSquares[9]) const
	{
		auto ullTemp = ullData;
		for (int i = 0; i < 9; ++i)
		{
			ucSquares[i] = ullTemp & 0b1111;
			ullTemp >>= 4;
		}
	}

	//用于获取空位（0方块）的下标
	int GetZeroPos(void) const
	{
		return iZeroPos;
	}

	//判断两个九宫格是否完全相等
	bool operator==(const NineSquares &_NineSquares) const
	{
		return ullData == _NineSquares.ullData;
	}

	//用于判断两个九宫格之间的相似性（差距）的函数，实际上返回数值位置相同的个数
	int MatchingDegree(const NineSquares &_NineSquares) const
	{
		auto ullLeft = ullData;
		auto ullRight = _NineSquares.ullData;

		int iMatchingDegree = 0;
		for (int i = 0; i < 9; ++i)
		{
			if ((ullLeft & 0b1111) == (ullRight & 0b1111))
			{
				++iMatchingDegree;
			}
			ullLeft >>= 4;
			ullRight >>= 4;
		}

		return iMatchingDegree;
	}

	//用于获取九宫格中对应位置值的函数
	unsigned char Get(int iPos) const
	{
		return (unsigned char)((ullData >> (4 * iPos)) & 0b1111);
	}

	//用于设置九宫格中对应位置值的函数
	void Set(int iPos, unsigned char ucNew)
	{
		//清零对应位置
		auto ullZero = (unsigned long long)0b1111;
		ullZero <<= 4 * iPos;
		ullZero = ~ullZero;
		ullData &= ullZero;

		//设置对应位置
		auto ullNew = (unsigned long long)(ucNew & 0b1111);
		ullNew <<= 4 * iPos;
		ullData |= ullNew;
	}

	//方位枚举
	enum Direction :int
	{
		up = 0,
		dn,
		lf,
		rg,
	};

	//移动参数指示方向上的方块到空位（0方块）上
	bool MoveToZeroBlock(Direction enDirection)
	{
		int iX = iZeroPos % 3;
		int iY = iZeroPos / 3;

		//上下左右变换
		const constexpr struct
		{
			int iXConver;//x变换
			int iYConver;//y变换
		}stConver[4] =
		{
			{ 0,-1},	//上
			{ 0, 1},	//下
			{-1, 0},	//左
			{ 1, 0},	//右
		};

		iX += stConver[enDirection].iXConver;
		iY += stConver[enDirection].iYConver;

		if (iX < 0 || iY < 0 || iX >= 3 || iY >= 3)
		{
			return false;
		}

		//获取新的0位置和值
		int iNewZeroPos = iY * 3 + iX;
		unsigned char ucVal = Get(iNewZeroPos);

		//交换两个位置的值
		Set(iNewZeroPos, 0);
		Set(iZeroPos, ucVal);

		//更新0位置坐标
		iZeroPos = iNewZeroPos;

		return true;
	}
};

//树节点
struct Node
{
	NineSquares csNineSquares{};//九宫格
	long lG = 0;//沉没成本G
	long lH = 0;//预估成本H
	long lF = 0;//总成本F

	Node *pParent = nullptr;//父节点
};

//用于对key算hash的函数
struct NodeHash
{
	size_t operator()(const Node *pNode) const
	{
		return std::hash<unsigned long long>()((unsigned long long)pNode->csNineSquares);
	}
};

//用于哈希表相等判断的函数
struct NodeEqual
{
	bool operator()(const Node *pNodeLeft, const Node *pNodeRight) const
	{
		return pNodeLeft->csNineSquares == pNodeRight->csNineSquares;
	}
};

//用于优先级队列排序的比较函数
struct NodeCmp
{
	bool operator()(const Node *pNodeLeft, const Node *pNodeRight) const
	{
		return pNodeLeft->lF > pNodeRight->lF;
	}
};

//保护成员偷家用的类
class Clear :public std::priority_queue<Node *, std::deque<Node *>, NodeCmp>
{
public:
	std::deque<Node *> &GetData(void)
	{
		return c;
	}
};

long long AStarSearch(const NineSquares &csBeg, const NineSquares &csEnd)
{
	std::priority_queue<Node *, std::deque<Node *>, NodeCmp> OpenList;//用于获得最小价值点
	std::unordered_multiset<Node *, NodeHash, NodeEqual> CloseList;//用于快速判断是否扩展过

	//设置好开始节点放入open列表等待扩展
	Node *pBeg = new Node{ csBeg };
	pBeg->lH = csBeg.MatchingDegree(csEnd);
	pBeg->lG = 0;
	pBeg->lF = pBeg->lG + pBeg->lH;
	OpenList.push(pBeg);

	Node *pFind = nullptr;
	do
	{
		//只要开放列表有节点，弹出顶部（最小）节点
		Node *pCurrent = OpenList.top();
		OpenList.pop();

		if (pCurrent->csNineSquares == csEnd)//已经找到了
		{
			pFind = pCurrent;
			break;
		}

		//如果当前点已经被扩展过，跳过该点
		if (CloseList.find(pCurrent) != CloseList.end())
		{
			//关于这里为什么要插入的问题：因为close表只通过内部的九宫格确定是否遇到过，
			//而实际上插入的节点指针可能不同，不插入直接丢弃会导致后续内存泄漏，
			//同时因为不同地址的指针内部的九宫格有可能对于close表来说相同，所以使用multiset
			CloseList.insert(pCurrent);
			continue;
		}

		//开始扩展此节点，放入close表然后向4个方向扩展（移动）
		CloseList.insert(pCurrent);
		for (int i = NineSquares::up; i <= NineSquares::rg; ++i)//遍历四种移动情况
		{
			//不能移动，跳过这种情况
			NineSquares csNew = pCurrent->csNineSquares;
			if (!csNew.MoveToZeroBlock((NineSquares::Direction)i))
			{
				continue;
			}

			//已经扩展过，跳过该点
			if (CloseList.find((Node*)&csNew) != CloseList.end())
			{
				continue;
			}

			//接下来就是处理成功扩展的点，计算代价并组织树结构，最后插入开放列表
			Node *pNew = new Node{ csNew };
			pNew->lH = csNew.MatchingDegree(csEnd);
			pNew->lG = pCurrent->lG + 1;
			pNew->lF = pNew->lG + pNew->lH;
			pNew->pParent = pCurrent;
			OpenList.push(pNew);
		}


	} while (!OpenList.empty());

	//已经找到目标，回溯树结构并计数，获得最短路径步数
	long long i = 0;
	if (pFind != nullptr)
	{
		while (pFind->pParent != nullptr)
		{
			pFind = pFind->pParent;
			++i;
		}
	}
	else
	{
		i = -1;//没找到返回-1
	}

	//清理资源
	auto &csDeque = ((Clear *)&OpenList)->GetData();//偷家类，把对面保护成员偷出来释放
	for (auto it : csDeque)
	{
		delete it;
	}

	for (auto it : CloseList)
	{
		delete it;
	}

	return i;
}


int main(void)
{
	/*
	unsigned char ucBeg[9] =
	{
		1,2,0,
		4,7,3,
		8,5,6,
	};
	unsigned char ucEnd[9] =
	{
		0,2,3,
		1,4,7,
		8,5,6,
	};

	//4
	*/
	unsigned char ucBeg[9];
	unsigned char ucEnd[9] = { 1,2,3,8,0,4,7,6,5 };

	for (int i = 0; i < 9; ++i)
	{
		ucBeg[i] = (char)getchar() - '0';
	}

	(void)printf("%lld\n", AStarSearch(ucBeg, ucEnd));

	return 0;
}