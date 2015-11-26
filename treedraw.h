#include <GL/glut.h>
#include <math.h>
#include <vector>
#include <iostream>
#include <fstream>

//------------------------------DATA STRUCTURE DEFINITIONS-------------------------------------------

int spacing;	//  Spacing constant for horizontal distance between nodes.
int r;      	//  Radius of circle.

//Structure for tree node
struct node
{
	std::string data;
	int parent;
	std::vector<int> children;
	int height; 				// which level the node is on
	int modifier;				// field for Algo3 implementation to correct coordinate

	int x; 						// x coordinate
	int y; 						// y coordinate
	int status; 				// 1 - First visit, 2 - Left Visit, 3 - Right Visit for algo 2 and 3
								// Varies from 0 to len(children) for algo1

	node() : x(-1), y(-1), status(0), height(0) {}		//Initialization list
};	

//Global data structures and variables
//Main data structure is dynamic array (vector) tree
//Parent vector is passed from readTree() to the buildTree() routine
//N is the size of tree, taken at runtime
//max_height is globally used both for coordinate assignment and drawing functions
//max_width is an additional variable introduced to adjust global tree width
std::vector<node> tree;
std::vector<int> parent;
std::vector<std::string> labels;
int N;
int max_height;
int max_width;

//Enumeration for status field in node structure
enum {NIL, FIRST_VISIT, LEFT_VISIT, RIGHT_VISIT};

//---------------------------END OF DATA STRUCTURE DEFINITIONS------------------------------------------



//------------------------------GRAPHICAL ROUTINES (OpenGL)---------------------------------------------

//Required initializations for a 2D workspace
void init2D()
{ 
	//Black background
	glClearColor(0.0, 0.0, 0.0, 0.0);
	//Set working matrix to projection for loading identity matrix to specify non-rotated
	//orthogonal plane
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//Orthogonally defined clipped plane
	gluOrtho2D(0.0, 900.0, 0.0, 600.0);
	//Reset working matrix to MODELVIEW to work with the tree
	glMatrixMode(GL_MODELVIEW);
}

//Called by drawCircle function to plot a point in 8 symmetric sections
//drawCircle only executes midpoint algorithm on an octant
//This helps replicate all the octants to form a circle 
void plot8symmetry(int x, int y, int xc, int yc)
{
	//Set color to blue
	glColor3f(0,0,1);
	glBegin(GL_POINTS);
	    glVertex2i(x+xc,y+yc);
	    glVertex2i(-x+xc,y+yc);
	    glVertex2i(x+xc,-y+yc);
	    glVertex2i(-x+xc,-y+yc);
	    glVertex2i(y+xc,x+yc);
	    glVertex2i(-y+xc,x+yc);
	    glVertex2i(y+xc,-x+yc);
	    glVertex2i(-y+xc,-x+yc);
	glEnd();
}

//Implementation of midpoint circle algorithm
void drawCircle(int xc, int yc, int r)
{
	//Start point is (0,r) and decision parameter is set.
	int x = 0, y = r; 
	int d = 1-r;
	
	//Plot the first point
	plot8symmetry(x, y, xc, yc);
	
	//Loop to cover points from y axis to a y where circle's slope = 1 (to exploit 8-way symmetry)
	while(y>=x)	        
	{
		if(d < 0)				//  If d < 0, choose EAST point
			d += 2*x + 3;   	//  Update rule using first derivative of implicit equation of circle.
		else					//  SOUTHEAST point chosen
		{
			d += 2*(x-y) + 5;
			y--;
		}
		x++;
		//Plot the symmetrical points
		plot8symmetry(x, y, xc, yc);
	}
}

/*
  A general implementation of Bresenham's line drawing algorithm
  which needs to handle

    i) Slope<=1 and yleft<=yright
   ii) Slope<=1 and yleft>yright
  iii) Slope>1 and yleft<=yright
   iv) Slope>1 and yleft>yright
    v) SPECIAL CASE : xleft = xright i.e slope = infinity
		a) subcase yleft<=yright
		b) subcase yleft>yright

	It is always assured that xleft<=xright due to the way we call this function
*/
//The input parameters are float, since integral coordinates break the tree drawing
void drawLine(float xleft, float yleft, float xright, float yright)
{
    //Set color : CYAN
	glColor3f(0,1,1);
	
	//SPECIAL CASE: If line is vertical, i.e slope = infinity
	if(xleft==xright )
	{
	    int count = yright - yleft;
	    int add;
	    //Handle cases v)a and v)b
	    if(yright>yleft)
	        add = 1;	        
	    else
	        add = -1;
	    
	    count = abs(count);
	    
	    //Naive vertical line point plotting
	    while(count--)
	    {
	        glBegin(GL_POINTS);
			    glVertex2i(xleft, yleft);
		    glEnd();
		    yleft += add;
		}
    }

	else
	{
	    //Bresenham constants
	    int dy = (yright-yleft);
	    int dx = (xright-xleft);
	    int d, incrE, incrNE;

	    //To handle cases of yleft<yright and yleft>yright
	    int add = 1;
	    
	    //Working x and y coordinates
	    int x = xleft, y = yleft;

	    //Swap y coordinates so that yleft<=yright always
	   	if(dy<0)
	    {
	        std::swap(yleft, yright);
	    	dy = -dy;

	    	//If we have swapped then we need to work from the other x coordinate
	    	//in reverse
	    	x = xright;
	    	add = -1;
	    }

	    //Inital line constants
	    d = 2*dy - dx;
	    incrE = 2 * dy;
	    incrNE = 2 * (dy - dx);

	    //Slope<=1 case, cases 1) and 2) are handled by add value
	    if(abs(dy/dx) <= 1)
	    {
	        x = xleft;
		    while(x<=xright)
		    {
		        if(d <= 0)
		            d += incrE;
		        else
		        {
		            d += incrNE;
		            y += add;
		        }
		        glBegin(GL_POINTS);
			        glVertex2i(x, y);
		        glEnd();
		        x++;
		    }
	    }

	    else
	    {
	    	//Revise line constants if slope>1
	    	//We work from the y axis perspective
	    	d = 2*dx - dy;
	    	incrE = 2 * dx;
	    	incrNE = 2 * (dx - dy);

		    y = yleft;
		    while(y<=yright)
		    {
			    if (d < 0)
		            d += incrE;
		        else
		        {
		            d += incrNE;
		            x += add;
		        }
		        glBegin(GL_POINTS);
			        glVertex2i(x, y);
		        glEnd();
		        y++;
		    }
		}
    }
}

