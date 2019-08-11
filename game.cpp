#include <iostream>
#include <vector>
#include <map>
#include <random>
#include <math.h>
using namespace std;

#include "libsqlite.hpp"

#include <iostream>
#include <string>
#include "libsqlite.hpp"
#include <string.h>

using namespace std;



void updateHistory(string player_name, int count_items, int count_combat)
{ 
  /*
  Function that updates the leaderboard/hisotry
  -Robert
  */
  sqlite::sqlite db( "gameData.sqlite");   //data base connection
  auto lur = db.get_statement();
  lur->set_sql("INSERT INTO history VALUES(?,?,?);");
  lur->prepare();
  lur->bind(1, player_name);
  lur->bind(2, count_combat);
  lur->bind(3, count_items);
  lur->step();
  
  
}

void showLeaderboard(MainWindow& gui,int leaderboardGui,vector<string>& chars,vector<string>& colours,vector<string>& textColours)
{
	/*
	Function that displays leaderboard
	-Robert except where otherwise stated
	*/
  vector<tuple<string,int,int>> ranking={}; //kacper
  string leaderdisplay;
  sqlite::sqlite db( "gameData.sqlite");   //data base connection
  auto mur = db.get_statement();
  mur->set_sql("SELECT Name FROM history ORDER BY Wins DESC;");
  mur->prepare();
  while(mur->step())
    {
      string name = mur->get_text(0);
      auto vur = db.get_statement();
      vur->set_sql("SELECT Wins FROM history WHERE Name = ?;");
      vur->prepare();
      vur->bind(1, name);
      vur->step();
      int wins = vur->get_int(0);
      auto iur = db.get_statement();
      iur->set_sql("SELECT Items_Collected FROM history WHERE Name = ?;");
      iur->prepare();
      iur->bind(1, name);
      iur->step();           
      int items = iur->get_int(0);   
	  if (ranking.size()<15){ //kacper
      	ranking.emplace_back(make_tuple(name,wins,items));
	  }
     }
 gui.msgBox(leaderboardGui,0,0,"LEADERBOARD",4,28,8,0,58,21); //kacper
 gui.getWindow(leaderboardGui).drawText(1,2,"Rank | Name | Wins | Items"); //kacper
 for (int i=0;i<ranking.size();i++){ //kacper
	gui.getWindow(leaderboardGui).drawText(1,3+i,to_string(i+1)+" | "+get<0>(ranking[i])+" | "+to_string(get<1>(ranking[i]))+" | "+to_string(get<2>(ranking[i])));
 }
 gui.draw();
 waitForKeyPress();
 gui.closeWindow(leaderboardGui);
 }

string cleanString(string str){ //remove non alphanumeric characters from string to make joining safe, taken and adapted from http://www.cplusplus.com/forum/beginner/148876/
	string s=str;
	for(string::iterator i = s.begin(); i != s.end(); i++){
   		if(!isalpha(s.at(i - s.begin())))
			s.erase(i);
   }
	return s;
}

tuple<int,int> pick_tile (string lMapName){
	/*
	Pick a random walkable tile and return its coordinates
	-Francisco
	*/
	int width,height;
	vector<int> randtile={};
	try{                    
		sqlite::sqlite db( "gameData.sqlite" );
		auto cur = db.get_statement();
		cur->set_sql("SELECT * FROM "+cleanString(lMapName)+"_mapdata WHERE tile_id=? or tile_id=?;");
		cur->prepare();
		cur->bind(1,2);
		cur->bind(2,8);
		while (cur->step()){
		   randtile.emplace_back(cur->get_int(0));
		}
		cur = db.get_statement();
		cur->set_sql("SELECT * FROM "+lMapName+"_mapinfo WHERE attribute='width';");
		cur->prepare();
		cur->step();
		width=cur->get_int(1);
		cur = db.get_statement();
		cur->set_sql("SELECT * FROM "+lMapName+"_mapinfo WHERE attribute='height';");
		cur->prepare();
		cur->step();
		height=cur->get_int(1);
	}
	catch (sqlite::exception e){
		cerr << e.what() << endl;
	}
	// https://cpppatterns.com/patterns/choose-random-element.html
	std::random_device random_device;
	std::mt19937 engine{random_device()};
	std::uniform_int_distribution<int> dist(0, randtile.size() - 1);
	int rtile= randtile[dist(engine)];
	int y = rtile / width;
	int  x = rtile - width * y;
	return make_tuple(x,y);
}

