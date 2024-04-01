#include <stdio.h>
#include <stack>
#include <ctype.h>

/*
两个栈
一个数值栈
一个运算符栈

通过优先级来运算
遇到数值压栈，遇到运算符查看运算符栈顶优先级，当前运算符优先级更高则继续压入，
否则弹出原先栈顶的计算，结果压栈再压入运算符。
括号优先级最大，内括号优先级仍然比外括号大，也就是相同优先级的括号仍然继续压栈，
而不像运算符一样相同优先级先进行运算，当前运算符为左括号则压入，为右括号则弹出，
持续运算，直到遇到左括号为止。
当遇到表达式末尾则依次弹出运算所有剩余内容，运算符栈为空时，运算结束，返回数值栈栈顶。

优先级排序（从小到大）
加减
乘除
负号
括号

*/


enum class Symbol : int
{
	nul,//-1
	//加减
	add,//0
	sub,
	//乘除
	mul,//1
	div,
	//负号
	neg,//2
	//左右括号
	lbk,//3
	rbk,
};


int GetPriority(Symbol enSymbol)
{
	constexpr int iPriority[] =
	{
		-1,
		0,0,
		1,1,
		2,
		3,3,
	};

	return iPriority[(int)enSymbol];
}

struct Input
{
	bool isVal;
	union
	{
		Symbol enSymbol;
		long long llVal;
	};
};

bool isNeg(Input &stLast)
{
	if (stLast.isVal == false && stLast.enSymbol != Symbol::rbk)//不是值并且不是右括号
	{
		return true;
	}

	return false;
}

bool GetNext(Input &stCurr, Input &stLast)
{
	stLast = stCurr;//保存为上一次的信息，然后读取新的覆盖Curr

	int ret = getchar();
	if (ret == EOF || (char)ret == '@')//截止符
	{
		return false;
	}

	//是数值
	if (isdigit(ret))
	{
		ungetc(ret, stdin);
		long long llVal;
		if (scanf("%lld", &llVal) == 1)
		{
			stCurr.isVal = true;
			stCurr.llVal = llVal;
			return true;
		}
		return false;
	}

	//否则是符号
	if (ret != -1)
	{
		char cSymbol = (char)ret;
		stCurr.isVal = false;
		switch (cSymbol)
		{
		case '+':
			stCurr.enSymbol = Symbol::add;
			break;
		case '-':
			stCurr.enSymbol = isNeg(stLast) ? Symbol::neg : Symbol::sub;
			break;
		case '*':
			stCurr.enSymbol = Symbol::mul;
			break;
		case '/':
			stCurr.enSymbol = Symbol::div;
			break;
		case '(':
			stCurr.enSymbol = Symbol::lbk;
			break;
		case ')':
			stCurr.enSymbol = Symbol::rbk;
			break;
		default:
			stCurr.enSymbol = Symbol::nul;
			return false;
			break;
		}

		return true;
	}
	
	return false;
}


bool CalculateStackTop(std::stack<long long> &stkVal, std::stack<Symbol> &stkSymbol)
{
	if (stkVal.empty() || stkSymbol.empty())
	{
		return false;
	}

	Symbol enSymbol = stkSymbol.top();
	stkSymbol.pop();

	//提前处理负号（单目运算符）
	if (enSymbol == Symbol::neg)
	{
		stkVal.top() = -stkVal.top();
		return true;
	}

	//处理其他符号（双目运算符，多一个检测）
	long long llRight = stkVal.top();
	stkVal.pop();
	//判空
	if (stkVal.empty())
	{
		return false;
	}

	//没问题后开始计算
	switch (enSymbol)
	{
	case Symbol::add:
		{
			stkVal.top() += llRight;
		}
		break;
	case Symbol::sub:
		{
			stkVal.top() -= llRight;
		}
		break;
	case Symbol::mul:
		{
			stkVal.top() *= llRight;
		}
		break;
	case Symbol::div:
		{
			stkVal.top() /= llRight;
		}
		break;
	case Symbol::neg:
		{
			//前面已经处理过，无需再次处理
		}
		break;
	case Symbol::nul:
	case Symbol::lbk:
	case Symbol::rbk:
	default:
		return false;
		break;
	}

	return true;
}


void callback(void)
{
	std::stack<long long> stkVal;
	std::stack<Symbol> stkSymbol;

	Input stCurr{ false,Symbol::nul }, stLast{ false,Symbol::nul };
	long long i = 0;
	while (GetNext(stCurr, stLast))
	{
		//printf("[%lld] ", i++);
		//值处理

		//直接压入
		if (stCurr.isVal)
		{
			stkVal.push(stCurr.llVal);
			continue;
		}
		
		//符号处理

		//右括号，一直运算直到左括号为止
		if (stCurr.enSymbol == Symbol::rbk)
		{
			if (stkSymbol.empty())
			{
				goto error;
			}

			while (stkSymbol.top() != Symbol::lbk)
			{
				if (CalculateStackTop(stkVal, stkSymbol) == false)
				{
					goto error;
				}
			}

			stkSymbol.pop();//弹出左括号
			continue;
		}

		//其他符号，判断优先级，更高则压入
		while (!stkSymbol.empty() &&//符号栈不为空才计算
				stkSymbol.top() != Symbol::lbk &&//栈顶不是左括号才能计算
				GetPriority(stCurr.enSymbol) <= GetPriority(stkSymbol.top()) &&//优先级更低或相同才能计算
				stCurr.enSymbol != Symbol::neg)//不是负号才能计算
		{
			//计算栈顶
			if (CalculateStackTop(stkVal, stkSymbol) == false)
			{
				goto error;
			}
		}

		//计算完毕，压入符号
		stkSymbol.push(stCurr.enSymbol);
	}

	//没有更多的输入了，处理完栈内剩下信息
	while (CalculateStackTop(stkVal, stkSymbol))
	{
		continue;
	}

	if (stkSymbol.empty() && stkVal.size() == 1)
	{
		//printf("\n计算结果:%lld\n", stkVal.top());
		printf("%lld", stkVal.top());
	}
	else
	{
	error:
		//printf("\n计算结果:语法错误\n");
		printf("NO");
	}

}

int main(void)
{
	//while (true)
	{
		callback();
	}

	return 0;
}

//-1+1--1+-1*-1+-(-1*-1-1+-1)-1/1=2