#include <iostream>
#include <map>
#include <cmath>
#include <cstdlib>
#include <iterator>
#include <time.h>
#include <math.h>
#include <limits>
#include <iomanip>
#include <fstream>
#include <string>
#include <graphics.h>	
#include <X11/Xlib.h>
using namespace std;
#define PI 3.14159

//Data structure of the node
class Node{
public:
	int identifier;
	float x;
	float y;
    float angle;
    map <int, float> temp_view;
    map <int, float > Neighbours;
};

//Global variable declarations
int N, K;
float sum = 0.0;
map <int, Node*> Global_Node_Table;
string TOPOLOGY;

//Distance functions
float Distance(Node* mine, Node* other){
	if( (TOPOLOGY == "B") || (TOPOLOGY == "b")){	
		if (other == Global_Node_Table[N]){
			if ((mine == Global_Node_Table[1]) || (mine == Global_Node_Table[N-1])) 
				return 1.0;
			else 
				return numeric_limits<float>::max();
		}
		else
			return pow((pow((mine->x - other->x), 2) + pow((mine->y - other->y), 2)), 0.5);
	}
	else if ((TOPOLOGY == "S") || (TOPOLOGY == "s")){
		if (other == Global_Node_Table[N]){
			return numeric_limits<float>::max();
		}
		else{
			return pow((pow((mine->x - other->x), 2) + pow((mine->y - other->y), 2)), 0.5);
		}
	}
}

//Position_Initialization
void Position_Initializations(){
	if ((TOPOLOGY == "B") || (TOPOLOGY == "b")){	
		for(int i = 1; i <N; i++){
			Node *n = new Node;
			n->identifier = i;
			float angle = (PI/2) - ((i-1)*(PI/(N-2)));
			n->angle = angle;
			n->x = cos(angle);
			n->y = sin(angle);
			Global_Node_Table.insert(pair <int, Node*> (n->identifier, n));
		}
		Node* n = new Node;
		n->identifier = N;
		n->angle = PI/2;
		n->x = cos(PI/2);
		n->y = sin(PI/2);
		Global_Node_Table.insert(pair <int, Node*> (n->identifier, n));
		//for(int i = 1; i <= N; i++){
			//cout << Global_Node_Table[i]->angle << "\n"; 
		//}
	}
	else if ((TOPOLOGY == "S") || (TOPOLOGY == "s")) {
		for(int i =1; i <=N; i++){
			Node *n = new Node;
			if (i<= (int)(N*2)/5){
				n->identifier = i;
				float angle = ( (i-1)*((5*PI)/N) );
				n->angle = angle;
				n->x = 1+ cos(angle);
				n->y = sin(angle);
			}
			else if ( (i> (int)(N*2)/5) && (i<=(3*N)/5)){
				n->identifier = i;
				float angle = ((i-((2*N)/5))*(5*PI)/(N+5));
				n->angle = angle;
				n->x = 3 + cos(angle);
				n->y = -sin(angle);
			}
			else{
				n->identifier = i;
				float angle = (PI+((i-(3*N)/5)*(5*PI)/N));
				n->angle = angle;
				n->x = 5 + cos(angle);
				n->y = sin(angle);
			}
			Global_Node_Table.insert(pair <int, Node*> (n->identifier, n));
		}
	}
}

//0 cycle 
void Network_Initialization(){
	//Taking Random K number of nodes
	Node *n;
	for(int i = 1; i <= N; i++){
		//Select one node from the list
		n = Global_Node_Table[i];
		//copy the table so that it will not main table
		map <int, Node*> Copy_Table = Global_Node_Table;
		for(int j = 1; j <= K; j++){
			int x =(rand()%N) + 1;
			while((x == i) || (Copy_Table[x] == NULL)) {x = (rand()%N) + 1;}
			//nth node other than sending node is selected
			n->Neighbours.insert(pair <int, float> (x, Distance(n, Global_Node_Table[x])));
			Copy_Table.erase(x);
		}

		map <int, float> :: iterator f;
		for(f = n->Neighbours.begin(); f != n->Neighbours.end(); f++){
    			if (f->second != numeric_limits<float>::max()) { 
    				sum += f->second;
    			}
    		}
	}
}

//Random selection of nodes
int Random_Selection(Node* n){
	map <int, float> :: iterator itr;
	itr = n->Neighbours.begin();
	//randomly selecting a node from neighnour of nth node
	int random_number = rand()%K + 1;
	while(random_number != 1){itr++; random_number--;}
	return itr->first;
}

//Table handling utility
map <float, int> Compatible(map <float, int> Distance_Table){
	if (Distance_Table.size() > K){
		map <float, int> :: reverse_iterator reverse;
		reverse = Distance_Table.rbegin();
		while( Distance_Table.size() != K){
			// erase does not take reverse operater to decrementing
			Distance_Table.erase(--reverse.base());
		}
	}
	return Distance_Table;
}

