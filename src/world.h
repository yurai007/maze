#ifndef _world_h_
#define _world_h_

#include<gtkmm.h> 
#include<iostream>
#include<time.h>
#include<list>
#include<sstream>

using namespace Gtk;
using namespace std;

class Area : public DrawingArea
{
	private:
		vector<int> hashH,hashA;//trzymają id odp. przeciwników i artefaktów
		Random_maze labirynt;
		list<Point> maze;//buforowany labirynt 
		list<int> Kosz;//trzymamy id usuniętych artefaktów
		int scale,value_artefacts;
		
			struct paczka 
					{
						string dir;
						int val;
					};
		paczka spis[5];
		vector<string> diry;
		
	public://to trzeba zasłonić
		Gracz ludek;
		vector<Zlodziej*> wrogowie;
		vector<Artefakt*> znajdki;
		
		//w = 35 h = 24
		
		static bool cmp(Point a,Point b)
		{
			return (rand()%100<rand()%100);
		}
	
		void obsluz_napisy(paczka IT[])
		{
			diry.assign(6,"");
				for (int i=0;i<5;i++) 
				{
					spis[i].dir = IT[i].dir;	spis[i].val = IT[i].val;
					diry[i] = spis[i].dir;
				} 
		}
		
		Area( int nh,int na,int nver,int nhor,int kv,int kh,int fpa,
         	const string dir1,const string dir2,const string dir3,const string dir4,const string dir5,const string dir6,
         	const string dir7,const string dir8,
         	const string dir9,int scale) : ludek(dir1,0,-30), 
						  		  				  		  labirynt(nver,nhor,kv,kh,fpa,scale,dir9),
						  		  				  		  scale(scale)
		{//max(nh,na,1) <= nhor*nver*fpa
		
			Zlodziej *wsk = NULL;
			Artefakt *it = NULL;
			wrogowie.assign(nh+1,wsk);
			hashH.assign(10000,0);
			hashA.assign(10000,0);
			znajdki.assign(na+1,it);
			Kosz.clear();
			
			list<Point> Buffer = labirynt.get_vertex();
			maze.clear();
			maze = Buffer;
			Buffer.sort(cmp);
			maze.sort(cmp);
			
			Point q(0,0);
				for (int i=1;i<=nh;i++)
				{
						wsk = new Zlodziej(dir2,0,0);
						q = *Buffer.begin(); Buffer.erase(Buffer.begin());
						wsk->set_pos(q.getx()*scale,q.gety()*scale);
						wrogowie[i] = wsk;
						hashH[q.getx()*100+q.gety()] = i;
				}
			paczka IT[]= {{dir4,50},{dir8,30},{dir5,20},{dir6,10},{dir7,5}};
			obsluz_napisy(IT);
	
			int s;
			value_artefacts = 0;
			
				for (int i=1;i<=na;i++)
				{
					s = rand()%5;
					q = *Buffer.begin(); Buffer.erase(Buffer.begin());
					it = new Artefakt(q.getx()*scale,q.gety()*scale,IT[s].val,diry,s);
					znajdki[i] = it;
					hashA[q.getx()*100+q.gety()] = i;
					value_artefacts += znajdki[i]->get_value();
				}				
			q = *(Buffer.begin());
			Buffer.erase(Buffer.begin());	
			ludek.set_pos(q.getx()*scale,q.gety()*scale);
			ludek.set_mode(false);
   	 	Glib::signal_timeout().connect(sigc::mem_fun(*this, &Area::on_lets_live),500);
		}
		
		void set_enabled_hostels(bool fl)
		{
			for (int i=1;i<wrogowie.size();i++)
    				wrogowie[i]->set_mode(fl);
		}
		
		void rozrzuc_artefakty(int ile)
		{
			vector<int> U(5,0);
			int wp = ile,i=0;
			
				while (wp > 0)
				{
					U[i] = wp/spis[i].val; wp -= spis[i].val*U[i]; i++;
				}
			Point q(0,0); int id;
			
			for (int j=0;j<5;j++)
				for (int i=1;i<=U[j];i++)
				{
					if (maze.size() == 0)
					{
						maze = labirynt.get_vertex();
						maze.sort(cmp);
					}
					else
					{
						q = *maze.begin(); maze.erase(maze.begin());
						id = *(Kosz.begin()); Kosz.erase(Kosz.begin()); 
						hashA[q.getx()*100+q.gety()] = id;
						
						znajdki[id] = new Artefakt(q.getx()*scale,q.gety()*scale,spis[j].val,diry,j);
						
					}
				}  
		}
		
