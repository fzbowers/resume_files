// The Maze Solver
// Fatima Bowers
// 4/4/22
// Description- Solve a maze given on standard input by employing depth first search. Maze files must contain a "ROWS r COLS c" line, and can optionally contain "WALL index index" lines.

#include <iostream>
#include <cstdio>
#include <vector>
using namespace std;

class Node {
	public:
		bool visited;
		vector <int> walls; // cells that share a wall with node
		vector <int> edges; // cells that are connected to node (do not share a wall)
};

class Graph { 
	public:
		vector <Node *> nodes; // all nodes in the graph
		vector <int> path;     // path from beginning to end of maze
		bool DFS(int index);   
		bool no_wall(Node *n, int cell); //returns true if there is no wall between n and cell
};

int main()
{
	Graph g;
	Node *n;
	string s;
	int r, c, i1, i2; //rows, cols, index 1, index 2
	int i;

	//read in and print rows and cols
	cin >> s >> r >> s >> c;
	printf("ROWS %d COLS %d\n", r, c);

	//create all nodes
	for(i = 0; i < r*c; i++){
		n = new Node;
		n->visited = false;
		g.nodes.push_back(n);
	}

	//read in walls
	while(!cin.fail()){
		cin >> s >> i1 >> i2;
		if(!cin.fail()){

			//print walls and add to walls vectors
			printf("WALL %d %d\n", i1, i2);
			g.nodes[i1]->walls.push_back(i2);
			g.nodes[i2]->walls.push_back(i1);
		}
	}

	//create edges vector for each node, check if the cells near the node are connected or if a wall separates them
	for(i = 0; i < r*c; i++){
		//current cell
		n = g.nodes[i];

		//checking cell above
		if(i/c != 0 && g.no_wall(n, i-c)) n->edges.push_back(i-c);

		//checking cell below
		if(i/c != r-1 && g.no_wall(n, i+c)) n->edges.push_back(i+c);


		//checking left cell
		if(i%c != 0 && g.no_wall(n, i-1)) n->edges.push_back(i-1);

		//checking right cell
		if(i%c != c-1 && g.no_wall(n, i+1)) n->edges.push_back(i+1);
	}

	//call depth first search on graph
	g.DFS(0);

	return 0;
}

bool Graph::DFS(int index)
{
	Node *n;
	size_t i, j;

	n = nodes[index];

	//check n's visited field. if true, then return false
	if(n->visited == true) return false;

	//set n's visited field to true
	n->visited = true;	

	//check if reached end of maze, add current node to path then return true
	if((size_t)index == nodes.size()-1){
		path.push_back(index);

		if(index == 0) printf("PATH %d\n", index);
		return true;
	}

	//for all edges of the form (n, u) call DFS on u
	for(i = 0; i < n->edges.size(); i++){

		//add index to path vector
		path.push_back(index);

		//print path if DFS returned true, meaning end of maze was reached
		if(DFS(n->edges[i])){
			for(j = 0; j < path.size(); j++){
				printf("PATH %d\n", path[j]);
			}
		}

		//if met with a dead end, remove last index from path vector
		path.pop_back();
	}
	return false;
}

bool Graph::no_wall(Node *n, int cell)
	//checks if there is a wall between the current node and given cell
	//return true if there is no wall between them
{
	size_t i;

	for(i = 0; i < n->walls.size(); i++){
		if(cell == n->walls[i]) return false;
	}

	return true;
}
