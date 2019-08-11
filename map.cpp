//Code in charge of handling the main Map object for use in levels
//-Francisco

#include <iostream>
#include <vector>
using namespace std;

class Map 
  
{
  public:
  
  int width, height;
  vector<int> mapdata;
  vector<tuple<int,int,int>> entityData;
  void savemap(string lMapname);
  void saveEntities(string lMapname);
  
  struct tile
  {
   int mapcolour;
   int mapchar;
   int mapcharcolour;
   bool passable;   
  };
    
  vector<tile> tiles;
  vector<Entity> entities;
  vector<NPC> NPCs;
  vector<Item> Items;
  
  
  Map( int a, int b, vector<int> c,vector<tuple<int,int,int>> e) //entityData: tuple<int,int,int>(id,posX,posY) 
  {
    width = a;
    height = b;
    mapdata = c;
    entities= {};
    NPCs= {};
    entityData=e;
    
    
    for(int i = mapdata.size(); i<width*height;i++)
    {
      mapdata.emplace_back(0);
    }
    
    
  }
  
  tile createTile(int mapcolour,int mapchar,int mapcharcolour, bool passable){
	  tile t{mapcolour,mapchar,mapcharcolour,passable};
  	return t;
  }

  void addTile(int mapcolour,int mapchar,int mapcharcolour, bool passable)
  {
  	tile t=createTile(mapcolour,mapchar,mapcharcolour,passable);
    tiles.emplace_back(t);
  
  }
  
  int getTile(int x, int y)
   
  {
   
    return mapdata[width*y+x];
  
  }
};
                
void Map::savemap(string lMapName){
       try{
				sqlite::sqlite db( "gameData.sqlite" );
				auto cur = db.get_statement(); 
        cur->set_sql("DROP TABLE IF EXISTS "+lMapName+"_mapdata;");
				cur->exec();
        cur = db.get_statement();
				cur->set_sql("CREATE TABLE "+lMapName+"_mapdata (index_id NUMBER(3) PRIMARY KEY,tile_id NUMBER(3),FOREIGN KEY (tile_id) REFERENCES chars(tile_id));");
				cur->exec();
				for (int i=0;i<mapdata.size();i++){
					cur= db.get_statement(); 
					cur->set_sql("INSERT INTO "+lMapName+"_mapdata VALUES (?,?);");
					cur->prepare();
					cur->bind(1,i);
					cur->bind(2,mapdata[i]);
					cur->step();
        }            
        cur= db.get_statement(); 
				cur->set_sql("COMMIT;");
				cur->exec();
                    
			}
      catch (sqlite::exception e){
				cerr << e.what() << endl;
			}
}
    
void Map::saveEntities(string lMapName){
  /*
  Function that saves the entities in the map to an sql database
  -Silvia
  */
     try{
				sqlite::sqlite db( "gameData.sqlite" );
				auto cur = db.get_statement(); 
                cur->set_sql("DROP TABLE IF EXISTS "+lMapName+"_mapEntityData;");
				cur->exec();
				cur= db.get_statement(); 
				cur->set_sql("CREATE TABLE "+lMapName+"_mapEntityData (index_id NUMBER(3) PRIMARY KEY,entity_id NUMBER(3),posX NUMBER(2),posY NUMBER(2));");
				cur->exec();
				for (int i=0;i<entityData.size();i++){
					cur= db.get_statement(); 
					cur->set_sql("INSERT INTO "+lMapName+"_mapEntityData VALUES (?,?,?,?);");
					cur->prepare();
					cur->bind(1,i);
					cur->bind(2,get<0>(entityData[i])); //saving vectors of tuples
					cur->bind(3,get<1>(entityData[i]));
					cur->bind(4,get<2>(entityData[i]));
					cur->step();
				}
				cur= db.get_statement(); 
				cur->set_sql("COMMIT;");
				cur->exec();
                    
			}
      catch (sqlite::exception e){
				cerr << e.what() << endl;
			}
}