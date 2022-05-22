#ifndef SIMULATE_H_INCLUDED
#define SIMULATE_H_INCLUDED
class Node {
public:
    int getId();
    void setId(int);
    int getSubId();
    void setSubId(int);
    void setUpdate(bool);
    bool getUpdate();
    void setDirection(int);
    int getDirection();
    void setHeat(float);
    float getHeat();
    sf::Sprite area;
    Node();
private:
    int id;
    int subid; //used so different liquids/gasses are visually different, but the mechanics don't have to be redone and complicated
    int direction;
    float heat;
    bool update;
};
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
class Cursor {
public:
    sf::Sprite area;
    int getXPos();
    int getYPos();
    void setXPos(int);
    void setYPos(int);
    int getId();
    void setId(int);
    int getSubId();
    void setSubId(int);
    void update();
private:
    int xpos;
    int ypos;
    int id;
    int subid;
};
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
#endif // SIMULATE_H_INCLUDED
