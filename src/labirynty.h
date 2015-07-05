#ifndef _labirynty_h_
#define _labirynty_h_

#include<list>
#include<vector>
#include<gtkmm.h>
#include<time.h>
#include<iostream>
#include<algorithm>

using namespace std;
using namespace Gtk;

class Simple_maze
{
	/* perzechowuje generowany losowo spójny labirynt o size polach aktywnych.Zbior pól aktywnych labiryntu
		to podzbior {minx,..,maxx-1}X{miny,..,maxy-1},maxx<=99,maxy<=99 dlatego konieczne jest przeskalowanie każdego punktu (x,y)
		płaszczyzny do pola bedacego kwadratem o boku c i lewym górnym rogu położonym w (cx,cy).size <= maxx*maxy < 10000
	*/  
	protected:
	
		vector<list<int> > Graf;//listy dł. max. 4,graf reprezentuje strukture przejść w labiryncie
		vector<Point> Points;//Podzbiór NxN reprezentowany przez graf
		vector<int> Vp;//id wierzchołków aktywnych
	   vector<int> hash;
		int n,m;//n<=1000
		const Point P0,PM;
		
		list<Point> get_arround(Point q)
		{
			Point p(0,0),p0(P0.getx()-1,P0.gety()-1);
			list<Point> L; L.clear();
			for (int a=-1;a<=1;a++)
					for (int b=-1;b<=1;b++)
						if (( a==0 )&&( b!=0 )||( a!=0 )&&( b==0 ))
							{
								p.set_coor(q.getx()+a,q.gety()+b);
									if (p.inside_rect(p0,PM))
										L.insert(L.end(),p);
							}
			return L;
		}
		
		Point wybierz_sasiada(const list<Point> &Lista)
		{//losowy element niepustej listy lista
				int j = rand()%Lista.size(),k=0;
				list<Point>::const_iterator it = Lista.begin();
					while (k<j)
							{
								k++; it++;
							} 
				return *it;
		}
		
		static int count(Point p)
		{
			return p.getx()*100+p.gety();
		}
		
		static Point recount(int key)
		{
			Point p(key/100,key%100);
			return p;
		}
		
		bool nie_ma(const Point &p)
		{
			return (hash[count(p)] == 0);
		}
		
		bool pelny(int id,int len)
		{
			int x = Points[id].getx(), x0 = P0.getx(), xm = PM.getx()-1;
			int y = Points[id].gety(), y0 = P0.gety(), ym = PM.gety()-1;
			
				if ((x == x0)&&(y == y0) || (x == xm)&&(y == ym) || (x == x0)&&(y == ym) || (x == xm)&&(y == y0))
					if (len == 2)
						return true;
				if ((x == x0) || (x == xm) || (y == y0) || (y == ym))
					if (len == 3)
						return true;
				if ((x != x0)&&(x != xm)&&(y != y0)&&(y != ym))
					if (len == 4)
						return true;
						
			return false;
		}
		
		void construct_graph()
		{// złożoność pesymistyczna O(size*size)
			int minx = P0.getx(),maxx = PM.getx(),miny = P0.gety(),maxy = PM.gety();
			Point p(rand()%(maxx-minx)+minx,rand()%(maxy-miny)+miny);
			Points[1] = p; Vp[1] = 1; hash[count(p)] = 1;
			list<Point> otoczenie; otoczenie.clear();//hipotetyczne otoczenie
			int q,j,size = 1;
			
				for (int i=2;i<=n;i++)
				{
				
					q = Vp[rand()%size+1];
					//dopuszczalni sąsiedzi wokół q
					otoczenie = get_arround(Points[q]);
					//przesiej OTOCZENIE = OTOCZENIE/SASIEDZI
					list<int> sasiedzi; sasiedzi.clear();
					//zrobić miejsce w sasiedzi?
					copy(Graf[q].begin(),Graf[q].end(),back_inserter(sasiedzi));
				
					list<Point>::iterator usun;
					
						for (list<int>::iterator it=sasiedzi.begin();it != sasiedzi.end(); it++)
						{
							usun = find(otoczenie.begin(),otoczenie.end(),Points[*it]);
								if (usun != otoczenie.end())
									otoczenie.erase(usun);
						}
					//losuj wspólrzędne sąsiada q
					
					p = wybierz_sasiada(otoczenie);
					
					//obsługa zbiorów i grafu
					Points[i] = p;
					hash[count(p)] = i;
					otoczenie = get_arround(p);
					
					//otoczenie.remove_if(nie_ma); //filter w otoczeniu tylko te z grafu
					list<Point>::iterator pn = otoczenie.begin(),g = pn; 
					
						while (g!= otoczenie.end())
						{
							pn = g; pn++;
							if (nie_ma(*g)) otoczenie.erase(g);
							g = pn;
						}
					
					int id_s;
					
					//analiza potencjalnych sąsiadów p
						for (list<Point>::iterator it=otoczenie.begin();it != otoczenie.end(); it++)
						{
							id_s = hash[count(*it)];
							Graf[id_s].insert(Graf[id_s].end(),i);
							Graf[i].insert(Graf[i].end(),id_s);//dołożenie krawędzi p-id_s
							
								if (pelny(id_s,Graf[id_s].size())) 
								{
									vector<int>::iterator del = find(Vp.begin(),Vp.end(),id_s); 
									//możnaby logn,ale i tak będziemy przesuwać Vp
									
									Vp.erase(del);	//tutaj pięta Achillesowa:(
									size--;
								}
						}
						if (!pelny(i,Graf[i].size()))
							Vp[++size] = i;
				}
		}
		