		list<Point> get_arround()
		{	
			list<Point> sth;
			sth.clear();
				for (int i=1;i<wrogowie.size();i++)
					sth.insert(sth.end(),wrogowie[i]->get_pos());
			
			return sth;
		}
		
		bool check_collision_GZ(Point pos,list<Point> poz)
	  	{
	  			for (list<Point>::iterator it = poz.begin();it != poz.end();it++)
	  			if (*it == pos)
	  				return true;
	  			return false;
	  	}
	  	
	  	
	  	bool check_collision_GL(Point where)
		{
			where.set_coor(where.getx()/scale,where.gety()/scale);
			return (!labirynt.member(where) || !where.inside_rect(labirynt.get_area().first,labirynt.get_area().second) );
		
		}
		
		int find_collision_GA(Point where)
		{
			return hashA[where.getx()*10/3+where.gety()/30];
		}
		
		void delete_artefact(Point where)
		{
			int indx = where.getx()*10/3+where.gety()/30;
			int id = hashA[indx];
			Kosz.insert(Kosz.end(),id);
			delete znajdki[id]; znajdki[id] = NULL;
			hashA[indx] = 0;
		}
		
		void add_value(int id)
		{
				ludek.set_kieszen(ludek.get_kieszen()+znajdki[id]->get_value());		
		}
		
		bool finish()
		{
			return (ludek.get_kieszen() == value_artefacts); 
		}
		
		void przeskocz()
		{
			maze = labirynt.get_vertex();
			maze.sort(cmp);
			Point q(ludek.get_pos().getx()/30,ludek.get_pos().gety()/30);
		
			for (list<Point>::const_iterator ci = maze.begin(); ci != maze.end(); ci++)
				if (hashH[ci->getx()*10/3+ci->gety()/30] == 0)
				{
					q = *ci;
					break;
				}
					
			ludek.set_pos(q.getx()*30,q.gety()*30);
		}
	
	protected:	
	
		void draw_text(Glib::RefPtr<Style> gs)
		{
			Glib::RefPtr<Pango::Context> pc=Gtk::Widget::create_pango_context();
			Glib::RefPtr<Pango::Layout> pl=Pango::Layout::create(pc);	
			Pango::FontDescription fd;
			fd.set_absolute_size(25000);
			
			int i = ludek.get_kieszen();
			string val1,val2;
			stringstream out1,out2;
			out1<<i;
			out2<<value_artefacts;
			val1 = out1.str();
			val2 = out2.str();
			
			pl->set_text("Kieszen: " + val1 + " MAX: " + val2); 
			pl->set_font_description(fd);
			get_window()->draw_layout(gs->get_black_gc(),10,10,pl); 
		
		}
	
		bool check_collision_ZG(Zlodziej *z)
		{//czy wróg na l,r,u,d
				Point pn = z->get_pos() + z->get_vec(); 
				return (pn==ludek.get_pos());
		}
				
		
		bool check_collision_ZZ(int id)
		{
			//pobierz aktualną i planowaną pozycje,przeskaluj 
			Point curr = wrogowie[id]->get_pos();
			Point s = curr + wrogowie[id]->get_vec();
			//szukaj s w zbiorze punktów
				if (hashH[s.getx()*10/3+s.gety()/30] != 0)
					return true;
				else
				{//niezajęte
					hashH[curr.getx()*10/3+curr.gety()/30] = 0;
					hashH[s.getx()*10/3+s.gety()/30] = id;
				}
			return false;
		}
		
		bool check_collision_ZL(int id)
		{
			Point next(0,0);
			next = wrogowie[id]->get_pos()+wrogowie[id]->get_vec();
			next.set_coor(next.getx()/scale,next.gety()/scale);
			return (!labirynt.member(next) || !next.inside_rect(labirynt.get_area().first,labirynt.get_area().second) );
		}
		

