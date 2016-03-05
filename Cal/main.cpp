#include <iostream>
#include <string>
#include <cmath>
using namespace std;

const char NUM[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.' };
const char OPERATOR[] = { '+', '-', '*', '/' };

//Prioirty: 3 for parentheses, 2 for multiplication and division, 1 for addition and substraction

//Create a double linked list to store calculation units
struct Unit {
	Unit(
		int p,
		char o,
		string s,
		double num,
		int t,
		Unit *pre = NULL,
		Unit *n = NULL) :
		priority(p), operate(o), sourceCode(s), number(num), type(t), pre(pre), next(n){}
	int priority; //Priority
	char operate; //Operator
	string sourceCode; //Source Code
	double number; //Operand
	int type; //Type: 0 for operators and 1 for numbers
	Unit *pre; //Previous unit
	Unit *next; //Next unit
};

//Create a class to represent the node of the expression tree
class Node{
public:
	Node(int p,
		char o,
		int e = 1,
		double num = 0,
		Node *ph = NULL,
		Node *pl = NULL,
		Node *pr = NULL
		) :
		priority(o), operate(o), expression(e), number(num), head(ph), left(pl), right(pr){}
	Node *head;//The head of the tree
	Node *left;//Left branch
	Node *right;//Right branch
	double getValue();
	char getOperator() const {
		return operate;
	}
	int getPriority() const {
		return priority;
	}
	int isExpression() const {
		return expression;
	}

private:
	int priority;
	char operate;
	int expression; //Check the node expression is 0 or 1
					// 1 for calculation not completed and 0 for completed
	double number; //The value of this node
};

//Calculate the value of the current node
double Node::getValue() {
	if (isExpression())
	{
		double lValue, rValue;
		lValue = left->getValue();
		rValue = right->getValue();
		expression = 0;
		char op = getOperator();
		switch (op)
		{
		case '+':
			return lValue + rValue;
		case '-':
			return lValue - rValue;
		case '*':
			return lValue * rValue;
		case '/':
			if (rValue==0)
			{
				cout << "[Error] Can't divide a number by zero!" << endl;
			}
			else
				return lValue / rValue;
		default:
			return 0;
		}
	}
	else
	{
		return number;
	}
}

//Check if the read-in character is a digit
bool isDigit(char c) {
	for (int i = 0; i < sizeof(NUM); i++) {
		if (c==NUM[i])
		{
			return true;
		}
	}	
	return false;
}

//Check if the read-in character is a operator
bool isOperator(char c) {
	for (int i = 0; i < sizeof(OPERATOR); i++)
	{
		if (c==OPERATOR[i])
		{
			return true;
		}
	}
	return false;
}

//Analyze the expression and transfer it into linked list
Unit * Analyze(string exp) {
	int priority = 0; //The current priority
	int state = -1; //The current reading state: 0 for parenthese, 1 for number, 2 for operator
	Unit *head = NULL, *p = NULL;
	int i = 0, expLen;
	expLen = exp.size();
	for ( i = 0; i < expLen; i++)
	{
		char c = exp.at(i);
		if (c=='=')
		{
			if (i!=expLen-1)
			{
				cout << "[Error]: '=' is not at the end of the expression!" << endl;
			}
			break;
		}
		if (isspace(c))
		{
			i++;
		}
		else if (c=='(')
		{
			priority += 3;
			state = 0;
		}
		else if (c==')')
		{
			priority -= 3;
			state = 0;
		}
		else if (isOperator(c))
		{
			if (head == NULL)
			{
				cout << "[Error] Operator can't at the beginning of the expression!" << endl;
			}
			Unit *temp = p; //Current Unit
			int addPri;
			if (c == '+' || c == '-')
			{
				addPri = 1;
			}
			else if (c == '*' || c == '/')
			{
				addPri = 2;
			}
			p->next = new Unit(priority + addPri, c, " ", 0, 0);
			p = p->next;
			p->pre = temp;
		}
		else
		{
			string num = "";
			while (i<expLen && (c=exp.at(i), isDigit(c)))
			{
				num += c;
				i++;
			}
			i--;
			if (head==NULL)
			{
				p = new Unit(priority, ' ', num, 0, 1);
				head = p;
			}
			else
			{
				Unit *temp = p;
 				p->next = new Unit(priority, ' ', num, 0, 1);
				p = p->next;
				p->pre = temp;
			}
		}
	}
	return head;
}

Unit * Calculate(Unit *head) {
	Unit *p = head;
	while (p!=NULL)
	{
		if (p->type == 1) {
			string temp = p->sourceCode;
			double lValue = 0;
			int lPoint = 0;
			int i = 0;
			char ch;
			while (i<temp.size() && (ch=temp.at(i), isDigit(ch)))
			{
				if (ch == '.') {
					lPoint++;
					i++;
					continue;
				}
				if (!lPoint)
				{
					lValue *= 10;
				}
				lValue += (ch - '0') * pow(10, -lPoint);
				i++;
				if (lPoint)
				{
					lPoint++;
				}
			}
			p->number = lValue;
		}
		p = p->next;
	}
	return head;
}

Node * Tree(Unit *head) {
	Node * root = NULL, *pRoot = NULL, *pBranch = NULL;
	Unit * p = head;
	int currentPrio; //Current Priority
	bool hadop = false;
	while (p!=NULL)
	{
		if (p->type == 0){ //If the current Unit is an operator 
			hadop = true;
			if (root==NULL)
			{	
				pRoot = new Node(p->priority, p->operate, 1);
				root = pRoot;
				pBranch = root;
				currentPrio = p->priority;
				pRoot->left = new Node(0, ' ', 0, p->pre->number);
				pRoot->right = new Node(0, ' ', 0, p->next->number);
			}
			else
			{
				if (p->priority<=currentPrio)
				{
					pRoot = new Node(p->priority, p->operate, 1);
					pRoot->left = root;
					pRoot->right = new Node(0, ' ', 0, p->next->number);
					root = pRoot;
					pBranch = pRoot;
				}
				else
				{
					Node *temp;
					temp = new Node(p->priority, p->operate, 1);

					pBranch->right = temp;
					temp->head = pBranch;

					pBranch = pBranch->right;
					pBranch->left = new Node(0, ' ', 0, p->pre->number);
					pBranch->right = new Node(0, ' ', 0, p->next->number);
				}
				currentPrio = p->priority;
			}
		}
		p = p->next;
	}
	if (!hadop)
	{
		root = new Node(0, ' ', 0, head->number);
	}
	return root;
}

int main() {
	string exp;
	cout << "Input your expression to calculate:" << endl;
	while (! getline(cin,exp).eof())
	{
		if (exp == "")
			continue;
		Unit * h = Analyze(exp);
		h = Calculate(h);
		Node * root = Tree(h);
		cout << "The result is: " << root->getValue() << endl;
	}
	return 0;
}