#include"Game.h"

int main()
{
	Game game("idk",1150,1100,4,6,false);

	//Main loop
	while (!game.getWindowShouldClose())
	{
		//uptade input 
		game.update();
		game.render();
		
	}
	return 0;
}

