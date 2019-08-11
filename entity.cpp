//Code responsible for managing Entities like NPCs and Items
//-Silvia and Robert

#include <iostream>
#include <algorithm>
#include <vector>
#include <memory>
#include <string>
using namespace std;
#include "libsqlite.hpp" 

//Silvias code

class Entity{
    public: 
        int id;
        int posX;
        int posY;
        int mapchar;
        int mapCharColour;
		string name;
};

//creating a subclass of Entity
class NPC : public Entity{
    public:
    //local attributes
        float maxHealth;
        float currentHealth;
        int level;
        int attack;
        float maxExperience;
        float currentExp;
        float magic;
        int mana= 50;
        void pick_item(int, int);
        void get_exp();
        void level_up();
        
        
        
        NPC(int a, int b, int c, int d, string e,int f){
            maxHealth = c;
            currentHealth=c;
            attack=10;
            magic=10;
            mana= 30;
            mapchar = a;
            level = 1; 
            mapCharColour=b;
            name=e;
            id=f;
            currentExp=0;
            maxExperience=100;
    };
};

//reach the database through NPC method
NPC createNPC(int id){
    string sqliteFile = "gameData.sqlite";

    try
    {
        sqlite::sqlite db( sqliteFile );
        auto s = db.get_statement(); 

    s->set_sql( "SELECT * FROM NPC WHERE npc_id = ?;" );
    s->prepare();
    s->bind( 1, id );
    s->step();
    NPC tempNPC(s->get_int(1),s->get_int(2),s->get_int(3),s->get_int(4),s->get_text(5),id);
    return tempNPC; 
    }    
    catch( sqlite::exception e )
        {
        std::cerr << e.what() << std::endl;
        }
};

//Robert s code


//Item definition 
class Item : public Entity {  //change NPC in Entity in the real code
    public :      //local attributes
      string type;
      int value;
    void generate(int);
    string getName();
  // string notification();
    // ???? view() 
  
  
};

//Item methods and other useful functions

void Item::generate(int ID) //it creates the Item identified with id
{
  id=ID;
  sqlite::sqlite db( "gameData.sqlite");
  //get the colour id from the data base
  auto cur = db.get_statement();
  cur->set_sql("SELECT txtCol_id FROM items WHERE item_id = ?;");
  cur->prepare();
  cur->bind(1, id);
  cur->step();
  mapCharColour= cur->get_int(0);
  //get the char id from the data base
  cur = db.get_statement();
  cur->set_sql("SELECT char_id FROM items WHERE item_id = ?;");
  cur->prepare();
  cur->bind(1,id);
  cur->step();
  mapchar = cur->get_int(0);
  cur= db.get_statement();
  cur->set_sql("SELECT i_value FROM items WHERE item_id = ?;");  //get the value of item
  cur->prepare();
  cur->bind(1, id);
  cur->step();
  value = cur->get_int(0);
  cur= db.get_statement();
  cur->set_sql("SELECT i_name FROM items WHERE item_id = ?;");  //get the value of item
  cur->prepare();
  cur->bind(1, id);
  cur->step();
  name = cur->get_text(0);
}

void NPC::pick_item(int item_ID, int item_Value){
  //first digit of ID indicates item's type
  if(item_ID/100 == 3)
    attack = attack + item_Value;    //weapon item type increases atack power
  else if(item_ID/100 == 5)  
    magic = magic + item_Value;      //staff item type increases magic power
  else if(item_ID/100 == 4)
  {
    maxHealth= maxHealth + item_Value;
    currentHealth = currentHealth + item_Value;
  }                                //shields increase health
  else if(item_ID/100 == 6 && item_ID%10 == 0)
    currentHealth = maxHealth;
  else
    mana = mana + item_Value;
                                     
}

int pick_id(int player_level,int &exist) //remember to make exist 0 every time player levels up
{
  int id =600;
  if(player_level<=5 && exist<100)
  {
    if(exist%10==3)
    {
      id = 400+ player_level - 1;
      exist = exist*10 + 4;
      
    }
  else if (exist % 10 == 4)
    {
      id = 500 + player_level -1;
      exist = exist*10 + 5;
    }
  else {
        id = 300 + player_level -1;
        exist = exist*10 +3;
         }
   }
   if (player_level == 6)
   {
     if(exist%10 == 3)
       {
         id = 400+ player_level -1;
         exist = exist*10 +4;
     }
     else{
       id =300 + player_level -1;
     }
   }
    if (player_level ==7 && exist ==0)
      id = 307;
      
  return id;
}

void NPC::level_up()
{
  level= level +1;
  maxExperience = maxExperience + 0.33* maxExperience ;    //increase exp bar
  currentExp = 0 ;                 //reset current exp
  maxHealth =  maxHealth  + 0.25* maxHealth;   //increase hp
  currentHealth = maxHealth; // hp is full after you levle up
  attack = attack + 0.1 * attack;    //increase attack damage
  magic = magic + 0.15 * magic;   //increase magic damage
  mana += 10;    //increase mana
};  
  
void NPC::get_exp()
{
  currentExp = currentExp + 0.30* maxExperience + 28;    //currentExp increases
};

string Item::getName()
{
  string itsCalled;
  sqlite::sqlite db( "items.sqlite");
  auto nur= db.get_statement();
  nur->set_sql("SELECT Name FROM items WHERE item_id = ?;");  //get the name of item
  nur->prepare();
  nur->bind(1, id);
  nur->step();
  itsCalled = nur->get_text(0);
  return itsCalled;
}
