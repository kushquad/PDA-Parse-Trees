// Assignment 1 - Algorithm 3 (Wetherell and Shannon)
// Tidy Tree Drawing
// Combines both Algorithm 1 and 2 to meet both aesthetics (most of the time)
// - minimum width tree drawing
// - parent centered around children

// Modified Algorithm 3 in assignCoordinatesModified() at end

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "treedraw.h"

using namespace std;

//Shannon and Wetherell Algorithm 3 for coordinate assignment (modified as per paper)
//Satisfies both aesthetics while minimizing paper width
void assignCoordinates()
{
	//Variables required by algorithm
	int modifier[max_height+1];
	int next_pos[max_height+1];

	for(int i=0;i<=max_height;i++)
		{modifier[i] = 0, next_pos[i] = 1;}

	//Initiate traversal from tree
	node* current = &tree[0];
	current->status = 0;
	//Flag is to indicate when NIL has been reached i.e when parent=-1
	bool flag = true;

	while(flag)
	{
		//Iterative traversal through status markers
		//POST-ORDER TRAVERSAL
		if(current->status==0)
		{
			//current = &tree[current->children[0]];
			for(int i=0; i<current->children.size(); i++)
				tree[current->children[i]].status = 0; 
			current->status = 1;
		}
		
		else if(current->status>=1 && current->status<=current->children.size())
		{
			current->status++;
			current = &tree[current->children[current->status-2]];
		}

		else
		{
			//After RIGHT, two possibilities arise
			//We are on a leaf or an internal node

			//We need to adjust the place of this node accordingly
			int h = current->height;
			bool is_leaf = (current->children.size()==0);
			int place;

			//Use algorithm 1's concept if a leaf
			if(is_leaf)
				place = next_pos[h];
			//If only one child, keep child below parent
			else if(current->children.size()==1)
				place = tree[current->children[0]].x;
			//If both children, take average of positions and assign
			else
			{
				int sum = 0;
				for(int i=0; i<current->children.size();i++)
					sum += tree[current->children[i]].x;
				place = (sum/current->children.size());
			}

			//Compute modifier for second traversal
			modifier[h] = max(modifier[h], next_pos[h] - place);
			if(is_leaf)
				current->x = place;
			else
				current->x = place + modifier[h];
			current->modifier = modifier[h];
			current->y = 2*current->height + 1;

			//Update next_pos for current height
			next_pos[h] = current->x + 2;

			//Go up level and visit parent
			if(current->parent!=-1)
				current = &tree[current->parent];
			//If parent==-1, means we have reached NIL
			else
				flag = false;
		}
	}
	
	//Set traversal variables to initial values for second traversal
	//MODIFICATIONS BEGIN HERE
	for(int i=0;i<=max_height;i++)
		next_pos[i] = 1;
	
	current = &tree[0];
	current->status = 0;
	int modifier_sum = 0;
	flag = true;

	while(flag)
	{
		//printf("Node %d:\n",current->data);
		if(current->status==0)
		{
			for(int i=0;i<current->children.size();i++)
			{
				int deviation = (current->children.size()%2)?(i-current->children.size()/2):((i-current->children.size()/2)
																					+((i<current->children.size()/2)?(0):(1)));
				int temp = current->x+2*deviation;
				if(temp>=next_pos[tree[current->children[i]].height])
				{
					tree[current->children[i]].x =  temp;
					tree[current->children[i]].status = 0;	
					next_pos[tree[current->children[i]].height] = temp+2; 
				}
				else
				{
					tree[current->children[i]].x = next_pos[tree[current->children[i]].height];
					tree[current->children[i]].status = 0;
					next_pos[tree[current->children[i]].height] += 2; 
				}
				//printf("%d:%d\n",i,tree[current->children[i]].x);
			}
			//printf("\n");
			current->status = 1;
		}

		else if(current->status>=1 and current->status<=current->children.size())
		{
			current->status++;
			current = &tree[current->children[current->status-2]];
		}

		else
		{
			if(current->parent!=-1)
				current = &tree[current->parent];
			else
				flag = false;
		}
	}

}

void updateRoutine(void)
{
	//Read, build and assign coordinates to tree nodes according to Algorithm 2
	readTree();
	buildTree();
	assignCoordinates();
 	setSpacingParameters();
 	drawTree();
}

int main(int argc, char* argv[])
{
	//GLUT setup
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(50,50);
	glutInitWindowSize(900,600);
	glutCreateWindow("Algorithm 3");
	
	//Display tree and window
	init2D();
	glutDisplayFunc(updateRoutine);
	glutIdleFunc(updateRoutine);
	glutMainLoop();

	return 0;
}

