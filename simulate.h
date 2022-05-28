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
class TextureMap {
  //contains texture mappings for use when assigning textures to sprites
public:
  std::vector<sf::Texture> mapping;
  sf::Texture& getTexture(const int&);
  void assign(int, sf::Texture);
  bool initialize(const std::string&);
  int size();
};

void assignTextureToNode(sf::Sprite&, TextureMap&, int);

int maxSub(int);
#endif // SIMULATE_H_INCLUDED