	public:
	
		Simple_maze(int size,int minx,int miny,int maxx,int maxy) : P0(minx,miny),PM(maxx,maxy)
		{
			n = size;	
			hash.assign(10000,0);
			Points.assign(n+1,P0);
			Vp.assign(n+1,0);
			list<int> Lp; Lp.clear();
			Graf.assign(n+1,Lp);
			construct_graph();	
		}
		
		list<Point> get_vertex() const
		{
			list<Point> result; result.clear(); 
			copy(Points.begin(),Points.end(),back_inserter(result));
			result.erase(result.begin());
			return result;
		}
};


class Random_maze
{
	/* przechowuje generowany pseudolosowo spójny labirynt o ciekawszej strukturze niż Simple_maze ze zbioru 
		{0,..,maxx-1}x{0,..,maxy-1}. lver - szerokość siatki,lhor - wysokość siatki, kv - szerokość obszaru pola, 
		kh - wysokość ..., n - ogr.dolne na liczbe pól w pojedynczym obszarze ,scale - skalowanie pola na ekran.
		Powinno zachodzić max(nh,na,1) <= nhor*nver*fpa
	*/ 
	protected:
		int nver,nhor,nv,nh,ile,scale,maxx,maxy;
		vector<Point> Points;
		vector<int> hash;
		Glib::RefPtr<Gdk::Pixbuf> kafel1;
		
	static int count(Point p)
		{
			return p.getx()*100+p.gety();
		}
		
	static Point recount(int key)
		{
			Point p(key/100,key%100);
			return p;
		}
	
	static bool cmpX(pair<Point,bool> a,pair<Point,bool> b)
	{
		return (a.first.getx()<b.first.getx());
	}
	
	static bool cmpY(pair<Point,bool> a,pair<Point,bool> b)
	{
		return (a.first.gety()<b.first.gety());
	}	
	
	static bool ok(list<pair<Point,bool> >::iterator x,char wsk)
	{
		list<pair<Point,bool> >::iterator y = x; y++;
			if (wsk == 'X')
				return (( x->first.getx() == y->first.getx() ) && (x->second != y->second));
			else
				return (( x->first.gety() == y->first.gety() ) && (x->second != y->second));
	}
	
	void iteruj(list<Point> &L1,list<Point> &L2,char wsk,Point zp,Point zk,int start,int stop,int it)
	{
		Point g(0,0);
			for (int i=start;i<=stop;i++)
					{
						if (wsk == 'X')
							g.set_coor(it,i);
						else
							g.set_coor(i,it);
						if (hash[count(g)] == 0)
						{
							if (g.inside_rect(zp,zk))
								L2.insert(L1.end(),g);
							else
								L1.insert(L2.end(),g);
							hash[count(g)] = 1;
						}
					}
	}
	