//Line drawing from node to node
//Calculates the periphery points of both nodes to draw line between
void linen2n(int x1, int y1, int x2, int y2)
{
    float xleft, xright, yleft, yright;
    
    //Euclidean distance between two nodes.
    float dist = sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
    
    //Locate Perimeter point of node to draw line from.
    xleft = x1 + r*(x2-x1)/dist;
    yleft = y1 + r*(y2-y1)/dist;
    
    xright = x2 + r*(x1-x2)/dist;
    yright = y2 + r*(y1-y2)/dist;
	
	//Draw line.
	drawLine(xleft, yleft, xright, yright);	
}

//Main tree drawing function
//Composed of circle for node and line from node to node
void drawTree(void) 
{
	glClear(GL_COLOR_BUFFER_BIT);
    
    int xl, xr, yl, yr;
	drawCircle(tree[0].x, tree[0].y, r);
    glColor3f(0,1,0);
    glRasterPos2f(tree[0].x-r/2,tree[0].y-1);
    for(int j=0;j<tree[0].data.length();j++)
        	{glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12,tree[0].data[j]);}

    for(int i=1; i<N; i++)
    {
        xl = tree[i].x;
        yl = tree[i].y;
        
        xr = tree[tree[i].parent].x;        
        yr = tree[tree[i].parent].y;
        
        drawCircle(xl, yl, r);
        
        glColor3f(0,1,0);
        glRasterPos2f(xl-r/2,yl-1);
        for(int j=0;j<tree[i].data.length();j++)
        	{glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12,tree[i].data[j]);}
        
        //This assures that xleft<xright always.
        //Helps simplify cases in general Bresenham
        if(xl<=xr)
          	linen2n(xl, yl, xr, yr);
        else
            linen2n(xr, yr, xl, yl);        
    }
    glutSwapBuffers();
}

//------------------------------END OF GRAPHICAL ROUTINES (OpenGL)---------------------------------------


//------------------------------ALGORITHM UTILITY FUNCTIONS----------------------------------------------

//Horizontal = vertical spacing
//r is radius of circle used for nodes
//Reassign coordinates based on spacing parameters
void setSpacingParameters()
{
	//We have introduced max_width to ensure tree always fits
	for(int i=0; i<N; i++)
    	max_width = std::max(max_width, tree[i].x);
	
	//This definition of spacing helps fit the tree irrespective of height and width
	spacing = std::min(std::min(40, 300 / (max_height + 1)), 900/ (max_width+1));
	r = spacing/3;

	for(int i=0;i<N;i++)
	{
		tree[i].x = spacing*tree[i].x;
		tree[i].y = 600 - spacing*tree[i].y;
	}
}

//Prompt at runtime to specify tree by size and parent array
void readTree()
{
		std::ifstream file;
		while(!file.is_open())
			file.open("notes.txt",std::fstream::in);

		parent.erase(parent.begin(),parent.end());
		tree.erase(tree.begin(),tree.end());
		labels.erase(labels.begin(),labels.end());
		max_height = 0;
		max_width = 0;
		
		int temp = -1;
		std::string label;
		file>>label;
		while(!file.eof())
		{
			parent.push_back(temp);
			labels.push_back(label);
			file>>temp;
			file>>label;
		}
		file.close();
		
		N = parent.size();
}

//Build internal representation of tree from parental array
//by populating tree vector
void buildTree()
{
	tree.resize(N);
	for(int i=0;i<N;i++)
	{
		//std::cerr<<"Loop"<<i<<" "<<N<<"\n";
		if(parent[i]==-1)
		{
			tree[i].height = 0;
			tree[i].parent = -1;
			tree[i].data = labels[i];
			continue;
		}

		//We get around using pointers by using the concept of parent and child indexes
		//to traverse the tree
		tree[i].parent = parent[i];
		tree[i].data = labels[i];
		tree[parent[i]].children.push_back(i);
		
		//Update height of new node w.r.t to parent
		//Also store max height so far
		tree[i].height = tree[parent[i]].height + 1;
		max_height = std::max(tree[i].height, max_height);
	}
}

//Command-line output for debugging
void printTree()
{
	for(int i=0;i<N;i++)
	{
		std::cout<<"Node "<<i<<" : ";
		std::vector<int> tempv = tree[i].children;
		if(tempv.size()==0)
			std::cout<<"Leaf node. ";
		else
			for(int j=0;j<tempv.size();j++)
				std::cout<<tempv[j]<<" ";
		std::cout<<"Height : "<<tree[i].height;
		std::cout<<" x = "<<tree[i].x;
		std::cout<<" y = "<<tree[i].y;
		std::cout<<" modifier = "<<tree[i].modifier;
		std::cout<<"\n";
	}
}

//----------------------------END OF ALGORITHM UTILITY FUNCTIONS------------------------------------------
