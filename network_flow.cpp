// Letter Dice (Network Flow)
// Fatima Bowers
// 4/25/22
//Description- Program is given a dice file with a list of dice strings and a word file with a list of words. Find if each word can be spelled by matching each letter with a different dice. If the word can be spelled, print the order of dice used to spell the word.

#include <iostream>
#include <cstdio>
#include <vector>
#include <fstream>
#include <queue>

using namespace std;

class Node{
	public:
		int ID; //Nodes are numbered, order: source, dice, letters, sink

		char word_letter; 
		string name;

		vector <bool> dice_letters;
		vector <class Edge *> adj;
		class Edge *backedge;

		bool visited; //for BFS
		int order; //the dice number that corresponds with the letter, for final results
};

class Edge{
	public:
		Node *to;
		Node *from;
		Edge *reverse;

		string name;
		int original;
		int residual;
};     

class Graph{
	public:
		Graph(char **argv);
		~Graph();

		//Using Edmonds-Karp algorithm
		bool BFS();
		int Solve();

		//Creating the graph
		void Create_Dice(ifstream &df);
		void Create_Word(string line);
		void Delete_Half();

		Node *New_Node(const string &s);
		Edge *New_Edge(Node *n1, Node *n2, bool rev);

		vector <Edge *> Path; //for BFS

		//Holds all Nodes and edges
		vector <Node *> Nodes;
		vector <Edge *> Edges;

		//Holds vector sizes after only source and dice are added to graph
		int minNodes;
		int minEdges;

		string word;
};

int main(int argc, char **argv)
{
	Graph *G;

	if(argc != 3){
		cout << "usage: worddice dice-file word-list\n";
		return 1;
	}

	G = new Graph(argv);

	delete G;
	return 0;
}

bool Graph::BFS()
	//find shortest path from source to sink
{
	size_t i;
	Node *n, *curr, *source, *sink;
	Edge *e;
	queue <Node *> q;

	//reset backedges and visited values
	for(i = 0; i < Nodes.size(); i++){
		n = Nodes[i];
		n->backedge = NULL;
		n->visited = false;
	}

	source = Nodes[0];
	sink = Nodes[Nodes.size()-1];

	//push source onto the queue
	q.push(source); 
	source->visited = true;

	while(!q.empty()){

		curr = q.front();
		q.pop();

		//create path when reached the end of graph
		if(curr == sink){
			while(curr != source){

				//if the curr node is a letter
				if(curr->ID >= minNodes && curr != sink){

					//set the order variable - the dice number that matches
					curr->order = curr->backedge->from->ID - 1;
				}

				Path.push_back(curr->backedge);
				curr = curr->backedge->from;
			}
			return true;
		}

		//iterate through the adj vector of curr node		
		for(i = 0; i < curr->adj.size(); i++){
			e = curr->adj[i];
			n = e->to;

			//follows paths that have not been used before
			if(n->visited == false && e->original == 1){
				q.push(n);
				n->backedge = e;
				n->visited = true;
			}
		}
	}
	return false;
}

int Graph::Solve()
	//check if graph can be solved
{
	size_t i;
	Edge *e;

	//for all paths found using breadth-first search
	while(BFS()){

		//each edge in the path
		for(i = 0; i < Path.size(); i++){
			e = Path[i];

			//change original and residual values since the edge has been used
			e->original = 0;
			e->residual = 1;

			e->reverse->original = 1;
			e->reverse->residual = 0;
		}

		Path.clear();
	}

	//check if each letter has a match
	for(i = minNodes; i < Nodes.size()-1; i++){

		//edge from letter to sink
		e = Nodes[i]->adj[0];

		//there was not a path with this letter, cannot spell word
		if(e->residual == 0) return 0;
	}

	//each letter had a residual of 1 - there is a path through all letters
	return 1;
}

Edge *Graph::New_Edge(Node *n1, Node *n2, bool rev)
	//set edge variable
{
	string en;
	Edge *e;

	e = new Edge;
	Edges.push_back(e);

	//set name
	e->name = n1->name + "->" + n2->name;

	//set original and residual based on whether this is an edge or reverse edge
	rev ? e->original = 0 : e->original = 1;
	rev ? e->residual = 1 : e->residual = 0;

	e->from = n1;
	e->to = n2;
	e->reverse = NULL;

	return e;
}