DisplayWindow mapToWindow(Map Tmap,vector<string> chars,vector<string> colours,vector<string> txtColours){
	/*
	Converts a Map object into a DisplayWindow object
	-Kacper
	*/
	vector<int> backgroundColours={};
	vector<int> characters={};
	vector<int> characterColours={};
	for (int i;i<Tmap.mapdata.size();i++){
		backgroundColours.emplace_back(Tmap.tiles[Tmap.mapdata[i]].mapcolour);
		characters.emplace_back(Tmap.tiles[Tmap.mapdata[i]].mapchar);
		characterColours.emplace_back(Tmap.tiles[Tmap.mapdata[i]].mapcharcolour);
	}
	DisplayWindow rtrnMap(Tmap.width,Tmap.height,backgroundColours,characters,characterColours,chars,colours,txtColours);
	return rtrnMap;
}

void drawEntities(MainWindow& gui,vector<tuple<int,int,int>>& entityData,vector<Entity>& entities){
	/*
	Draw the entities in the level on the screen using the "one off" method in the GUI system
	-Kacper
	*/
	gui.oneOffs={};
	for (int i=0;i<entityData.size();i++){
		gui.oneOffs.emplace_back(make_tuple(get<1>(entityData[i])+1,get<2>(entityData[i])+1,entities[get<0>(entityData[i])].mapchar,-1,entities[get<0>(entityData[i])].mapCharColour));
	}
}

int interactWithEntity(MainWindow& gui,Map& gameMap,int entId,int playerIndex,map<string,string>& inventory,int& item_count,int& count_combat,vector<string>& chars,vector<string>& colours,vector<string>& textColours){
	/*
	Function responsible for handling all interactions between the player and entities in the game
	-Kacper
	*/
	if (gameMap.entities[get<0>(gameMap.entityData[entId])].id<100){ //if interacting with enemy
		gui.oneOffs={}; //clear the one offs from the gui to not overlay the entities on the screen
		combatMain(gameMap,gui,entId,playerIndex,count_combat,chars,colours,textColours);
	}
	else{
		int npcCount=0;
		for (int i=0;i<gameMap.entityData.size();i++){
			if (get<0>(gameMap.entityData[i])<100){
				npcCount++;
			}
		}
		vector<string> attributeNames={"attack","health","magic"};
		vector<string> itemTypes={"weapon","shield","staff"};
		gui.oneOffs={};
		int itemGui=gui.addWindow(0,0,{},{},{},chars,colours,textColours,0,0,50);
		int itemId=gameMap.entities[get<0>(gameMap.entityData[entId])].id;
		int itemValue=gameMap.Items[get<0>(gameMap.entityData[entId-npcCount])].value;
		//int itemId=301;
		//int itemValue=10;
		item_count++;
		gui.msgBox(itemGui,0,0,"You picked up a "+(gameMap.entities[get<0>(gameMap.entityData[entId])].name),4,28,8,0,58,21);
		gui.getWindow(itemGui).drawText(1,2,"and it increases your "+attributeNames[(itemId/100)-3]+" by "+to_string(itemValue));
		gui.draw();
		waitForKeyPress();
		inventory[itemTypes[(itemId/100)-3]]=gameMap.entities[get<0>(gameMap.entityData[entId])].name;
		//gameMap.NPCs[get<0>(gameMap.entityData[playerIndex])].pick_item(gameMap.Items[get<0>(gameMap.entityData[entId-gameMap.NPCs.size()+1])].id,gameMap.Items[get<0>(gameMap.entityData[entId-gameMap.NPCs.size()+1])].value);
		gameMap.NPCs[get<0>(gameMap.entityData[playerIndex])].pick_item(itemId,itemValue);
		gameMap.entityData.erase(gameMap.entityData.begin()+entId);
		gui.closeWindow(itemGui);
	}
}

