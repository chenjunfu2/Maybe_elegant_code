#include <queue>
#include <unordered_set>
#include <stdio.h>

//�Ź���
class NineSquares
{
private:
	unsigned long long ullData;//�ڴ沼�֣�����0~9�����~��λ
	int iZeroPos;
public:
	NineSquares(void) :ullData(0), iZeroPos(-1)
	{}

	NineSquares(const unsigned char ucSquares[9])
	{
		Set(ucSquares);
	}

	NineSquares(const NineSquares &_NineSquares) :ullData(_NineSquares.ullData), iZeroPos(_NineSquares.iZeroPos)
	{}

	~NineSquares(void) = default;

	explicit operator unsigned long long(void) const
	{
		return ullData;
	}

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

	void Get(unsigned char ucSquares[9]) const
	{
		auto ullTemp = ullData;
		for (int i = 0; i < 9; ++i)
		{
			ucSquares[i] = ullTemp & 0b1111;
			ullTemp >>= 4;
		}
	}

	int GetZeroPos(void) const
	{
		return iZeroPos;
	}

	bool operator==(const NineSquares &_NineSquares) const
	{
		return ullData == _NineSquares.ullData;
	}

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

	unsigned char Get(int iPos) const
	{
		return (unsigned char)((ullData >> (4 * iPos)) & 0b1111);
	}

	void Set(int iPos, unsigned char ucNew)
	{
		//�����Ӧλ��
		auto ullZero = (unsigned long long)0b1111;
		ullZero <<= 4 * iPos;
		ullZero = ~ullZero;
		ullData &= ullZero;

		//���ö�Ӧλ��
		auto ullNew = (unsigned long long)(ucNew & 0b1111);
		ullNew <<= 4 * iPos;
		ullData |= ullNew;
	}

	enum Direction :int
	{
		up = 0,
		dn,
		lf,
		rg,
	};


	bool MoveToZeroBlock(Direction enDirection)
	{
		int iX = iZeroPos % 3;
		int iY = iZeroPos / 3;

		//�������ұ任
		const constexpr struct
		{
			int iXConver;//x�任
			int iYConver;//y�任
		}stConver[4] =
		{
			{ 0,-1},	//��
			{ 0, 1},	//��
			{-1, 0},	//��
			{ 1, 0},	//��
		};

		iX += stConver[enDirection].iXConver;
		iY += stConver[enDirection].iYConver;

		if (iX < 0 || iY < 0 || iX >= 3 || iY >= 3)
		{
			return false;
		}

		//��ȡ�µ�0λ�ú�ֵ
		int iNewZeroPos = iY * 3 + iX;
		unsigned char ucVal = Get(iNewZeroPos);

		//��������λ�õ�ֵ
		Set(iNewZeroPos, 0);
		Set(iZeroPos, ucVal);

		//����0λ������
		iZeroPos = iNewZeroPos;

		return true;
	}
};


struct Node
{
	NineSquares csNineSquares{};//�Ź���
	long lG = 0;//��û�ɱ�G
	long lH = 0;//Ԥ���ɱ�H
	long lF = 0;//�ܳɱ�F

	Node *pParent = nullptr;//���ڵ�
};

struct NodeHash
{
	size_t operator()(const Node *pNode) const
	{
		return std::hash<unsigned long long>()((unsigned long long)pNode->csNineSquares);
	}
};
struct NodeEqual
{
	bool operator()(const Node *pNodeLeft, const Node *pNodeRight) const
	{
		return pNodeLeft->csNineSquares == pNodeRight->csNineSquares;
	}
};

struct NodeCmp
{
	bool operator()(const Node *pNodeLeft, const Node *pNodeRight) const
	{
		return pNodeLeft->lF > pNodeRight->lF;
	}
};

class Clear :public std::priority_queue<Node *, std::deque<Node *>, NodeCmp>
{
public:
	std::deque<Node *> GetData(void)
	{
		return c;
	}
};

long long AStarSearch(const NineSquares &csBeg, const NineSquares &csEnd)
{
	std::priority_queue<Node *, std::deque<Node *>, NodeCmp> OpenList;//���ڻ����С��ֵ��
	std::unordered_multiset<Node *, NodeHash, NodeEqual> CloseList;//���ڿ����ж��Ƿ���չ��

	//���úÿ�ʼ�ڵ����open�б�ȴ���չ
	Node *pBeg = new Node{ csBeg };
	pBeg->lH = csBeg.MatchingDegree(csEnd);
	pBeg->lG = 0;
	pBeg->lF = pBeg->lG + pBeg->lH;
	OpenList.push(pBeg);

	Node *pFind = nullptr;
	do
	{
		//ֻҪ�����б��нڵ㣬������������С���ڵ�
		Node *pCurrent = OpenList.top();
		OpenList.pop();

		if (pCurrent->csNineSquares == csEnd)//�Ѿ��ҵ���
		{
			pFind = pCurrent;
			break;
		}

		//�����ǰ���Ѿ�����չ���������õ�
		if (CloseList.find(pCurrent) != CloseList.end())
		{
			//��������ΪʲôҪ��������⣺��Ϊclose��ֻͨ���ڲ��ľŹ���ȷ���Ƿ���������
			//��ʵ���ϲ���Ľڵ�ָ����ܲ�ͬ��������ֱ�Ӷ����ᵼ�º����ڴ�й©��
			//ͬʱ��Ϊ��ͬ��ַ��ָ���ڲ��ľŹ����п��ܶ���close����˵��ͬ������ʹ��multiset
			CloseList.insert(pCurrent);
			continue;
		}

		//��ʼ��չ�˽ڵ㣬����close��Ȼ����4��������չ���ƶ���
		CloseList.insert(pCurrent);
		for (int i = NineSquares::up; i <= NineSquares::rg; ++i)//���������ƶ����
		{
			//�����ƶ��������������
			NineSquares csNew = pCurrent->csNineSquares;
			if (!csNew.MoveToZeroBlock((NineSquares::Direction)i))
			{
				continue;
			}

			//�Ѿ���չ���������õ�
			if (Node csTest{ csNew }; CloseList.find(&csTest) != CloseList.end())
			{
				continue;
			}

			//���������Ǵ���ɹ���չ�ĵ㣬������۲���֯���ṹ�������뿪���б�
			Node *pNew = new Node{ csNew };
			pNew->lH = csNew.MatchingDegree(csEnd);
			pNew->lG = pCurrent->lG + 1;
			pNew->lF = pNew->lG + pNew->lH;
			pNew->pParent = pCurrent;
			OpenList.push(pNew);
		}


	} while (!OpenList.empty());

	//�Ѿ��ҵ�Ŀ�꣬�������ṹ��������������·������
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
		i = -1;
	}

	//������Դ
	auto csDeque = ((Clear *)&OpenList)->GetData();//͵���࣬�Ѷ��汣����Ա͵�����ͷ�
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