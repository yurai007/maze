#include "controller.hpp"
#include <iostream>

namespace control
{

char controller::get_direction() const
{
	return direction;
}

void controller::reset_direction()
{
	direction = 0;
}

bool controller::on_key_press_event(GdkEventKey *event)
{
	bool up = (event->keyval == GDK_KEY_Up);
	bool down = (event->keyval == GDK_KEY_Down);
	bool left = (event->keyval == GDK_KEY_Left);
	bool right = (event->keyval == GDK_KEY_Right);

	if (up)
		direction = 'U';
	if (down)
		direction = 'D';
	if (left)
		direction = 'L';
	if (right)
		direction = 'R';
	return true;
}

bool controller::on_key_release_event(GdkEventKey *)
{
	return true;
}

}