void drawInfoBox(MainWindow& gui,int infoCont,int playerIndex,Map& gameMap,map<string,string>& inventory){
	/*
	Function responsible for drawing the info box to the right of the screen
	-Kacper
	*/
	NPC& player=gameMap.NPCs[get<0>(gameMap.entityData[playerIndex])];
	gui.msgBox(infoCont,40,0," ",4,28,8,0,18,21);
	gui.getWindow(infoCont).drawText(1,1,"Inventory: ");
	gui.getWindow(infoCont).drawText(2,2,"Weapon: "+inventory.find("weapon")->second);
	gui.getWindow(infoCont).drawText(2,3,"Shield: "+inventory.find("shield")->second);
	gui.getWindow(infoCont).drawText(2,4,"Staff: "+inventory.find("staff")->second);
	gui.getWindow(infoCont).drawText(2,5,"Potion: "+inventory.find("potion")->second);
	gui.getWindow(infoCont).drawText(1,7,"Stats: ");
	gui.getWindow(infoCont).drawText(2,8,"Health: "+to_string((int) floor(player.currentHealth))+"/"+to_string((int) floor(player.maxHealth)));
	gui.getWindow(infoCont).drawText(2,9,"Xp: "+to_string((int) floor(player.currentExp))+"/"+to_string((int) floor(player.maxExperience)));
	gui.getWindow(infoCont).drawText(2,10,"Level: "+to_string(player.level));
	gui.getWindow(infoCont).drawText(2,11,"Melee Atck: "+to_string((int) floor(player.attack)));
	gui.getWindow(infoCont).drawText(2,12,"Magic Atck: "+to_string((int) floor(player.magic)));
}

Map loadLevel(string levelName){
	/*
	Function that loads SQL data and converts it into a Map object 
	-Kacper
	*/
	cout << "Loading..." << endl;
	try{
        sqlite::sqlite db( "gameData.sqlite" );
        auto cur = db.get_statement(); 
		cur->set_sql("SELECT * FROM "+levelName+"_mapinfo WHERE attribute='width';");
		cur->prepare();
		cur->step();
		int tempW=cur->get_int(1);
		cur = db.get_statement();
		cur->set_sql("SELECT * FROM "+levelName+"_mapinfo WHERE attribute='height';");
		cur->prepare();
		cur->step();
		int tempH=cur->get_int(1);
		cur = db.get_statement();
		cur->set_sql("SELECT * FROM "+levelName+"_mapdata;");
		cur->prepare();
		vector<int> tempD={};
		while (cur->step()){
			tempD.emplace_back(cur->get_int(1));
		}
		cur= db.get_statement(); 
		cur->set_sql("CREATE TABLE IF NOT EXISTS "+levelName+"_mapEntityData (index_id NUMBER(3) PRIMARY KEY,entity_id NUMBER(3),posX NUMBER(2),posY NUMBER(2));");
		cur->exec();
		cur = db.get_statement();
		cur->set_sql("SELECT * FROM "+levelName+"_mapEntityData;");
		cur->prepare();
		vector<tuple<int,int,int>> tempE={};
		while (cur->step()){
			tempE.emplace_back(make_tuple(cur->get_int(1),cur->get_int(2),cur->get_int(3)));
		}
		Map newMap(tempW,tempH,tempD,tempE);
		cur = db.get_statement();
		cur->set_sql("SELECT * FROM tiles;");
		cur->prepare();
		while (cur->step()){
			newMap.addTile(cur->get_int(4),cur->get_int(2),cur->get_int(3),(cur->get_int(1) ? true : false)); //load all the tiles into the Map object
		}
		cur = db.get_statement();
		cur->set_sql("SELECT * FROM NPC;");
		cur->prepare();
		while (cur->step()){
			NPC tempNpc=createNPC(cur->get_int(0));
			newMap.entities.emplace_back(tempNpc); //has to be a bettr way
			newMap.NPCs.emplace_back(tempNpc); //has to be a bettr way
		}
		cur = db.get_statement();
		cur->set_sql("SELECT * FROM items;");
		cur->prepare();
		while (cur->step()){
			Item tempItem;
			tempItem.generate(cur->get_int(1));
			newMap.entities.emplace_back(tempItem); //has to be a bettr way
			newMap.Items.emplace_back(tempItem);
		}
		return newMap;
	}
	catch (sqlite::exception e){
		cerr << e.what() << endl;
	}
	
}