	static char rev(char c)
	{
		return (c == 'X')? 'Y' : 'X';
	}
	
		
	void merge_with_left_and_up(list<Point> &L1,list<Point> &L2,char wsk,pair<Point,Point> z)//zakres z L2,L2 - neutralny
	{
		list<pair<Point,bool> > La,Lb,result; La.clear(); Lb.clear(); result.clear();
		
			for (list<Point>::iterator p = L1.begin(); p!= L1.end(); p++)
				La.insert(La.end(),make_pair(*p,false));
			for (list<Point>::iterator p = L2.begin(); p!= L2.end(); p++)
				Lb.insert(Lb.end(),make_pair(*p,true));			
				
			if (wsk == 'X')
			{
				La.sort(cmpX); Lb.sort(cmpX); //tu wszedzie stabilne
				copy(La.begin(),La.end(),back_inserter(result)); result.merge(Lb,cmpX);
			}
			else
			{
				La.sort(cmpY); Lb.sort(cmpY); //tu wszedzie stabilne
				copy(La.begin(),La.end(),back_inserter(result)); result.merge(Lb,cmpY);
			}
		
		list<pair<Point,bool> >::iterator q;
			for (q = result.begin(); q != --result.end(); q++)
				if (ok(q,wsk)) break; //możnaby też przechodzić od tyłu
				
		Point zp(z.first.getx()-1,z.first.gety()-1);	
				
			if (ok(q,wsk)) 
			{
				pair<Point,bool> p1 = *q,p2 = *(++q);
				int start,stop,it;
					if (wsk == 'X')
					{
						if (p1.first.gety() > p2.first.gety()) swap(p1,p2);
						start = p1.first.gety()+1; stop = p2.first.gety()-1; it = p1.first.getx();
					}
					else
					{
						if (p1.first.getx() > p2.first.getx()) swap(p1,p2);
						start = p1.first.getx()+1; stop = p2.first.getx()-1; it = p1.first.gety();
					}
				iteruj(L1,L2,wsk,zp,z.second,start,stop,it);
			} 
			else 
			{
				//póki co bez tego
				//weź dowolne 2 punkty p1,p2
				//oblicz zgięcie p3 
				//od p1 poziomo/pionowo do p3, póżniej przeciwny wektor do p2
				int go1,go2,stop1,stop2,it1,it2;
				Point p1(0,0),p2(0,0),p3(0,0);
					if (rand()%2 == 0)
						p1 = *L1.begin();
					else
						p1 = *(--L1.end()); 
					if (rand()%2 == 0)
						p2 = *L2.begin();
					else
						p2 = *(--L2.end());
						 
					if (wsk == 'X')
					{
						p3.set_coor(p1.getx(),p2.gety());
						go1 = p1.gety()+1; stop1 = p3.gety();
						it1 = p3.getx(); it2 = p3.gety();
							if (p3.getx()<p2.getx())
							{
								go2 = p3.getx()+1; stop2 = p2.getx()-1;
							}
							else
							{
								go2 = p2.getx()+1; stop2 = p3.getx()-1;
							}
					}
					else
					{
						p3.set_coor(p2.getx(),p1.gety());
						go1 = p1.getx()+1; stop1 = p3.getx();
						it1 = p3.gety(); it2 = p3.getx();
							if (p2.gety()<p3.gety())
							{
								go2 = p2.gety()+1; stop2 = p3.gety()-1;
							}
							else
							{
								go2 = p3.gety()+1; stop2 = p2.gety()-1;
							}
					}
				iteruj(L1,L2,wsk,zp,z.second,go1,stop1,it1);
				iteruj(L1,L2,rev(wsk),zp,z.second,go2,stop2,it2);
			}

	}
		
	public:
	
	void draw(Glib::RefPtr<Gdk::Window> gw,Glib::RefPtr<Style> gs)
		{//wyswietla dopełnienie Points, skalowanie o scale
			Point q(0,0); 
			for (int i=0;i<10000;i++)
				if (hash[i] == 0)
				{
					q = recount(i);
					kafel1->render_to_drawable(gw,gs->get_black_gc(),0,0,q.getx()*scale,q.gety()*scale,kafel1->get_width(),kafel1->get_height(), 		  	 			Gdk::RGB_DITHER_NONE,0,0);
				} 
		}
	
	
	Random_maze(int lver,int lhor,int kv,int kh,int n,int scale,string dir) : nver(lver),nhor(lhor),nv(kv),nh(kh),ile(n),scale(scale)
	{
		maxx = kv*lver;
		maxy = kh*lhor;
		srand(time(NULL));
		Point Pmin(0,0),Pmax(0,0),Pmi(0,0);
		Simple_maze *generator;
		
		hash.assign(10000,0);
		list<Point> sth; sth.clear();
		vector<vector<list<Point> > > Areas;
		Areas.assign(lver+1,vector<list<Point> >(lhor+1,sth));
		
		//może od razu łączenie left-up?
		for (int j=1;j<= lhor;j++)
			for (int i=1;i<=lver;i++)
			{
				Pmin.set_coor(kv*(i-1),kh*(j-1)); Pmax.set_coor(kv*i,kh*j); Pmi.set_coor(Pmin.getx()-1,Pmin.gety()-1);
				generator = new Simple_maze(n,Pmin.getx(),Pmin.gety(),Pmax.getx(),Pmax.gety());//tu z randem?
				Areas[i][j] = generator->get_vertex(); //sortowanie leniwie, kiedy potrzebne
				
				for (list<Point>::iterator p=Areas[i][j].begin();p!= Areas[i][j].end();p++)	
					hash[count(*p)] = 1;
			
		
					if (i > 1)
						merge_with_left_and_up(Areas[i-1][j],Areas[i][j],'Y',make_pair(Pmi,Pmax)); // left prostym longerem lub haczykiem
					if (j > 1)
						merge_with_left_and_up(Areas[i][j-1],Areas[i][j],'X',make_pair(Pmi,Pmax)); //up dokłada do argumentów
				delete generator;
			}
		int size=0;
			
		for (int j=1;j<= lhor;j++)
			for (int i=1;i<=lver;i++)
			{
				Points.insert(Points.end(),Areas[i][j].begin(),Areas[i][j].end());
				size += Areas[i][j].size();
			}
		
		kafel1 = Gdk::Pixbuf::create_from_file(dir);	
	}

	list<Point> get_vertex() const
		{
			list<Point> result; result.clear(); 
			copy(Points.begin(),Points.end(),back_inserter(result));
			//result.erase(result.begin());
			return result;
		}
	
	bool member(Point x) const //czy jest  
	{
		return (hash[count(x)] == 1);
	}
	
	pair<Point,Point> get_area() const
	{
		Point a(-1,-1),b(maxx,maxy);
		return make_pair(a,b);
	}
};

#endif