		bool on_lets_live()
		{
				for (int i=1;i<wrogowie.size();i++)
				{
					wrogowie[i]->ustaw_czynnosc();//po tym aktuailzacja zbioru złodzieji
						if (wrogowie[i]->get_mode())
						{
							if (check_collision_ZG(wrogowie[i]))
							{//zabierz część skarbu
								wrogowie[i]->set_mode(false);
									if (ludek.get() == 0)
									{
										int zabierz = min(ludek.get_kieszen(),value_artefacts/5 - (value_artefacts/5)%10);
										ludek.set_kieszen(ludek.get_kieszen() - zabierz);
										rozrzuc_artefakty(zabierz);
									}
										
								ludek.set_nietykalnosc(10);
							}
							else
							if (check_collision_ZZ(i))
								wrogowie[i]->set_mode(false);
							else
							if (check_collision_ZL(i))
								wrogowie[i]->set_mode(false);
						}
    			}
    			
    			ludek.dec_nietykalnosc(); ludek.dec_teleport();
    			ludek.set_mode(false);//maska na ludzika
    			queue_draw();
			
			return true;
		}
		
	
		virtual bool on_expose_event(GdkEventExpose* event) 
  		{
  			Glib::RefPtr<Gdk::Window> winda = get_window(); //smart_pointer na okno 
  				if (winda)
  				{
  					labirynt.draw(winda,get_style());	
    				ludek.draw(winda,get_style());
    					
    					for (int i=1;i<wrogowie.size();i++)
    						wrogowie[i]->draw(winda,get_style());
    					
    					for (int i=1;i<znajdki.size();i++)
    					if (znajdki[i] != NULL) 
    						znajdki[i]->draw(winda,get_style());
    						
    				//tekst
    				draw_text(get_style());
    			}
    		return true;
  		} 
};


class MainClass : public Window
{	  	 
	  private:
	  		Area world; 		
	  		bool ok;
	  		
     public:
     		
     		bool key_released(GdkEventKey* event)
     		{
     			ok = true;
     		}
     
     		bool key_pressed(GdkEventKey* event)
			{
				if (!ok) return true; 
				else
					ok = false;
					
				bool up = (event->keyval == GDK_Up);
				bool down = (event->keyval == GDK_Down);
				bool left = (event->keyval == GDK_Left);
				bool right = (event->keyval == GDK_Right);
				bool space = (event->keyval == GDK_space);//teleport
				
					if (up||down||left||right||space)
					{
			   			Point pom(world.ludek.get_vec().getx(),world.ludek.get_vec().gety());
							if (up)
							{	
					 			world.ludek.set_vec(0,-30);
					 			world.ludek.skieruj(0);
							}
							else
							if (down)
							{
								world.ludek.set_vec(0,30);
								world.ludek.skieruj(180);
							} 
							else
							if (right)
							{
								world.ludek.set_vec(30,0);
								world.ludek.skieruj(90);
							} 
							else
							if (left)
							{
								world.ludek.set_vec(-30,0);
								world.ludek.skieruj(270);
							}
							else
							if (world.ludek.get_teleport() == 0)
							{
								world.przeskocz();
								world.ludek.set_teleport(20);
							}
							
				
							if (!(pom == world.ludek.get_vec()) )  world.ludek.set_mode(false);
							else
							if (world.check_collision_GZ(world.ludek.get_pos()+world.ludek.get_vec(),world.get_arround()))
								world.ludek.set_mode(false);
							else
							if (world.check_collision_GL(world.ludek.get_pos()+world.ludek.get_vec()))
								world.ludek.set_mode(false);
							else
							{
								int p = world.find_collision_GA(world.ludek.get_pos()+world.ludek.get_vec());
									if (p != 0)
									{
										world.add_value(p);
										world.delete_artefact(world.ludek.get_pos()+world.ludek.get_vec());
											if (world.finish())
											{
												cout<<"THE END"<<endl; exit(0);
											}
									}
							}
								
							world.set_enabled_hostels(false);//maska na wrogów
							queue_draw();
					}
				return true;
			} 
     
     
         MainClass( int nh,int na,int nver,int nhor,int kv,int kh,int fpa,
         			  const string dir1,const string dir2,const string dir3,const string dir4,const string dir5,const string dir6,
         			  const string dir7,const string dir8,const string dir9,								
         			  const int ww,const int wh,const int scale) : world(nh,na,nver,nhor,kv,kh,fpa,dir1,dir2,
         			  															  dir3,dir4,dir5,dir6,dir7,dir8,dir9,scale)
         {
         		set_title("LABIRYNT");
         		set_default_size(ww,wh);
         		add(world);
           		signal_key_press_event().connect(sigc::mem_fun(*this,&MainClass::key_pressed),false);
           		signal_key_release_event().connect(sigc::mem_fun(*this,&MainClass::key_released),false);
           		
           		show_all_children();
         }          
};


#endif