int checkEntity(int x,int y,Map& newMap){
	/*
	Function that checks 1. if there is an entity in given coordinates using linear search and 2. if there is an entity it returns which entity it is, if there is none it returns -1
	-Kacper
	*/
	for (int i=0;i<newMap.entityData.size();i++){
		if (x==get<1>(newMap.entityData[i]) && y==get<2>(newMap.entityData[i])){
			//exists
			return i;
		}
		else if (i==newMap.entityData.size()-1){
			return -1;
		}
	}
}

void movePlayer(Map& gameMap, int& playerIndex,int dir,MainWindow& gui,map<string,string>& inventory,int& item_count,int& count_combat,vector<string>& chars,vector<string>& colours,vector<string>& textColours){
	/*
	Function responsible for moving the player on the game map
	-Kacper
	*/
	int entId=-1;
	if (dir==0){
		if (get<2>(gameMap.entityData[playerIndex])>0 && gameMap.tiles[gameMap.getTile(get<1>(gameMap.entityData[playerIndex]),get<2>(gameMap.entityData[playerIndex])-1)].passable){
			entId=checkEntity(get<1>(gameMap.entityData[playerIndex]),get<2>(gameMap.entityData[playerIndex])-1,gameMap);
			if (gameMap.getTile(get<1>(gameMap.entityData[playerIndex]),get<2>(gameMap.entityData[playerIndex])-1)==9){gameMap.NPCs[get<0>(gameMap.entityData[playerIndex])].currentHealth-=2;}
			if (entId==-1){
				get<2>(gameMap.entityData[playerIndex])--;
			}
			else {
				interactWithEntity(gui,gameMap,entId,playerIndex,inventory,item_count,count_combat,chars,colours,textColours);
				if (playerIndex>entId){playerIndex--;}
				get<2>(gameMap.entityData[playerIndex])--;
			}
		}
	}
	else if (dir==1){
		if (get<2>(gameMap.entityData[playerIndex])<gameMap.height-1 && gameMap.tiles[gameMap.getTile(get<1>(gameMap.entityData[playerIndex]),get<2>(gameMap.entityData[playerIndex])+1)].passable){
			entId=checkEntity(get<1>(gameMap.entityData[playerIndex]),get<2>(gameMap.entityData[playerIndex])+1,gameMap);
			if (gameMap.getTile(get<1>(gameMap.entityData[playerIndex]),get<2>(gameMap.entityData[playerIndex])+1)==9){gameMap.NPCs[get<0>(gameMap.entityData[playerIndex])].currentHealth-=2;}
			if (entId==-1){
				get<2>(gameMap.entityData[playerIndex])++;
			}
			else {
				interactWithEntity(gui,gameMap,entId,playerIndex,inventory,item_count,count_combat,chars,colours,textColours);
				if (playerIndex>entId){playerIndex--;}
				get<2>(gameMap.entityData[playerIndex])++;
			}
		}
	}
	else if (dir==2){
		if (get<1>(gameMap.entityData[playerIndex])>0 && gameMap.tiles[gameMap.getTile(get<1>(gameMap.entityData[playerIndex])-1,get<2>(gameMap.entityData[playerIndex]))].passable){
			entId=checkEntity(get<1>(gameMap.entityData[playerIndex])-1,get<2>(gameMap.entityData[playerIndex]),gameMap);
			if (gameMap.getTile(get<1>(gameMap.entityData[playerIndex])-1,get<2>(gameMap.entityData[playerIndex]))==9){gameMap.NPCs[get<0>(gameMap.entityData[playerIndex])].currentHealth-=2;}
			if (entId==-1){
				get<1>(gameMap.entityData[playerIndex])--;
			}
			else {
				interactWithEntity(gui,gameMap,entId,playerIndex,inventory,item_count,count_combat,chars,colours,textColours);
				if (playerIndex>entId){playerIndex--;}
				get<1>(gameMap.entityData[playerIndex])--;
			}
		}
	}
	else if (dir==3){
		if (get<1>(gameMap.entityData[playerIndex])<gameMap.width-1 && gameMap.tiles[gameMap.getTile(get<1>(gameMap.entityData[playerIndex])+1,get<2>(gameMap.entityData[playerIndex]))].passable){
			entId=checkEntity(get<1>(gameMap.entityData[playerIndex])+1,get<2>(gameMap.entityData[playerIndex]),gameMap);
			if (gameMap.getTile(get<1>(gameMap.entityData[playerIndex])-1,get<2>(gameMap.entityData[playerIndex]))==9){gameMap.NPCs[get<0>(gameMap.entityData[playerIndex])].currentHealth-=2;}
			if (entId==-1){
				get<1>(gameMap.entityData[playerIndex])++;
			}
			else {
				interactWithEntity(gui,gameMap,entId,playerIndex,inventory,item_count,count_combat,chars,colours,textColours);
				if (playerIndex>entId){playerIndex--;}
				get<1>(gameMap.entityData[playerIndex])++;
			}
		}
	}
}

