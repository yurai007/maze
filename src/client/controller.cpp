#include "controller.hpp"
#include <iostream>

namespace control
{

char controller::get_direction() const
{
    return direction;
}

bool controller::is_space_on() const
{
    return is_space;
}

void controller::reset()
{
	direction = 0;
    is_space = false;
}

bool controller::on_key_press_event(GdkEventKey *event)
{
	bool up = (event->keyval == GDK_KEY_Up);
	bool down = (event->keyval == GDK_KEY_Down);
	bool left = (event->keyval == GDK_KEY_Left);
	bool right = (event->keyval == GDK_KEY_Right);
    is_space = (event->keyval == GDK_KEY_space);

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




