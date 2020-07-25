#include"PandaLex.h"
#include"PandaParse.h"
#include<iostream>
bool DFS(Node* node)
{
	
	if (!node->HasChild()) {
		std::cout << node->ReturnSymbol()<<"";
		return true;
	}
	else
	{
		std::cout << node->ReturnSymbol() << ":(" ;
		for (int i = 0; i < node->ReturnSize(); i++) {
			DFS(node->At(i));
		}
		std::cout << ")";
		return true;
	}
}
int main() {
		PandaLex lex("D:\\VsProject\\PandaComplier\\Complier\\lex.lex", "D:\\VsProject\\PandaComplier\\Complier\\Src.txt");
		PandaParse parse("D:\\VsProject\\PandaComplier\\Complier\\grammer.gra", "D:\\VsProject\\PandaComplier\\Complier\\lr.lr",&lex);
		InternalNode * temp = parse.GetAST();
		DFS(temp);
		return 0;
}