Node *Graph::New_Node(const string &s)
	//set node variables
{
	Node *n;
	int i;

	n = new Node;
	Nodes.push_back(n);

	//set id and name
	n->ID = Nodes.size()-1;
	n->name = s;

	//create dice letters vector, set element true for letters in the dice string
	n->dice_letters.resize(256, false);
	for(i = 0; i < (int)s.size(); i++) n->dice_letters[s[i]] = true;

	//initialize variables
	n->word_letter = '-';
	n->backedge = NULL;
	n->visited = false;
	n->order = -1;

	return n;
}

void Graph::Delete_Half()
	//removes half the graph - letter and sink nodes and corresponding edges
{
	int i;
	Edge *e;

	//iterate through edges vector
	for(i = 0; i < (int)Edges.size(); i++){
		e = Edges[i];

		//delete letter nodes and clear adj vectors for dice nodes
		if(i > 0 && i < (int)Nodes.size()){
			if(i >= minNodes) delete Nodes[i];
			else Nodes[i]->adj.clear();
		}

		//delete letter edges
		if(i >= minEdges) delete Edges[i];

		//reset original and residual values
		e->original = 1;
		e->residual = 0;
	}

	//resize vectors to remove letters and sink nodes and edges
	Nodes.resize(minNodes);
	Edges.resize(minEdges);
}

void Graph::Create_Dice(ifstream &df)
	//adds dice and source nodes and edges to the graph
{
	Node *n1, *n2;
	Edge *e, *r;
	string line;

	//create source node
	n1 = New_Node("SOURCE");

	//read in dice from dice file
	while(getline(df, line)){

		//create a dice node
		n2 = New_Node(line);

		//create an edge and reverse edge between source and dice node
		//add edges to adj vectors
		e = New_Edge(n1, n2, false);
		n1->adj.push_back(e);


		r = New_Edge(n2, n1, true);
		n2->adj.push_back(r); 

		e->reverse = r;
		r->reverse = e;
	}

	//save vector sizes for delete half function
	minNodes = Nodes.size();
	minEdges = Edges.size();
}

void Graph::Create_Word(string line)
	//adds letters and sink nodes and edges to the graph
{

	int i, j;
	string s;
	Node *n1, *n2, *n;
	Edge *e, *r;

	//create letter nodes for the word
	for(i = 0; i < (int)line.size(); i++){
		s = line[i];

		//create node and set word letter variable
		n1 = New_Node(s);
		n1->word_letter = line[i];
	}

	//create sink node
	n2 = New_Node("SINK");

	//iterate through letter nodes
	for(i = minNodes; i < (int)Nodes.size()-1; i++){
		n1 = Nodes[i];

		//create an edge and reverse edge between sink and letter nodes
		e = New_Edge(n1, n2, false);
		n1->adj.push_back(e);

		r = New_Edge(n2, n1, true);
		n2->adj.push_back(r);

		e->reverse = r;
		r->reverse = e;

		//iterate through dice, checking for matching letters
		for(j = 1; j < minNodes; j++){
			n = Nodes[j];

			//if the letter is also found in the dice, create edges between the two
			if(n->dice_letters[n1->word_letter] == true){

				e = New_Edge(n, n1, false);
				n->adj.push_back(e);

				r = New_Edge(n1, n, true);
				n1->adj.push_back(r);

				e->reverse = r;
				r->reverse = e;

			}
		}
	}
}

Graph::Graph(char **argv)
{
	size_t i;
	string line;
	ifstream df;
	ifstream wf;

	//open dice and word files
	df.open(argv[1]);
	wf.open(argv[2]);

	//creating the source and dice for the graph
	Create_Dice(df);

	//iterate through each word in the word file
	while(getline(wf, line)){
		word = line;

		//create the sink and letters for the graph
		Create_Word(line);

		//check if each letter in the word can be matched with a unique dice
		if(Solve()){

			//print the order of the dice used to spell the word
			for(i = minNodes; i < Nodes.size()-1; i++){
				cout << Nodes[i]->order;
				if(i != Nodes.size()-2) cout << ",";
			}
			cout << ": " << word << endl;
		}
		else cout << "Cannot spell " << word << endl;

		//delete half of the graph, removing sink nodes, letter nodes, and corresponding edges
		Delete_Half();
	}

	//close dice and word files
	df.close();
	wf.close();
}

Graph::~Graph()
	//destructor -- free memory
{
	size_t i;

	for(i = 0; i < Nodes.size(); i++) delete Nodes[i];
	for(i = 0; i < Edges.size(); i++) delete Edges[i];
}
