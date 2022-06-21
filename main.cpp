#include <iostream>
#include <SFML/Graphics.hpp>
#include "simulate.h"
//to do:
//make it so fire can't destroy other things
//restructure code to make it less of an awful bowl of spaghetti
//  -in progress

int main()
{
  //initialization
  sf::RenderWindow window(sf::VideoMode(256, 272), "Simulate");
  window.setVerticalSyncEnabled(true);
  window.setFramerateLimit(30);
  Node world[32][32];
  Node select;
  select.area.setPosition(0,0);
  select.area.setScale(2,2);
  for(int i=0;i<32;i++) {
    for(int j=0;j<32;j++) {
      world[i][j].area.setPosition(8*i,16+(8*j));
    }
  }
  // (attempt to) integrate the texture map system from stellar-enigma 
  TextureMap textureMap;
  if(!textureMap.initialize("assets/texturemap/default.tm"))  //default texture map, stored in a file
    std::cout << "Error: texture map not found\n";
  //maybe make a function to generate this dynamically using maxSub
  std::vector<int> subIdMapping = {0, 1, 4, 7, 8};
  sf::Texture pointer;
  pointer.loadFromFile("assets/point.png");
  sf::Texture title;
  title.loadFromFile("assets/title.png");

  //Heat is on a scale of 0-10; 0-2 = ice, 3-6 = liquid, 7-10 = gas, 9-10 = fire
  Cursor point;
  point.setXPos(0);
  point.setYPos(0);
  point.setId(1);
  point.area.setTexture(pointer);
  sf::Sprite top;
  top.setTexture(title);
  while (window.isOpen()) {
    //event polling
    sf::Event event;
    float l[32][32]; //holds temporary heat values
    while (window.pollEvent(event)) { //poll events
      switch(event.type) {
      case sf::Event::Closed:
	window.close();
	break;
      case sf::Event::KeyPressed:
	switch(event.key.code) {
	case sf::Keyboard::A: //move cursor to the left
	  point.setXPos((point.getXPos() != 0) ? point.getXPos()-1 : point.getXPos());
	  break;
	case sf::Keyboard::D: //move cursor to the right
	  point.setXPos((point.getXPos() != 31) ? point.getXPos()+1 : point.getXPos());
	  break;
	case sf::Keyboard::W: //move cursor up
	  point.setYPos((point.getYPos() != 0) ? point.getYPos()-1 : point.getYPos());
	  break;
	case sf::Keyboard::S: //move cursor down
	  point.setYPos((point.getYPos() != 31) ? point.getYPos()+1 : point.getYPos());
	  break;
	case sf::Keyboard::E: //cycle material (positive)
	  point.setId((point.getId() != 4) ? point.getId()+1: 0); //change 4 to max base material value
	  if(point.getSubId() > maxSub(point.getId()))
	    point.setSubId(maxSub(point.getId()));
	  break;
	case sf::Keyboard::Q: //cycle material (negative)
	  point.setId((point.getId() != 0) ? point.getId()-1: 4); //change 4 to max base material value
	  if(point.getSubId() > maxSub(point.getId()))
	    point.setSubId(maxSub(point.getId()));
	  break;
	case sf::Keyboard::LBracket: //cycle submaterial (positive)
	  point.setSubId((point.getSubId() != maxSub(point.getId())) ? point.getSubId()+1 : 0);
	  break;
	case sf::Keyboard::RBracket: //cycle submaterial (negative)
	  point.setSubId((point.getSubId() != 0) ? point.getSubId()-1 : maxSub(point.getId()));
	  break;
	case sf::Keyboard::Return: //place material selected by cursor
	  world[point.getXPos()][point.getYPos()].setId(point.getId());
	  world[point.getXPos()][point.getYPos()].setSubId(point.getSubId());
	  world[point.getXPos()][point.getYPos()].setUpdate(true);
	  if(world[point.getXPos()][point.getYPos()].getId() == 3)
	    world[point.getXPos()][point.getYPos()].setHeat(7.5);
	  if(world[point.getXPos()][point.getYPos()].getId() == 1 && world[point.getXPos()][point.getYPos()].getSubId() == 0)
	    world[point.getXPos()][point.getYPos()].setHeat(5);
	  if(world[point.getXPos()][point.getYPos()].getId() == 1 && world[point.getXPos()][point.getYPos()].getSubId() == 1)
	    world[point.getXPos()][point.getYPos()].setHeat(0);
	  if(world[point.getXPos()][point.getYPos()].getId() == 4)
	    world[point.getXPos()][point.getYPos()].setHeat(10);
	  if(world[point.getXPos()][point.getYPos()].getId() == 2 && world[point.getXPos()][point.getYPos()].getSubId() == 1)
	    world[point.getXPos()][point.getYPos()].setHeat(2);
	  if(world[point.getXPos()][point.getYPos()].getId() == 1 && world[point.getXPos()][point.getYPos()].getSubId() == 2)
	    world[point.getXPos()][point.getYPos()].setHeat(10);
	  break;
	case sf::Keyboard::L:
	  window.setFramerateLimit(30);
	  break;
	case sf::Keyboard::SemiColon:
	  window.setFramerateLimit(10);
	  break;
	case sf::Keyboard::Comma:
	  window.setFramerateLimit(4);
	  break;
	case sf::Keyboard::Period:
	  for(int i=0;i<32;i++) {
	    for(int j=0;j<32;j++) {
	      world[i][j].setId(0);
	      world[i][j].setSubId(0);
	      world[i][j].setHeat(5);
	    }
	  }
	  break;
	case sf::Keyboard::Quote:
	  std::cout << "Heat value is " << world[point.getXPos()][point.getYPos()].getHeat() << ".\n";
	  break;
	default:
	  break;
	}
	break;
      default:
	break;
      }
    }
    //end event polling

    
    for(int i=0;i<32;i++) { //movement resolution
      for(int j=0;j<32;j++) {
	bool side = false;
	//new water movement algorithm:
	if(world[i][j].getId() == 1 && world[i][j].getUpdate() == false) { // if water
	  if(j != 31){ //test if water falls
	    if(world[i][j+1].getId() != 2 && world[i][j+1].getId() != 1 && world[i][j+1].getId() != 3 && world[i][j+1].getId() != 4) {  //I honestly have no idea how this works... hope i can fix the problem
	      world[i][j+1].setId(1);
	      world[i][j+1].setUpdate(true);
	      world[i][j+1].setDirection(rand() % 2);
	      world[i][j+1].setHeat(world[i][j].getHeat());
	      world[i][j+1].setSubId(world[i][j].getSubId());
	      world[i][j].setId(0);
	      world[i][j].setSubId(0);
	      world[i][j].setUpdate(true);
	      side = false;
	    }
	    else if(world[i][j+1].getId() == 3) {  //if water falls onto steam
	      world[i][j+1].setHeat(world[i][j].getHeat());  //make it so heat and subid are transferred both ways
	      world[i][j+1].setSubId(world[i][j].getSubId());
	      world[i][j].setId(3);
	      world[i][j].setUpdate(true);
	      world[i][j+1].setId(1);
	      world[i][j+1].setUpdate(true);
	      world[i][j].setDirection(rand() % 2);
	      world[i][j+1].setDirection(rand() % 2);
	    }
	    else {
	      side = true;
	    }
	  }
	  else { //sets up things properly
	    side = true;
	  }
	  
	  if(world[i][j].getDirection() == 0 && side == true) { //if water moves right
	    if(i != 31) {
	      if(world[i+1][j].getId() == 0) {
		world[i+1][j].setDirection(world[i][j].getDirection());
		world[i+1][j].setHeat(world[i][j].getHeat());
		world[i+1][j].setSubId(world[i][j].getSubId());
		world[i][j].setId(0);
		world[i][j].setSubId(0);
		world[i][j].setUpdate(true);
		world[i+1][j].setId(1);
		world[i+1][j].setUpdate(true);
	      }
	      else if(world[i+1][j].getId() == 1 && world[i][j].getUpdate() == false) {
		int k=0;
		for(k=i;k<32;k++) {
		  if(world[k][j].getId() == 0) {
		    world[i][j].setId(0);
		    world[i][j].setUpdate(true);
		    world[k][j].setId(1);
		    world[k][j].setUpdate(true);
		    world[k][j].setDirection(0);
		    break;
		  }
		  if(world[k][j].getId() == 2) {
		    for(int l=i;l<k;l++) {
		      world[l][j].setDirection(1);
		    }
		    break;
		  }
		  if(k == 31) {
		    for(int l=i;l<32;l++) {
		      world[l][j].setDirection(1);
		    }
		  }
		  world[k][j].setDirection(0);
		  world[k][j].setUpdate(true);
		}
		if(k <= 31) { //if this breaks, change 30 to 31
		  bool suc = true;
		  if(k == 31)
		    suc = false;
		  else if(world[k+1][j].getId() != 2)
		    suc = false;
		  else
		    suc = true;
		  if(suc == false) {
		    for(int p=k;p>i;p--) {
		      if(world[p][j].getId() == 1) {
			world[p][j].setSubId(world[p-1][j].getSubId());
			world[p][j].setHeat(world[p-1][j].getHeat());
		      }
		    }
		  }
		  if(world[i][j].getId() != 1) {
		    world[i][j].setSubId(0);
		    world[i][j].setHeat(5);
		  }
		}
		else {
		  
		}
	      }
	      else if(world[i+1][j].getId() == 3) {
		//water and steam interactions are undefined
	      }
	      else { //if world[i+1][j] is rock
		world[i][j].setDirection(1);
		if(i != 0) {
		  if(world[i-1][j].getId() == 0) {
		    world[i-1][j].setDirection(world[i][j].getDirection());
		    world[i-1][j].setHeat(world[i][j].getHeat());
		    world[i-1][j].setSubId(world[i][j].getSubId());
		    world[i][j].setId(0);
		    world[i][j].setUpdate(true);
		    world[i-1][j].setId(1);
		    world[i-1][j].setUpdate(true);
		  }
		  else if(world[i-1][j].getId() == 1) {
		    //do something
		  }
		  else if(world[i-1][j].getId() == 3) {
		    //water and steam interactions are undefined
		  }
		  else {
		    //water doesn't move
		  }
		}
		//water doesn't move
	      }
	    }
	    else { //if i=31
	      world[i][j].setDirection(1);
	      if(world[i-1][j].getId() == 0) {
		world[i-1][j].setDirection(world[i][j].getDirection());
		world[i-1][j].setHeat(world[i][j].getHeat());
		world[i-1][j].setSubId(world[i][j].getSubId());
		world[i][j].setId(0);
		world[i][j].setSubId(0);
		world[i][j].setUpdate(true);
		world[i-1][j].setId(1);
		world[i-1][j].setUpdate(true);
	      }
	      else if(world[i-1][j].getId() == 1) {
		//do something
	      }
	      else if(world[i-1][j].getId() == 3) {
		//water and steam interactions are undefined
	      }
	      else {
		//for any solid things
	      }
	    }
	  }
	  else if(side == true) { //if it moves left
	    if(i != 0) {
	      if(world[i-1][j].getId() == 0) {
		world[i-1][j].setDirection(world[i][j].getDirection());
		world[i-1][j].setHeat(world[i][j].getHeat());
		world[i-1][j].setSubId(world[i][j].getSubId());
		world[i][j].setId(0);
		world[i][j].setSubId(0);
		world[i][j].setUpdate(true);
		world[i-1][j].setId(1);
		world[i-1][j].setUpdate(true);
	      }
	      else if(world[i-1][j].getId() == 1) {
		//do something
	      }
	      else if(world[i-1][j].getId() == 3) {
		//water and steam interactions are undefined
	      }
	      else {
		world[i][j].setDirection(0);
		if(i != 31) {
		  if(world[i+1][j].getId() == 0) {
		    world[i+1][j].setDirection(world[i][j].getDirection());
		    world[i+1][j].setHeat(world[i][j].getHeat());
		    world[i+1][j].setSubId(world[i][j].getSubId());
		    world[i][j].setId(0);
		    world[i][j].setUpdate(true);
		    world[i+1][j].setId(1);
		    world[i+1][j].setUpdate(true);
		  }
		  else if(world[i+1][j].getId() == 1) {
		    //do something
		  }
		  else if(world[i+1][j].getId() == 3) {
		    //water and steam interactions are undefined
		  }
		  else {
		    //water doesn't move
		  }
		}
		//water doesn't move
	      }
	    }
	    else { //if i = 0
	      world[i][j].setDirection(0);
	      if(world[i+1][j].getId() == 0) {
		world[i+1][j].setHeat(world[i][j].getHeat());
		world[i+1][j].setSubId(world[i][j].getSubId());
		world[i][j].setId(0);
		world[i][j].setSubId(0);
		world[i][j].setUpdate(true);
		world[i+1][j].setId(1);
		world[i+1][j].setUpdate(true);
		world[i+1][j].setDirection(0);
	      }
	      else if(world[i+1][j].getId() == 1) {
		//do something
	      }
	      else if(world[i+1][j].getId() == 3) {
		//water and steam interactions are undefined
	      }
	      else {
		//any solid block
	      }
	    }
	  }
	}
	else if(world[i][j].getId() == 3 && world[i][j].getUpdate() == false) { //if selected object is a gas
	  side = false;
	  if(j != 0) { //test if gas floats
	    if(world[i][j-1].getId() != 2 && world[i][j-1].getId() != 3 && world[i][j-1].getId() != 1 && world[i][j-1].getId() != 4) {
	      world[i][j-1].setHeat(world[i][j].getHeat());
	      world[i][j-1].setSubId(world[i][j].getSubId());
	      world[i][j].setId(0);
	      world[i][j].setUpdate(true);
	      world[i][j-1].setId(3);
	      world[i][j-1].setUpdate(true);
	      world[i][j-1].setDirection(rand() % 2);
	      side = false;
	    }
	    else {
	      side = true;
	    }
	  }
	  else { //sets up things properly
	    side = true;
	  }
	  if(world[i][j].getDirection() == 0 && world[i][j].getUpdate() == false) { //if gas moves right
	    if(i != 31) {
	      if(world[i+1][j].getId() == 0) {
		world[i+1][j].setDirection(world[i][j].getDirection());
		world[i+1][j].setHeat(world[i][j].getHeat());
		world[i+1][j].setSubId(world[i][j].getSubId());
		world[i][j].setId(0);
		world[i][j].setUpdate(true);
		world[i+1][j].setId(3);
		world[i+1][j].setUpdate(true);
	      }
	      else if(world[i+1][j].getId() == 3 && world[i][j].getUpdate() == false) {
		int k=0;
		for(k=i;k<32;k++) {
		  if(world[k][j].getId() == 0) {
		    world[i][j].setId(0);
		    world[i][j].setUpdate(true);
		    world[k][j].setId(3);
		    world[k][j].setUpdate(true);
		    world[k][j].setDirection(0);
		    break;
		  }
		  if(world[k][j].getId() == 2) {
		    for(int l=i;l<k;l++) {
		      world[l][j].setDirection(1);
		    }
		    break;
		  }
		  if(k == 31) {
		    for(int l=i;l<32;l++) {
		      world[l][j].setDirection(1);
		    }
		  }
		  world[k][j].setDirection(0);
		  world[k][j].setUpdate(true);
		}
		if(k <= 31) { //if this breaks, change 30 to 31
		  bool suc = true;
		  if(k == 31)
		    suc = false;
		  else if(world[k+1][j].getId() != 2)
		    suc = false;
		  else
		    suc = true;
		  if(suc == false) {
		    for(int p=k;p>i;p--) {
		      if(world[p][j].getId() == 3) {
			world[p][j].setSubId(world[p-1][j].getSubId());
			world[p][j].setHeat(world[p-1][j].getHeat());
		      }
		    }
		  }
		  if(world[i][j].getId() != 1) {
		    world[i][j].setSubId(0);
		    world[i][j].setHeat(5);
		  }
		}
		else {
		  
		}
	      }
	      else if(world[i+1][j].getId() == 1) {
		//water and steam interactions are undefined
	      }
	      else { //if world[i+1][j] is rock
		world[i][j].setDirection(1);
		if(i != 0) {
		  if(world[i-1][j].getId() == 0) {
		    world[i-1][j].setDirection(world[i][j].getDirection());
		    world[i-1][j].setHeat(world[i][j].getHeat());
		    world[i-1][j].setSubId(world[i][j].getSubId());
		    world[i][j].setId(0);
		    world[i][j].setUpdate(true);
		    world[i-1][j].setId(3);
		    world[i-1][j].setUpdate(true);
		  }
		  else if(world[i-1][j].getId() == 3) {
		    //do something
		  }
		  else if(world[i-1][j].getId() == 1) {
		    //water and steam interactions are undefined
		  }
		  else {
		    //water doesn't move
		  }
		}
		//water doesn't move
	      }
	    }
	    else { //if i=31
	      world[i][j].setDirection(1);
	      if(world[i-1][j].getId() == 0) {
		world[i-1][j].setDirection(world[i][j].getDirection());
		world[i-1][j].setHeat(world[i][j].getHeat());
		world[i-1][j].setSubId(world[i][j].getSubId());
		world[i][j].setId(0);
		world[i][j].setUpdate(true);
		world[i-1][j].setId(3);
		world[i-1][j].setUpdate(true);
	      }
	      else if(world[i-1][j].getId() == 3) {
		//do something
	      }
	      else if(world[i-1][j].getId() == 1) {
		//water and steam interactions are undefined
	      }
	      else {
		//for any solid things
	      }
	    }
	  }
	  else if(world[i][j].getDirection() == 1 && world[i][j].getUpdate() == false) { //if gas moves left
	    if(i != 0) {
	      if(world[i-1][j].getId() == 0) {
		world[i-1][j].setDirection(world[i][j].getDirection());
		world[i-1][j].setHeat(world[i][j].getHeat());
		world[i-1][j].setSubId(world[i][j].getSubId());
		world[i][j].setId(0);
		world[i][j].setUpdate(true);
		world[i-1][j].setId(3);
		world[i-1][j].setUpdate(true);
	      }
	      else if(world[i-1][j].getId() == 3) {
		//do something
	      }
	      else if(world[i-1][j].getId() == 1) {
		//water and steam interactions are undefined
	      }
	      else {
		world[i][j].setDirection(0);
		if(i != 31) {
		  if(world[i+1][j].getId() == 0) {
		    world[i+1][j].setDirection(world[i][j].getDirection());
		    world[i+1][j].setHeat(world[i][j].getHeat());
		    world[i+1][j].setSubId(world[i][j].getSubId());
		    world[i][j].setId(0);
		    world[i][j].setUpdate(true);
		    world[i+1][j].setId(3);
		    world[i+1][j].setUpdate(true);
		  }
		  else if(world[i+1][j].getId() == 3) {
		    //do something
		  }
		  else if(world[i+1][j].getId() == 1) {
		    //water and steam interactions are undefined
		  }
		  else {
		    //water doesn't move
		  }
		}
		//water doesn't move
	      }
	    }
	    else { //if i = 0
	      world[i][j].setDirection(0);
	      if(world[i+1][j].getId() == 0) {
		world[i+1][j].setHeat(world[i][j].getHeat());
		world[i+1][j].setSubId(world[i][j].getSubId());
		world[i][j].setId(0);
		world[i][j].setUpdate(true);
		world[i+1][j].setId(3);
		world[i+1][j].setUpdate(true);
		world[i+1][j].setDirection(0);
	      }
	      else if(world[i+1][j].getId() == 3) {
		//do something
	      }
	      else if(world[i+1][j].getId() == 1) {
		//water and steam interactions are undefined
	      }
	      else {
		//any solid block
	      }
	    }
	  }
	}
	else if(world[i][j].getId() == 4 && world[i][j].getUpdate() == false) {
	  if(world[i][j].getHeat() > 9.9 && world[i][j].getHeat() < 9.92) {
	    if(j > 0 && j < 31 && rand() % 3 == 1) {
	      world[i][j-1].setId(4);
	      world[i][j-1].setHeat(9.92);
	      world[i][j].setId(0);
	    }
	  }
	}
	//end of loop
      }
    }//motion handling

    for(int i=0;i<32;i++) { //heat transfer
      for(int j=0;j<32;j++) {
	float t = world[i][j].getHeat();
	if(i != 31) {
	  if(!(world[i+1][j].getId() == 0 || (world[i+1][j].getId() == 2 && world[i+1][j].getSubId() != 1)))
	    t = (t + world[i+1][j].getHeat()) / 2;
	}
	if(i != 0) {
	  if(!(world[i-1][j].getId() == 0 || (world[i-1][j].getId() == 2 && world[i-1][j].getSubId() != 1)))
	    t = (t + world[i-1][j].getHeat()) / 2;
	}
	if(j != 31) {
	  if(!(world[i][j+1].getId() == 0 || (world[i][j+1].getId() == 2 && world[i][j+1].getSubId() != 1)))
	    t = (t + world[i][j+1].getHeat()) / 2;
	}
	if(j != 0) {
	  if(!(world[i][j-1].getId() == 0 || (world[i][j-1].getId() == 2 && world[i][j-1].getSubId() != 1)))
	    t = (t + world[i][j-1].getHeat()) / 2;
	}
	t = ((t - world[i][j].getHeat()) / 6) + world[i][j].getHeat();
	l[i][j] = t;
      }
    }

    for(int i=0;i<32;i++) { //update heat values, change states
      for(int j=0;j<32;j++) {
	if(world[i][j].getId() != 0)  //l contains new heat values. If the space isn't empty, write heat values
	  world[i][j].setHeat(l[i][j]);
	else  //if space is empty, set heat to 5, default value.
	  world[i][j].setHeat(5);
	if(world[i][j].getId() == 1 && world[i][j].getSubId() != 2 && world[i][j].getHeat() >= 8)
	  world[i][j].setId(3);  //if space is water, isn't lava, and is hotter than 8, change to steam
	else if (world[i][j].getId() == 1 && world[i][j].getSubId() == 2) {
	  world[i][j].setHeat(world[i][j].getHeat() - 0.0016);  //if it is lava, decrease heat.
	  if (world[i][j].getHeat() <= 3) { //if it is cold enough, change to stone.
	    world[i][j].setId(2);
	    world[i][j].setSubId(0);
	  }
	}
	else if(world[i][j].getId() == 3 && world[i][j].getHeat() <= 7) {
	  world[i][j].setId(1);  //if steam is cold, change to water
	  world[i][j].setSubId(0);  //set it to normal water
	}
	else if(world[i][j].getId() == 4 && world[i][j].getHeat() <= 7.5)
	  world[i][j].setId(0);  //if fire gets too cold, it extinguishes
	else if(world[i][j].getId() == 4)
	  world[i][j].setHeat(world[i][j].getHeat() - 0.016);  //fire gets cooler
	else if(world[i][j].getId() == 2 && world[i][j].getSubId() == 1 && world[i][j].getHeat() >= 3) {
	  world[i][j].setId(1);  //if ice gets hot, it becomes water
	  world[i][j].setSubId(0);
	}
	else if(world[i][j].getId() == 2 && world[i][j].getSubId() == 2 && world[i][j].getHeat() >= 7.1) {
	  world[i][j].setId(4);  //if wood gets too hot, it becomes fire
	  world[i][j].setSubId(0);
	  world[i][j].setHeat(8.8);
	}
	else if(world[i][j].getId() == 1 && world[i][j].getSubId() == 0 && world[i][j].getHeat() <= 2) {
	  world[i][j].setId(2);  //if water gets too cold, it becomes ice
	  world[i][j].setSubId(1);
	}
	
      }
    }

    for(int i=0;i<32;i++) {
      for(int j=0;j<32;j++) {  //make sure that the subid transformation is always good and works (update: it doesn't)
	assignTextureToNode(world[i][j].area, textureMap, (subIdMapping[world[i][j].getId()]+world[i][j].getSubId()));
	world[i][j].setUpdate(false);
      }
    }

    assignTextureToNode(select.area, textureMap, (subIdMapping[point.getId()]+point.getSubId()));
    point.update();         //drawing to screen
    window.clear();
    for(int i=0;i<32;i++) {
      for(int j=0;j<32;j++) {
	window.draw(world[i][j].area);
      }
    }
    window.draw(point.area);
    window.draw(top);
    window.draw(select.area);
    window.display();
  }
  return 0;
}
