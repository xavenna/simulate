#include <iostream>
#include <vector>
#include <deque>
#include <fstream>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "simulate.h"

//class member function definitions
int Node::getId() {
    return id;
}
void Node::setSubId(int x) {
    subid = x;
}
int Node::getSubId() {
    return subid;
}
void Node::setId(int x) {
    id = x;
}
void Node::setUpdate(bool x) {
    update = x;
}
bool Node::getUpdate() {
    return update;
}
void Node::setDirection(int x) {
    direction = x;
}
int Node::getDirection() {
    return direction;
}
void Node::setHeat(float x) {
    heat = x;
}
float Node::getHeat() {
    return heat;
}
Node::Node() {
    subid = 0;
    id = 0;
    heat = 5.f;
    direction = rand() % 2;
}
int Cursor::getXPos() {
    return xpos;
}
int Cursor::getYPos() {
    return ypos;
}
void Cursor::setXPos(int x) {
    xpos = x;
}
void Cursor::setYPos(int x) {
    ypos = x;
}
int Cursor::getId() {
    return id;
}
void Cursor::setId(int x) {
    id = x;
}
int Cursor::getSubId() {
    return subid;
}
void Cursor::setSubId(int x) {
    subid = x;
}
void Cursor::update() {
    area.setPosition(xpos*8, 16+ypos*8);
}



sf::Texture& TextureMap::getTexture(const int& n) {
  return mapping.at(n);
}
void TextureMap::assign(int n, sf::Texture tex) {
  try {
    mapping.at(n) = tex;
  }
  catch(...) {
    std::cout << "Error: Invalid\n";
  }
}
bool TextureMap::initialize(const std::string& name) {
  mapping.resize(0);
  std::ifstream get(name);
  std::string line;
  if(!get.is_open()) {
    return false;
  }
  while(get.peek() != EOF) {
    sf::Texture te;
    mapping.push_back(te);
    std::getline(get, line);
    line = "assets/texture/" + line;
    mapping.back().loadFromFile(line);
  }
  return true;
}
int TextureMap::size() {
  return mapping.size();
}

void assignTextureToNode(sf::Sprite& sprite, TextureMap& textureMap, int id) {
  sprite.setTexture(textureMap.mapping.at(id));
}

//miscellaneous utility functions
int maxSub(int x) { //makes subid switching work
    switch(x) {
    case 0:
        return 0;
        break;
    case 1:
        return 2;
        break;
    case 2:
        return 2;
        break;
    case 3:
        return 0;
        break;
    case 4:
        return 0;
        break;
    default:
        return 0;
        break;
    }
}
