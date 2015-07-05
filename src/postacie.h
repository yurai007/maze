#ifndef _postacie_h_
#define _postacie_h_

#include<gtkmm.h> 
#include<iostream>
#include<time.h>
#include<list>

using namespace Gtk;
using namespace std;

class Postac
{	//klasa abstrakcyjna
	protected:
		Point uchwyt,wektor;//pos i move
		bool mode; //T-animuje z przesunieciem,F-animuje bez przesuniecia		może nomove?
		Glib::RefPtr<Gdk::Pixbuf> imageT;//wskażnik na pixbuf
		Glib::RefPtr<Gdk::Pixbuf> imageS;
		
		virtual void abc() = 0;
		
	public:		
		
		Postac(string dir,int a,int b) : uchwyt(0,0),wektor(a,b) 
		{
			mode = true;
			imageS = Gdk::Pixbuf::create_from_file(dir);
			imageT = Gdk::Pixbuf::create_from_file(dir);
		}
		
		void set_vec(int a,int b)
		{
			wektor.set_coor(a,b);
		}
		
		void set_pos(int i,int j)
		{
			uchwyt.set_coor(i,j);
		}
		
		
		Point get_vec() const
		{
			return wektor;
		}
		
		Point get_pos() const
		{
			return uchwyt;
		}
		
		void reset()
		{
			imageS->copy_area(0,0,30,30,imageT,0,0); //imageT to dest. 	
		}
		
		void obrot_w_prawo(int angle)
		{
			if (angle == 90)
				imageT = imageT->rotate_simple(Gdk::PIXBUF_ROTATE_CLOCKWISE);
			else
			if (angle == 180)
				imageT = imageT->rotate_simple(Gdk::PIXBUF_ROTATE_UPSIDEDOWN);
			else
			if (angle == 270)
				imageT = imageT->rotate_simple(Gdk::PIXBUF_ROTATE_COUNTERCLOCKWISE);
		}
		
		void skieruj(int where)//w prawo
		{
			if (mode)
			{
				reset(); obrot_w_prawo(where);
			}
		}		
		
		void set_mode(bool m)
		{
			mode = m;
		}
		
		bool get_mode() const
		{
			return mode;
		}	

		void draw(Glib::RefPtr<Gdk::Window> gw,Glib::RefPtr<Style> gs) 
		{//UWAGA: przywraca animacje z przesunieciem
			int u = wektor.getx();
			int v = wektor.gety();
			
				if (!mode)
					imageT->render_to_drawable(gw,gs->get_black_gc(),0,0,uchwyt.getx(),uchwyt.gety(),imageS->get_width(),imageS->get_height(), 		  	 		Gdk::RGB_DITHER_NONE,0,0);
				else 
				{  //animuje
					imageT->render_to_drawable(gw,gs->get_black_gc(),0,0,uchwyt.getx()+u,uchwyt.gety()+v,imageS->get_width(),
					imageS->get_height(),Gdk::RGB_DITHER_NONE,0,0);
					
					uchwyt.set_coor(uchwyt.getx()+u,uchwyt.gety()+v);
				}
			
			mode = true;	
		}
};


class Gracz : public Postac
{
	int kieszen,dl_nietykalny,dl_teleport;
	
	void abc() {}
	
	public:
		Gracz(string dir,int a,int b) : Postac(dir,a,b),kieszen(0),dl_nietykalny(0),dl_teleport(0) {}
		
		int get_kieszen() const
		{
			return kieszen;
		}
		
		void set_kieszen(int k)
		{
			kieszen = k;
		}
		
		void set_nietykalnosc(int k)
		{
			dl_nietykalny = k;
		}
		
		void set_teleport(int k)
		{
			dl_teleport = k;
		}
		
		int get() const
		{
			return dl_nietykalny;
		}
		
		int get_teleport() const
		{
			return dl_teleport;
		}
		
		void dec_nietykalnosc()
		{
			if (dl_nietykalny>0) --dl_nietykalny;
		}
		
		void dec_teleport()
		{
			if (dl_teleport>0) --dl_teleport;
		}

};


class Zlodziej : public Postac
{
	void abc() {}

	public:
		Zlodziej(string dir,int a,int b) : Postac(dir,a,b) {}
		
		void ustaw_czynnosc()
		{//prawdopodobienstwo "rozejrzyj się" i "idź gdzieś" rozklada sie odpowiednio jako 0.5 i 0.5
		
			static const struct paczka 
					{
						int u,v,kat;
					} 
					IT[] = {{0,-30,0},{0,30,180},{30,0,90},{-30,0,270}};
			
			short k = rand()%2; //można jako klase
			Point pred(wektor.getx(),wektor.gety());
			
				if (k == 0)
				{//rozejrzyj się
					do		
						k = rand()%4;
					while ( (IT[k].u == pred.getx())&&(IT[k].v == pred.gety()) );
					
					set_vec(IT[k].u,IT[k].v);
					skieruj(IT[k].kat);
					set_mode(false);
				}
		}

};

#endif

