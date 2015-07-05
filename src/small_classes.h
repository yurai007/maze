#ifndef _small_classes_h_
#define _small_classes_h_

#include<utility>
#include<time.h>
#include<stdlib.h>

using namespace std;

class Point 
{
	pair<int,int> po;
	
	public:
		Point(int x,int y) 
		{
			po = make_pair(x,y);
		}
		
		void set_coor(int x,int y)
		{
			po = make_pair(x,y);
		}
		
		int getx() const
		{
			return po.first;
		}
		
		int gety() const
		{
			return po.second;
		}
		
		inline bool operator==(Point p)
		{
			return ( (getx()==p.getx())&&(gety()==p.gety()) );
		}
		
		Point operator+(Point p)
		{
			p.set_coor(p.getx()+getx(),p.gety()+gety());
			return p;
		}
		
		bool inside_rect(Point a,Point b)
		{//nierowności ostre
			return (a.getx()<getx()) && (getx()<b.getx()) && (a.gety()<gety()) && (gety()<b.gety());
		}
		
};

#endif