int makeMapWindow(MainWindow& gui,int id,Map level,int offX,int offY,int width,int height,vector<string> chars,vector<string> colours,vector<string> textColours){
	/*
	Create a window in the gui that displays the map
	-Kacper
	*/
	DisplayWindow mapWindow=mapToWindow(level,chars,colours,textColours).crop(offX,offY,width,height);
	gui.closeWindow(id);
	return gui.addWindow(mapWindow);
}

int levelEditor(MainWindow& gui,vector<string>& chars,vector<string>& colours,vector<string>& textColours,string sqliteFile,int win1){
	/*
	Function responsible for running the level editor in game
	-Kacper
	*/
	string lMapName=cleanString(gui.textPrompt(win1,2,2,4,28,8,0,0,0,"Enter map name")); //the name of the map is cleaned to remove any non alphanumeric characters, binding does not work when trying to do varaible table names
	Map newMap(1,1,{},{}); //create a generic Map so the compiler doesnt cry as soon as the code is ran
	string lWidth,lHeight; 
	try{
		sqlite::sqlite db( sqliteFile );
		auto cur = db.get_statement(); 
		cur->set_sql("select count(type) from sqlite_master where type='table' and name='"+lMapName+"_mapdata';"); //this line returns 1 if the table exists and 0 if it doesnt
		cur->prepare();
		cur->step();
		if (cur->get_int(0)==0){
			//no map
			lWidth=gui.textPrompt(win1,2,2,4,28,8,0,0,0,"Enter map width");
			lHeight=gui.textPrompt(win1,2,2,4,28,8,0,0,0,"Enter map height");
			newMap.~Map(); //delete temp Map object
			new(&newMap) Map(stoi(lWidth,nullptr,10),stoi(lHeight,nullptr,10),{},{}); //re-create the newMap object this time with actual parameters
		}
		else{
			//yes map, load it
			cur= db.get_statement(); 
			cur->set_sql("CREATE TABLE IF NOT EXISTS "+lMapName+"_mapEntityData (index_id NUMBER(3) PRIMARY KEY,entity_id NUMBER(3),posX NUMBER(2),posY NUMBER(2));");
			cur->exec();
			cur = db.get_statement();
			cur->set_sql("SELECT * FROM "+lMapName+"_mapinfo WHERE attribute='width';");
			cur->prepare();
			cur->step();
			int tempW=cur->get_int(1);
			cur = db.get_statement();
			cur->set_sql("SELECT * FROM "+lMapName+"_mapinfo WHERE attribute='height';");
			cur->prepare();
			cur->step();
			int tempH=cur->get_int(1);
			cur = db.get_statement();
			cur->set_sql("SELECT * FROM "+lMapName+"_mapdata;");
			cur->prepare();
			vector<int> tempD={};
			while (cur->step()){
				tempD.emplace_back(cur->get_int(1));
			}
			cur = db.get_statement();
			cur->set_sql("SELECT * FROM "+lMapName+"_mapEntityData;");
			cur->prepare();
			vector<tuple<int,int,int>> tempE={};
			while (cur->step()){
				tempE.emplace_back(make_tuple(cur->get_int(1),cur->get_int(2),cur->get_int(3)));
			}
			newMap.~Map(); //delete temp Map object
			new(&newMap) Map(tempW,tempH,tempD,tempE); //re-create the newMap object this time with actual parameters
		}
		cur = db.get_statement();
		cur->set_sql("SELECT * FROM tiles;");
		cur->prepare();
		while (cur->step()){
			newMap.addTile(cur->get_int(4),cur->get_int(2),cur->get_int(3),(cur->get_int(1) ? true : false)); //load all the tiles into the Map object
		}
		cur = db.get_statement();
		cur->set_sql("SELECT * FROM NPC;");
		cur->prepare();
		while (cur->step()){
			newMap.entities.emplace_back(createNPC(cur->get_int(0))); //has to be a bettr way
		}
		cur = db.get_statement();
		cur->set_sql("SELECT * FROM items;");
		cur->prepare();
		while (cur->step()){
			Item tempItem;
			tempItem.generate(cur->get_int(1));
			newMap.entities.emplace_back(tempItem); //has to be a bettr way
		}
	}
	catch (sqlite::exception e){
		cerr << e.what() << endl;
		return 1;
	}
	gui.closeWindow(win1); //close the user dialog for creating/loading a map
	int pointer=gui.addWindow(1,1,{0},{69},{11},chars,colours,textColours); //create a bunch of UI for the editor
	int selectionWindow=gui.addWindow(1,1,{},{},{},chars,colours,textColours);
	int mapCont=gui.addWindow(1,1,{},{},{},chars,colours,textColours,0,0,0);
	int mapInfo=gui.addWindow(1,1,{},{},{},chars,colours,textColours);
	int pX,pY,key,selectedTile,offX,offY,mapWI,contW,contH,mode;
	mapWI=0;
	offX=0;
	offY=0;
	if (newMap.width>29){ //make sure the container for the Map isnt larger than can fit on the screen
		contW=29;
	}
	else{
		contW=newMap.width;
	}
	if (newMap.height>18){
		contH=18;
	}
	else{
		contH=newMap.height;
	}
	pX=1;
	pY=1;
	mode=0;
	key=0;
	selectedTile=0;
	gui.msgBox(mapInfo,30,6,"Info:",4,28,8,0,0,0,20,10); //draw the ui
	gui.getWindow(mapInfo).drawText(1,2,"Size: "+lWidth+"x"+lHeight);
	gui.getWindow(mapInfo).drawText(1,4,"Controls: ");
	gui.getWindow(mapInfo).drawText(1,5,"Move pointer: arrows");
	gui.getWindow(mapInfo).drawText(1,6,"Scroll map: wasd");
	gui.getWindow(mapInfo).drawText(1,7,"Select tile: q/e");
	gui.getWindow(mapInfo).drawText(1,8,"Place tile: Enter");
	gui.getWindow(mapInfo).drawText(1,9,"Save map: Space");
	gui.getWindow(mapInfo).drawText(1,10,"Quit/delete: z");
	gui.getWindow(mapInfo).drawText(1,11,"Toggle titles/entities: t");
	gui.msgBox(mapCont,0,0," ",4,28,0,0,contW,contH+1);
	gui.getWindow(pointer).moveWindow(pX,pY,99);
	gui.draw();
	while (true){
		gui.getWindow(mapInfo).drawText(1,3,"Map position: "+to_string(pX+offX)+","+to_string(pY+offY)+"   ");
		DisplayWindow mapWindow=mapToWindow(newMap,chars,colours,textColours).crop(offX,offY,contW,contH);
		gui.closeWindow(mapWI);
		mapWI=gui.addWindow(mapWindow);
		gui.getWindow(mapWI).moveWindow(1,1,95);
		drawEntities(gui,newMap.entityData,newMap.entities);
		if (mode==0){
			gui.msgBox(selectionWindow,30,0,"Current Tile:",4,28,8,0,0,0,0,4);
			gui.getWindow(selectionWindow).drawText(1,2,"Current: ");
			gui.getWindow(selectionWindow).placeChar(10,2,newMap.tiles[selectedTile].mapchar);
			gui.getWindow(selectionWindow).placeColour(10,2,newMap.tiles[selectedTile].mapcolour);
			gui.getWindow(selectionWindow).placeTextColour(10,2,newMap.tiles[selectedTile].mapcharcolour);
			gui.getWindow(selectionWindow).drawText(1,3,"Current #: "+to_string(selectedTile));
			gui.getWindow(selectionWindow).drawText(1,4,"Passable: "+to_string(newMap.tiles[selectedTile].passable));
		}
		else if (mode==1){
			gui.msgBox(selectionWindow,30,0,"Current Entity:",4,28,8,0,0,0,0,4);
			gui.getWindow(selectionWindow).drawText(1,2,"Current: ");
			gui.getWindow(selectionWindow).placeChar(10,2,newMap.entities[selectedTile].mapchar);
			gui.getWindow(selectionWindow).placeTextColour(10,2,newMap.entities[selectedTile].mapCharColour);
			gui.getWindow(selectionWindow).drawText(1,3,"Name: "+newMap.entities[selectedTile].name);
		}
		gui.getWindow(pointer).moveWindow(pX,pY,99);
		gui.draw();
		key=waitForKeyPress(); //all the controls
		if (key==65 and pY>1){//up arrow key
		  pY--;
		}
		else if (key==66 && pY<=contH-1){//down arrow key
		  pY++;
		}
		else if (key==68 && pX>1){//left arrow key
		  pX--;
		}
		else if (key==67 && pX<=contW-1){//right arrow key
		  pX++;
		}
		if (key==119 and offY>0){//w key
		  offY--;
		}
		else if (key==115 && offY<newMap.height-contH){//s key
		  offY++;
		}
		else if (key==97 && offX>0){//a key
		  offX--;
		}
		else if (key==100 && offX<newMap.width-contW){//d key
		  offX++;
		}
		else if (key==113 && selectedTile>0){//q key
		  selectedTile--;
		}
		else if (key==101 && ((selectedTile<newMap.tiles.size()-1 && mode==0) || (selectedTile<newMap.entities.size()-1 && mode==1))){ //e key
		  selectedTile++;
		}
		else if (key==116){ //t key
		  if (mode==0){mode=1;}else{mode=0;}
		  selectedTile=0;
		}
		else if (key==10){ //enter key
		  if (mode==0){
		  	newMap.mapdata[(newMap.width*(pY-1+offY))+(pX-1+offX)]=selectedTile;
		  }
		  else {
		  	//check if theres already an entity at that position
			for (int i=0;i<newMap.entityData.size();i++){
				if (pX-1==get<1>(newMap.entityData[i]) && pY-1==get<2>(newMap.entityData[i])){
					//exists
					newMap.entityData.erase(newMap.entityData.begin()+i);
					break;
				}
			}
			if (selectedTile!=0){
				newMap.entityData.emplace_back(make_tuple(selectedTile,pX-1,pY-1));
			}
		  }
		}
		else if (key==122){ //z key
		  gui.oneOffs={};
		  int win1=gui.addWindow(0,0,{},{},{},chars,colours,textColours,0,0,999);
		  int sel=gui.selectList(win1,5,5,66,65,10,4,28,8,0,{"Quit","Delete map","Cancel"},"Exit            ",1,0,3,40,41);
		  if (sel==0){
			gui.closeWindow(mapInfo);
			gui.closeWindow(mapCont);
			gui.closeWindow(pointer);
			gui.closeWindow(mapWI);
			gui.closeWindow(selectionWindow);
			gui.closeWindow(win1);
			return 0;
		  }
		  else if (sel==1){
			  try{
					sqlite::sqlite db( sqliteFile );
					auto cur = db.get_statement(); 
					cur->set_sql("DROP TABLE IF EXISTS "+lMapName+"_mapdata;");
					cur->exec();
					cur= db.get_statement(); 
					cur->set_sql("DROP TABLE IF EXISTS "+lMapName+"_mapinfo;");
					cur->exec();
				    cur= db.get_statement(); 
					cur->set_sql("DROP TABLE IF EXISTS "+lMapName+"_mapEntityData;");
					cur->exec();
					cur= db.get_statement(); 
					cur->set_sql("COMMIT;");
					cur->exec();
				}
				catch (sqlite::exception e){
					cerr << e.what() << endl;
					return 1;
				}
			  	gui.closeWindow(mapInfo);
				gui.closeWindow(mapCont);
				gui.closeWindow(pointer);
				gui.closeWindow(mapWI);
				gui.closeWindow(selectionWindow);
				gui.closeWindow(win1);
				gui.oneOffs={};
				return 0;
			}
		   else if (sel==2){
		   		gui.closeWindow(win1);
		   }
		}
		else if (key==32){ //space key
		  //save de map
			cout << "Saving..." << endl; //just some feedback, when the map is drawn again after saving has finished the cout will be wiped by the gui.draw() function
			newMap.savemap(lMapName);
			newMap.saveEntities(lMapName);
			try{
				sqlite::sqlite db( sqliteFile );
				auto cur = db.get_statement(); 
				cur->set_sql("DROP TABLE IF EXISTS "+lMapName+"_mapinfo;");
				cur->exec();
				cur= db.get_statement(); 
				cur->set_sql("CREATE TABLE "+lMapName+"_mapinfo (attribute VARCHAR2(9) PRIMARY KEY,value NUMBER(3));");
				cur->exec();
				cur= db.get_statement(); 
				cur->set_sql("INSERT INTO "+lMapName+"_mapinfo(attribute,value) VALUES(?,?);");
				cur->prepare();
				cur->bind(1,"width");
				cur->bind(2,newMap.width);
				cur->step();
				cur= db.get_statement(); 
				cur->set_sql("INSERT INTO "+lMapName+"_mapinfo(attribute,value) VALUES(?,?);");
				cur->prepare();
				cur->bind(1,"height");
				cur->bind(2,newMap.height);
				cur->step();
				cur= db.get_statement(); 
				cur->set_sql("COMMIT;");
				cur->exec();
			}
			catch (sqlite::exception e){
				cerr << e.what() << endl;
				return 1;
			}
		}
	}
}