//Merge tables
map <int, float> Merge(map <int, float> table1, map <int, float> table2, int owner){
	//putting table1 in temp
	map <int, float> temp;
	map <int, float> :: iterator itr;
	for(itr = table1.begin(); itr != table1.end(); itr++){
		temp.insert(pair <int, float> (itr->first, Distance(Global_Node_Table[owner], Global_Node_Table[itr->first])));
	}
	//putting table2 in temp
	for(itr = table2.begin(); itr != table2.end(); itr++){
		temp.insert(pair <int, float> (itr->first, Distance(Global_Node_Table[owner], Global_Node_Table[itr->first])));
	}
	//taking temp and chaging list datatype by swapping key
	int index[K], p = 0;
	float dist[K];
	map <float, int> Sorted_list;
	map <float, int> :: iterator compare;
	for(itr = temp.begin(); itr != temp.end(); itr++){
		compare = Sorted_list.find(itr->second);
		if (compare == Sorted_list.end()) {Sorted_list.insert(pair <float, int> (itr->second, itr->first));}
		else {
			index[p] = itr->first;
			dist[p] = itr->second;
			p += 1;
		}
	}
	
	map <int, float> partial_table;
	int size = Sorted_list.size();
	int remaining = K - size; 
	//making the size equal to k of conatiner

	if (size >= K){
		Sorted_list = Compatible(Sorted_list);
	}
	else{
		while(remaining != 0){
			float min = dist[0];
			int final_index = 0;
			//to find minimum
			for(int t = 0; t < p; t++){
				if (dist[t] < min) {
					min = dist[t];
					final_index = t;
				}
			}
			partial_table.insert(pair <int, float> (index[final_index], min));
			remaining --;
			dist[final_index] = numeric_limits<float>::max();
		}
	}

	//Passing the <float, int> map remove more than k elements in the list
	map <float, int> :: iterator ktr;
    //Converting list agin in neighbour form to return
    map <int, float> return_table;
    for(ktr = Sorted_list.begin(); ktr != Sorted_list.end(); ktr++){
    	return_table.insert(pair <int, float> (ktr->second, ktr->first));
    }
    for(itr = partial_table.begin(); itr != partial_table.end(); itr++){
    	return_table.insert(pair <int, float> (itr->first, itr->second));
    }
    return return_table;
}

//Network Evolution
void Network_Evolution(int cycle){
	for(int i = 1; i <= N; i++){
		//randomly selecting a neighbour
		int to_send = Random_Selection(Global_Node_Table[i]);
		//making a copy of neighbour list to be sent
		map <int, float> sending_copy = Global_Node_Table[i]->Neighbours;
		//from this sending copy it will remove neighbour entry and puts its own entry with its own distance from the node
		sending_copy.erase(to_send);
		//inserting its own identifier
		sending_copy.insert(pair <int, float> (i, Distance(Global_Node_Table[i], Global_Node_Table[to_send])));
		//updated copy is sent to selected nodeis temporary view
		Global_Node_Table[to_send]->temp_view = sending_copy;

		//copying the neighbour table of to_send node
		map <int, float> sending_copy2 = Global_Node_Table[to_send]->Neighbours;
		map<int, float> :: iterator f;
		f = sending_copy2.find(i);
		if (f != sending_copy2.end()){ 
			sending_copy2.erase(i);
			sending_copy2.insert(pair <int, float> (to_send, Distance(Global_Node_Table[to_send], Global_Node_Table[i])));
		}

		Global_Node_Table[i]->temp_view = sending_copy2;
		Global_Node_Table[i]->Neighbours = Merge(Global_Node_Table[i]->temp_view, Global_Node_Table[i]->Neighbours, i);
		Global_Node_Table[i]->temp_view.clear();
		Global_Node_Table[to_send]->Neighbours = Merge(Global_Node_Table[to_send]->temp_view, Global_Node_Table[to_send]->Neighbours, to_send);
		Global_Node_Table[to_send]->temp_view.clear();

		for(f = Global_Node_Table[i]->Neighbours.begin(); f != Global_Node_Table[i]->Neighbours.end(); f++){
    			if (f->second != numeric_limits<float>::max()) { sum += f->second; }
    		}

	}
	if ((cycle == 1) || (cycle == 5) || (cycle == 10) || (cycle == 15)){
		string str = TOPOLOGY + "_N" + to_string(N) + "_k" + to_string(K) + "_" + to_string(cycle) + ".txt";
		map <int, float > :: iterator p;
		ofstream file, file2;
		file.open(str);
		file << "Node   " << "Neighbor List" << endl;
		for(int i = 1; i <= N; i++){
			file << i << "   [ ";
			for(p = Global_Node_Table[i]->Neighbours.begin(); p != Global_Node_Table[i]->Neighbours.end(); p++){
				file << p->first << " ";
			}
			file << "]\n";
		}
	} 
}

int main(int argc, const char * argv[]){
	if (argc != 4){
		cout << " USAGE : <./TMAN.cpp> <Number of Nodes> <Number of Neighbors> <Topology (B/S)>\n";
		exit(0);
	}
	ofstream file_sum_dist;
	N = atoi(argv[1]);
	K = atoi(argv[2]);
	TOPOLOGY = argv[3];

	float total = 0.0;
	string str = TOPOLOGY + "_N" + to_string(N) + "_k" + to_string(K) + ".txt";
	file_sum_dist.open(str);
	
	Position_Initializations();
	Network_Initialization();
	//file_sum_dist << "0\t\t" << sum << endl;

	for(int cycle = 1; cycle <= 40; cycle++){	
		Network_Evolution(cycle);
		cout <<" Cycle" << cycle;
		cout << "      The sum is " << sum << endl;
		file_sum_dist << cycle << "\t\t" << sum << endl;
		sum = 0.0;
	}
	cout << endl;
	
	file_sum_dist.close();
	return 0;
}
