/*
SQL NOTE:
The use of string manipulation in sql statements is not by choice but rather a workaround to a bug with the library itself that David Croft couldnt solve himself and told me to
continue using the workaround of string manipulation (which i pass through a cleaning function that removes characters that could cause an injection anyway)
https://github.com/glycos/libsqlitepp/pull/1
*/
#include <iostream>
#include <fstream>
#include <vector>
#include "GUI.cpp"
#include "entity.cpp"
#include "map.cpp"
#include "combat.cpp"
#include "game.cpp"
using namespace std;

#include "libsqlite.hpp"

int main(){
	vector<string> chars; //vector of characters to be used by the window objects
	vector<string> colours; //vector of colours to be used by window objects
	vector<string> textColours; //vector of text colours to be used by the objects
 	string sqliteFile = "gameData.sqlite";
	try{ //Load in graphical details from the database
        sqlite::sqlite db( sqliteFile );
        auto cur = db.get_statement(); 
		cur->set_sql("SELECT * FROM chars;");
		cur->prepare();
		while (cur->step()){
			chars.emplace_back(cur->get_text(1));
		}
		cur= db.get_statement(); 
		cur->set_sql("SELECT * FROM colours;");
		cur->prepare();
		while (cur->step()){
			colours.emplace_back(cur->get_text(1));
		}
		cur= db.get_statement(); 
		cur->set_sql("SELECT * FROM textColours;");
		cur->prepare();
		while (cur->step()){
			textColours.emplace_back(cur->get_text(1));
		}
	}
	catch (sqlite::exception e){
		cerr << e.what() << endl;
		return 1;
	}
	MainWindow gui (60,22,{},{},{},chars,colours,textColours); //create the main GUI object responsible for handling all drawing operations
	while (true){
		int win1=gui.addWindow(5,5,{1,1,1,1,1},{},{},chars,colours,textColours); //create the main User Interface
		int select=gui.selectList(win1,3,3,66,65,10,4,28,8,0,{"New Game","Load Game","Level Editor","Custom Game","Leaderboard","Credits","Quit"},"Main Menu",1,0,3,40,41);
		gui.draw();
		if (select==0 || select==1){ //if a game has been started
			gui.closeWindow(win1); //close the now not needed main menu
			vector<string> levels={"tutorialone","tutorialtwo","tutorialthree","tutorialfour","tutorialfive","levelone","leveltwo","levelthree","levelfour","levelfive","final"}; //list of levels in order of progression
			vector<tuple<int,int>> levelEndCoords={make_tuple(7,0),make_tuple(7,0),make_tuple(19,3),make_tuple(20,0),make_tuple(7,0),make_tuple(11,0),make_tuple(14,4),make_tuple(14,9),make_tuple(19,5),make_tuple(22,4)};
			vector<string> levelEntryMsg={
                                                            "Your throne awaits you my lord"
                                                            " Enemies will be merciless and the lava will continue to pour"
                                                            "Lava is dangerous,it will always take you 2HP"
            
                ,"Battles with enemies are unpredictable,you will get a better chance to strike more damage depending on your level, that is going to build as you gain experience from battling"
           
                ," Because lava always takes you the same amount health, you can choose to avoid a battle with an enemy where you can survive without any health loss or die miserably"

                ," Enemies will get a lot stronger, which means that when defeated you will gain more experience from them than lower level ones"
                "You can also choose to avoid battles, but experience can help you a lot in battles with stronger enemies that you'll find along the way, so you're call."
                                                                
                            "Items will be spread across the maps, they will help you to win battles due to the bonuses that they give",
                                         
                                                                    "That's all my lord"
                                                                "Now, go fulfill your destiny"
                                         
                                         };
			int mapCont=gui.addWindow(40,20,{},{},{},chars,colours,textColours); //the map container window is created (the blue border that shows the map)
			int infoCont=gui.addWindow(20,20,{},{},{},chars,colours,textColours); //the info container window is created (to the right, shows stats etc)
			map<string,string> inventory={}; //temp
			inventory["weapon"]="None"; //temp
			inventory["shield"]="None"; //temp
			inventory["staff"]="None"; //temp
			inventory["potion"]="None"; //temp
			gui.msgBox(mapCont,0,0," ",4,28,0,0,40,21); //create the blue outline by making a message box 40x21 large and leaving the message blank
			int currentLevel;
			if (select==0){ //create new game
				currentLevel=0;
			}
			else if (select==1){ //load existing game
				//load the game
			}
			//main game goes here
			Map gameMap=loadLevel(levels[currentLevel]); //load the first level
			int mapWin=makeMapWindow(gui,0,gameMap,0,0,gameMap.width,gameMap.height,chars,colours,textColours); //create the window that holds the map
			int playerIndex=0; //varaible that keeps track of the players index within the entityData of the Map
			int exist=0;
			int item_count=0;
			int count_combat=0;
			int playerXp,playerMaxXp,playerCurrentHealth,playerMaxHealth,playerLevel,playerMeAttack,playerMaAttack;
			for (int i=0;i<gameMap.entityData.size();i++){ //linear search for the player entity
				if (gameMap.entities[get<0>(gameMap.entityData[i])].id==1){ //check the id of every entity, if its 1, the player, then set that as the index
					playerIndex=i;
					break;
				}
			}
			int lvlMsg=gui.addWindow(0,0,{},{},{},chars,colours,textColours,0,0,90);
			gui.msgBox(lvlMsg,0,0,"The end is here my lord.     Every creature that has ever lived is doomed.     Your death is certain.    But you will die as you were meant to die.    As a King.",4,28,8,0,58,21);
			gui.draw();
			waitForKeyPress();
			gui.closeWindow(lvlMsg);
			lvlMsg=gui.addWindow(0,0,{},{},{},chars,colours,textColours,0,0,90);
			gui.msgBox(lvlMsg,0,0,levelEntryMsg[currentLevel],4,28,8,0,58,21);
			gui.draw();
			waitForKeyPress();
			gui.closeWindow(lvlMsg);
			gui.getWindow(mapWin).moveWindow(1,1,5);//make sure the map is 1. not on the blue border 2. drawn on top of the empty message box by setting its z-value to 5
			while (true){ //main loop of the game
				try {
					if (gameMap.NPCs[get<0>(gameMap.entityData[playerIndex])].currentExp>=gameMap.NPCs[get<0>(gameMap.entityData[playerIndex])].maxExperience){
						gameMap.NPCs[get<0>(gameMap.entityData[playerIndex])].level_up();
						int id=pick_id(gameMap.NPCs[get<0>(gameMap.entityData[playerIndex])].level,exist);
						tuple<int,int> coords=pick_tile(levels[currentLevel]);
						int entitId=0;
						for (int i=0;i<gameMap.entities.size();i++){
							if (gameMap.entities[i].id==id){
								entitId=i;
								break;
							}
						}
						gameMap.entityData.emplace_back(make_tuple(entitId,get<0>(coords),get<1>(coords)));
					}
					drawInfoBox(gui,infoCont,playerIndex,gameMap,inventory); //draw the various elements of the game
					drawEntities(gui,gameMap.entityData,gameMap.entities);
					gui.draw();
					int key=waitForKeyPress();//wait for user input
					if (gameMap.NPCs[get<0>(gameMap.entityData[playerIndex])].currentHealth<=0){gameOver(gui,chars,colours,textColours);}
					if (key==65){ //controls
						movePlayer(gameMap,playerIndex,0,gui,inventory,item_count,count_combat,chars,colours,textColours);
					}
					else if (key==66){
						movePlayer(gameMap,playerIndex,1,gui,inventory,item_count,count_combat,chars,colours,textColours);
					}
					else if (key==68){
						movePlayer(gameMap,playerIndex,2,gui,inventory,item_count,count_combat,chars,colours,textColours);
					}
					else if (key==67){
						movePlayer(gameMap,playerIndex,3,gui,inventory,item_count,count_combat,chars,colours,textColours);
					}
					if (get<1>(gameMap.entityData[playerIndex])==get<0>(levelEndCoords[currentLevel]) && get<2>(gameMap.entityData[playerIndex])==get<1>(levelEndCoords[currentLevel])){//temp q key
						if (currentLevel<levels.size()-2){
							currentLevel++;
							gui.oneOffs={};
							if (currentLevel<levelEntryMsg.size() && levelEntryMsg[currentLevel]!=""){
								lvlMsg=gui.addWindow(0,0,{},{},{},chars,colours,textColours,0,0,90);
								gui.msgBox(lvlMsg,0,0,levelEntryMsg[currentLevel],4,28,8,0,58,21);
								gui.draw();
								waitForKeyPress();
								gui.closeWindow(lvlMsg);
							}
							NPC& player=gameMap.NPCs[get<0>(gameMap.entityData[playerIndex])];
							playerXp=player.currentExp;playerMaxXp=player.maxExperience;playerCurrentHealth=player.currentHealth;playerMaxHealth=player.maxHealth;playerLevel=player.level;playerMeAttack=player.attack;playerMaAttack=player.magic;
							gameMap=loadLevel(levels[currentLevel]);
							mapWin=makeMapWindow(gui,mapWin,gameMap,0,0,gameMap.width,gameMap.height,chars,colours,textColours);
							for (int i=0;i<gameMap.entityData.size();i++){
								if (gameMap.entities[get<0>(gameMap.entityData[i])].id==1){ //check the id of every entity, if its 1, the player, then set that as the index
									playerIndex=i;
									break;
								}
							}
							gameMap.NPCs[get<0>(gameMap.entityData[playerIndex])].currentExp=playerXp;gameMap.NPCs[get<0>(gameMap.entityData[playerIndex])].maxExperience=playerMaxXp;gameMap.NPCs[get<0>(gameMap.entityData[playerIndex])].currentHealth=playerCurrentHealth;gameMap.NPCs[get<0>(gameMap.entityData[playerIndex])].maxHealth=playerMaxHealth;gameMap.NPCs[get<0>(gameMap.entityData[playerIndex])].level=playerLevel;gameMap.NPCs[get<0>(gameMap.entityData[playerIndex])].attack=playerMeAttack;gameMap.NPCs[get<0>(gameMap.entityData[playerIndex])].magic=playerMaAttack;
							gui.getWindow(mapWin).moveWindow(1,1,5);
						}
						else{
							gui.oneOffs={};
							Map endMap=loadLevel("final");
							DisplayWindow endMapWin=mapToWindow(endMap,chars,colours,textColours);
							int endMapWinm=gui.addWindow(endMapWin);
							gui.getWindow(endMapWinm).moveWindow(1,1,500);
							int endWin=gui.addWindow(0,0,{},{},{},chars,colours,textColours,0,0,1500);
							gui.msgBox(endWin,4,4,"You win",4,28,8,0);
							gui.draw();
							waitForKeyPress();
							gui.closeWindow(endMapWinm);
							gui.closeWindow(endWin);
							throw 0;
						}
					}
				} catch(int e){
					int endG=gui.addWindow(0,0,{},{},{},chars,colours,textColours,0,0,1000);
					string pname=gui.textPrompt(endG,2,2,4,28,8,0,0,0,"Please enter your name");
					gui.draw();
					updateHistory(pname,item_count,count_combat);
					showLeaderboard(gui,endG,chars,colours,textColours);
					gui.closeWindow(endG);
					return e;
				}
			}
		}

		if (select==2){ //level editor
			levelEditor(gui,chars,colours,textColours,sqliteFile,win1);
		}
		if (select==3){ //custom level
			string lMapName=cleanString(gui.textPrompt(win1,2,2,4,28,8,0,0,0,"Enter map name"));
			Map gameMap=loadLevel(lMapName); //load the first level
			int mapWin=makeMapWindow(gui,0,gameMap,0,0,gameMap.width,gameMap.height,chars,colours,textColours); //create the window that holds the map
			int playerIndex=0; //varaible that keeps track of the players index within the entityData of the Map
			int exist=0;
			int item_count=0;
			int count_combat=0;
			int playerXp,playerMaxXp,playerCurrentHealth,playerMaxHealth,playerLevel,playerMeAttack,playerMaAttack;
			for (int i=0;i<gameMap.entityData.size();i++){ //linear search for the player entity
				if (gameMap.entities[get<0>(gameMap.entityData[i])].id==1){ //check the id of every entity, if its 1, the player, then set that as the index
					playerIndex=i;
					break;
				}
			}
			gui.closeWindow(win1); //close the now not needed main menu
			int mapCont=gui.addWindow(40,20,{},{},{},chars,colours,textColours); //the map container window is created (the blue border that shows the map)
			int infoCont=gui.addWindow(20,20,{},{},{},chars,colours,textColours); //the info container window is created (to the right, shows stats etc)
			map<string,string> inventory={}; //temp
			inventory["weapon"]="None"; //temp
			inventory["shield"]="None"; //temp
			inventory["staff"]="None"; //temp
			inventory["potion"]="None"; //temp
			gui.msgBox(mapCont,0,0," ",4,28,0,0,40,21); //create the blue outline by making a message box 40x21 large and leaving the message blank
			gui.getWindow(mapWin).moveWindow(1,1,5);//make sure the map is 1. not on the blue border 2. drawn on top of the empty message box by setting its z-value to 5
			while (true){ //main loop of the game
				try {
					if (gameMap.NPCs[get<0>(gameMap.entityData[playerIndex])].currentExp>=gameMap.NPCs[get<0>(gameMap.entityData[playerIndex])].maxExperience){
						gameMap.NPCs[get<0>(gameMap.entityData[playerIndex])].level_up();
						int id=pick_id(gameMap.NPCs[get<0>(gameMap.entityData[playerIndex])].level,exist);
						tuple<int,int> coords=pick_tile(lMapName);
						int entitId=0;
						for (int i=0;i<gameMap.entities.size();i++){
							if (gameMap.entities[i].id==id){
								entitId=i;
								break;
							}
						}
						gameMap.entityData.emplace_back(make_tuple(entitId,get<0>(coords),get<1>(coords)));
					}
					drawInfoBox(gui,infoCont,playerIndex,gameMap,inventory); //draw the various elements of the game
					drawEntities(gui,gameMap.entityData,gameMap.entities);
					gui.draw();
					int key=waitForKeyPress();//wait for user input
					if (gameMap.NPCs[get<0>(gameMap.entityData[playerIndex])].currentHealth<=0){gameOver(gui,chars,colours,textColours);}
					if (key==65){ //controls
						movePlayer(gameMap,playerIndex,0,gui,inventory,item_count,count_combat,chars,colours,textColours);
					}
					else if (key==66){
						movePlayer(gameMap,playerIndex,1,gui,inventory,item_count,count_combat,chars,colours,textColours);
					}
					else if (key==68){
						movePlayer(gameMap,playerIndex,2,gui,inventory,item_count,count_combat,chars,colours,textColours);
					}
					else if (key==67){
						movePlayer(gameMap,playerIndex,3,gui,inventory,item_count,count_combat,chars,colours,textColours);
					}
				} catch(int e){
					int endG=gui.addWindow(0,0,{},{},{},chars,colours,textColours,0,0,1000);
					string pname=gui.textPrompt(endG,2,2,4,28,8,0,0,0,"Please enter your name");
					gui.draw();
					updateHistory(pname,item_count,count_combat);
					showLeaderboard(gui,endG,chars,colours,textColours);
					gui.closeWindow(endG);
					return e;
				}
			}
		}
		if (select==4){ //leaderboard
			showLeaderboard(gui,win1,chars,colours,textColours);
		}
		if (select==5){ //credits
			gui.msgBox(win1,5,4,"Credits:",4,28,8,0,0,0,30,9);
			gui.getWindow(win1).drawText(1,3,"Main Game and GUI: Kacper Sowka");
			gui.getWindow(win1).drawText(1,4,"Map and tile code: Francisco Ramos");
			gui.getWindow(win1).drawText(1,5,"Items and level progress: Robert Savin");
			gui.getWindow(win1).drawText(1,6,"Entities: Silvia Gajdosova");
			gui.getWindow(win1).drawText(1,7,"Combat and saving: Mark Ashfield");
			gui.draw();
			waitForKeyPress();
			gui.closeWindow(win1);
		}
		if (select==6){
			//just exit the program
			return 0;
		}
	}
}