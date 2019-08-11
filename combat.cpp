//Collection of functions in charge of handling combat
//-Mark

#include <iostream>
#include <math.h>

using namespace std;

void gameOver(MainWindow& gui,vector<string>& chars,vector<string>& colours,vector<string>& textColours){
	/*
	Game over screen
	-Kacper
	*/
	gui.oneOffs={};
	int gameOverWin=gui.addWindow(0,0,{},{},{},chars,colours,textColours,0,0,90);
	gui.msgBox(gameOverWin,0,0," ",4,28,8,0,58,21);
	gui.getWindow(gameOverWin).drawText(4,4,"GAME OVER");
	gui.draw();
	waitForKeyPress();
	throw 0;
}

tuple<char,char> chooseattack(MainWindow& gui,int combatGui)
/*
Code to handle combat
-Mark (Couts replaced with gui and item compatibility by Kacper)
*/
{
   
    bool valid=false;
	char choice=' ';
	char attackType=' ';
	gui.getWindow(combatGui).rectangle(1,4,58,10,8,0,true);
	gui.getWindow(combatGui).drawText(1,4,"Choose option from below:");
	gui.getWindow(combatGui).drawText(1,5,"1.Sword Attack 2. Spell Attack 3. Heal");
	gui.draw();
	//attackType=gui.textEntry(2,5)[0];
    while (true){
		attackType=waitForKeyPress();
		if(attackType=='1'||attackType=='2')
		{
			gui.getWindow(combatGui).rectangle(1,4,58,10,8,0,true);
			//cout<<"1.Swing top right "<<endl<<"2. Swing top left"<<endl<<"3. Attack Bottom right"<<endl<<"4. Bottom left"<<endl<<endl;
			gui.getWindow(combatGui).drawText(1,4,"1. Swing top right");
			gui.getWindow(combatGui).drawText(1,5,"2. Swing top left");
			gui.getWindow(combatGui).drawText(1,6,"3. Attack Bottom right");
			gui.getWindow(combatGui).drawText(1,7,"4. Bottom left");
			gui.draw();
			choice=waitForKeyPress();
			gui.getWindow(combatGui).rectangle(1,4,58,10,8,0,true);
			break;
		}
		else if(attackType=='3'){
			break;
		}
	}
	return make_tuple(choice,attackType);
}

bool youhit(char choice)
{
    bool hit;
    int AIchoice = rand() % 6 + 1;
    if(AIchoice==5 && (choice=='1' ||choice=='3'))
    {
        hit=false;
    }
    else if(AIchoice==6 && (choice=='2' ||choice=='4'))
    {
        hit = false;
    }
    else
    {
        hit = true;
        
    }
    return hit;
                
}
bool enemyhit(MainWindow& gui,int combatGui,char choice)
/*
Code to handle combat
-Mark (Couts replaced with gui by Kacper)
*/
{
    char selection;
    bool hit;
    //cout<<"The enemy is about to attack choose a side to block: 1. Block right 2. Block left"<<endl;
	gui.getWindow(combatGui).rectangle(1,4,58,10,8,0,true);
	gui.getWindow(combatGui).drawText(1,4,"The enemy is about to attack choose a side to block:");
	gui.getWindow(combatGui).drawText(1,5,"1. Block right 2. Block left");
	gui.draw();
    selection=waitForKeyPress();
    if(selection=='1' && (choice=='1' ||choice=='3'))
    {
        hit=false;
    }
    else if(selection=='2' && (choice=='2' ||choice=='4'))
    {
        hit = false;
        
    }
    else
    {
        hit = true;
        
    }
    return hit;
}

tuple<int,int> attack(MainWindow& gui,int combatGui,char attackType,char choice,int playerhp,int enemyhp,int extraAttack,int extraMana)
/*
Code to handle combat
-Mark (Couts replaced with gui by Kacper)
*/
{
    int attack=(rand()%4+1)+extraAttack;
    int defense=rand()%2+1;
    int player_damage=0;
    int mana=50+extraMana;
    int manaUsed =0;
  
    int hpAdded=0;
    int damageDealt=0;
   
	gui.getWindow(combatGui).rectangle(1,4,58,10,8,0,true);
	gui.draw();
   
    if(attack>defense)//2 random numbers to see if the attack was succesfully hit
    {
        if(attackType=='1')//if the user chooses to attack with a sword
        {
            
            if(true)//go away stamina
            {
                if(youhit(choice)==false)//runs method to check if attack hit
                {
                    //cout<<"You got blocked by the enemy!"<<endl;
					gui.getWindow(combatGui).drawText(1,4,"You got blocked by the enemy!");
					gui.draw();
					waitForKeyPress();
                }
                else
                {
                    
        
                    switch(choice)
                    {
                        case '1':
                            //import level and expierience
                            //top right formula
                
                            damageDealt= rand()%30+1;
                            
                            
   
                          
                        case '2':
                            //top left formula
                            damageDealt= rand()%30+1;
                        case '3':
                            //bottom right formula
                            damageDealt=rand()%18+1;
                        case '4':
                            //bottom left formula
                            damageDealt=rand()%18+1;
  
                    }
                }
           
                //cout<<"You hit the enemy for "<<damageDealt<<"damage."<<endl ;    
				gui.getWindow(combatGui).rectangle(1,4,58,10,8,0,true);
				gui.getWindow(combatGui).drawText(1,4,"You hit the enemy for "+to_string(damageDealt)+" damage");
				gui.draw();
				waitForKeyPress();
                
                
            }
        }
        else if(attackType=='2')
        {
            if(mana>=6)
            {
                if(youhit(choice)==true)
                {
                    
                    switch(choice)
                    {
                        case '1':
                            //import level and expierience
                            //top right formula
                
                            damageDealt= rand()%48+1;
   
                          
                        case '2':
                            //top left formula
                            damageDealt= rand()%48+1;
                        case '3':
                            //bottom right formula
                            damageDealt=rand()%30+1;
                        case '4':
                            //bottom left formula
                            damageDealt=rand()%30+1;
                    }
                    manaUsed=6;
                    //cout<<"You hit the enemy with a fireball for "<<damageDealt<<"damage."<<endl ;  
					gui.getWindow(combatGui).rectangle(1,4,58,10,8,0,true);
					gui.getWindow(combatGui).drawText(1,4,"You hit the enemy with a fireball for "+to_string(damageDealt)+" damage");
					gui.draw();
					waitForKeyPress();
                }
                else
                {
                        //cout<<"The enemy blocked you with his shield!"<<endl;
						gui.getWindow(combatGui).rectangle(1,4,58,10,8,0,true);
						gui.getWindow(combatGui).drawText(1,4,"The enemy blocked you");
						gui.draw();
						waitForKeyPress();
                }
            }
       
            else
            {
                //cout<<"You didnt have enough potion!"<<endl;
				gui.getWindow(combatGui).rectangle(1,4,58,10,8,0,true);
				gui.getWindow(combatGui).drawText(1,4,"You didnt have enough potion!");
				gui.draw();
				waitForKeyPress();
               
            }
        }
        else
        {
            if(mana>=4)
            {
               
                manaUsed = 4;
                int hpAdded = rand()%18+1;
                if(playerhp==100)
                {
                    //cout<<"You are already full health, attack instead."<<endl;
					gui.getWindow(combatGui).rectangle(1,4,58,10,8,0,true);
					gui.getWindow(combatGui).drawText(1,4,"You didnt have enough potion!");
					gui.draw();
					waitForKeyPress();
                    chooseattack(gui,combatGui);
                   
                    
                }
                else if((hpAdded+playerhp)>100)
                {
                    playerhp=100;
					gui.getWindow(combatGui).rectangle(1,4,58,10,8,0,true);
					gui.getWindow(combatGui).drawText(1,4,"You healed to full health.");
					gui.draw();
					waitForKeyPress();
                }
                else
                {
                    //cout<<"You healed for "<<hpAdded<<endl;
					gui.getWindow(combatGui).rectangle(1,4,58,10,8,0,true);
					gui.getWindow(combatGui).drawText(1,4,"You healed for "+to_string(hpAdded));
					gui.draw();
					waitForKeyPress();
                }
               
            }
            else
            {
                //cout<<"You dont have enough potion to heal yourself"<<endl;
				gui.getWindow(combatGui).rectangle(1,4,58,10,8,0,true);
				gui.getWindow(combatGui).drawText(1,4,"You dont have enough potion to heal yourself");
				gui.draw();
				waitForKeyPress();
                
            }
            
        }
        
    }
    else
    {
        
        if(attackType=='1')
        {
            //cout<<"You missed the enemy! Focus!"<<endl;
			gui.getWindow(combatGui).rectangle(1,4,58,10,8,0,true);
			gui.getWindow(combatGui).drawText(1,4,"You missed the enemy! Focus!");
			gui.draw();
			waitForKeyPress();
        }
        else if(attackType=='2')
        {
            if(mana>6)
            {
                //cout<<"You missed the enemy!"<<endl;
				gui.getWindow(combatGui).rectangle(1,4,58,10,8,0,true);
				gui.getWindow(combatGui).drawText(1,4,"You missed the enemy!");
				gui.draw();
				waitForKeyPress();
            }
            else
            {
                //cout<<"The spell fizzled, not enough mana.";
				gui.getWindow(combatGui).rectangle(1,4,58,10,8,0,true);
				gui.getWindow(combatGui).drawText(1,4,"The spell fizzled, not enough mana.");
				gui.draw();
				waitForKeyPress();
            }
        }
        else
        {
            if(mana>4)
            {
                //cout<<"The spell fizzled."<<endl;
				gui.getWindow(combatGui).rectangle(1,4,58,10,8,0,true);
				gui.getWindow(combatGui).drawText(1,4,"The spell fizzled.");
				gui.draw();
				waitForKeyPress();
            }
            else
            {
                //cout<<"You do not have enough mana, collect more out of combat."<<endl;
				gui.getWindow(combatGui).rectangle(1,4,58,10,8,0,true);
				gui.getWindow(combatGui).drawText(1,4,"You do not have enough mana, collect more out of combat.");
				gui.draw();
				waitForKeyPress();
            }
            
        }
    }
    //cout<<"Damage dealt"<<damageDealt<<endl;
	gui.getWindow(combatGui).rectangle(1,4,58,10,8,0,true);
	gui.getWindow(combatGui).drawText(1,4,"Damage dealt: "+to_string(damageDealt));
    enemyhp=enemyhp-damageDealt;
    //cout<<"Enemy HP is: "<<enemyhp<<endl;
    mana-=manaUsed;
    playerhp+=hpAdded;
	gui.draw();
	return make_tuple(playerhp,enemyhp);
}
tuple<int,int> enemyattack(MainWindow& gui,int combatGui,char choice,int playerhp,int enemyhp)
/*
Code to handle combat
-Mark (Couts replaced with gui by Kacper)
*/
{
    int attack=rand()%6+1;
    int defense=(rand()%6+1);
    int mana=40;
    int manaUsed =0;
    int hpAdded=0;
    
    int attackChosen =rand()%3+1;
    int damageDealt=0;
    if(attack>defense)//2 random numbers to see if the attack was succesfully hit
    {   
        if(attackChosen==1)
        {
            if(true)
            {
                if(enemyhit(gui,combatGui,choice)==false)
                {
					gui.getWindow(combatGui).rectangle(1,4,58,10,8,0,true);
					gui.getWindow(combatGui).drawText(1,4,"You blocked the enemy.");
					gui.draw();
                    //cout<<"You blocked the enemy!"<<endl;
                }
                else
                {
        
                    switch(choice)
                    {
                        case '1':
                            //import level and expierience
                            //top right formula
                
                            damageDealt= rand()%24+1;
   
                          
                        case '2':
                            //top left formula
                            damageDealt= rand()%24+1;
                        case '3':
                            //bottom right formula
                            damageDealt=rand()%12+1;
                        case '4':
                            //bottom left formula
                            damageDealt=rand()%12+1;

                    }
                    //cout<<"The enemy hit you for "<<damageDealt<<"."<<endl;           
                    gui.getWindow(combatGui).rectangle(1,4,58,10,8,0,true);
					gui.getWindow(combatGui).drawText(1,4,"The enemy hit you for "+to_string(damageDealt));
					gui.draw();
                }
                
            }
        }
        else if(attackChosen==2)
        {
            if(mana>=6)
            {
                if(enemyhit(gui,combatGui,choice)==true)
                {
                    
                    damageDealt = rand()%10+1;
                    manaUsed=6;
                    switch(choice)
                    {
                        case '1':
                            //import level and expierience
                            //top right formula
                
                            damageDealt= rand()%42+1;
                            //cout<<"Watch your head!"<<endl;
							gui.getWindow(combatGui).rectangle(1,4,58,10,8,0,true);
							gui.getWindow(combatGui).drawText(1,4,"Watch your head!");
							gui.draw();
   
                          
                        case '2':
                            //top left formula
                            damageDealt= rand()%42+1;
                        case '3':
                            //bottom right formula
                            damageDealt=rand()%24+1;
                            //cout<<"You got hit in the right leg mate"<<endl;
							gui.getWindow(combatGui).rectangle(1,4,58,10,8,0,true);
							gui.getWindow(combatGui).drawText(1,4,"You got hit in the right leg mate");
							gui.draw();
                        case '4':
                            //bottom left formula
                            damageDealt=rand()%24+1;
                   }
                    
                   //cout<<"The enemy hit you with his fireball for "<<damageDealt<<"."<<endl;
				   gui.getWindow(combatGui).rectangle(1,4,58,10,8,0,true);
				   gui.getWindow(combatGui).drawText(1,4,"The enemy hit you with his fireball for "+to_string(damageDealt));
				   gui.draw();
                }
                else
                {
                    //cout<<"You blocked the enemy, good job!"<<endl;
					gui.getWindow(combatGui).rectangle(1,4,58,10,8,0,true);
				    gui.getWindow(combatGui).drawText(1,4,"You blocked the enemy, good job!");
				    gui.draw();

                }
            }
            else
            {
                //cout<<"Enemy didnt have enough potion!"<<endl;
				gui.getWindow(combatGui).rectangle(1,4,58,10,8,0,true);
				gui.getWindow(combatGui).drawText(1,4,"Enemy didnt have enough potion!");
				gui.draw();
               
            }
        
        }
        else
        {
            if(false) //potions broken
            {
                manaUsed = 4;
                hpAdded = rand()%18+1;
                if(enemyhp+hpAdded>=100)
                {
                    enemyhp=100;
                    //cout<<"The enemy healed back to full health.";
					gui.getWindow(combatGui).rectangle(1,4,58,10,8,0,true);
					gui.getWindow(combatGui).drawText(1,4,"The enemy healed back to full health.");
					gui.draw();
                }
                else
                {
                     //cout<<"The enemy healed for "<<hpAdded<<endl<<endl;
					 enemyhp+=hpAdded;
					 gui.getWindow(combatGui).rectangle(1,4,58,10,8,0,true);
					 gui.getWindow(combatGui).drawText(1,4,"The enemy healed for "+to_string(hpAdded));
					 gui.draw();
                }
               
            }
            else
            {
                //cout<<"Enemy didnt have enough potion"<<endl;
				gui.getWindow(combatGui).rectangle(1,4,58,10,8,0,true);
				gui.getWindow(combatGui).drawText(1,4,"Enemy didnt have enough potion");
				gui.draw();
                
            }
        }
    }
    else
    {
         //cout<<"The enemy missed zero damage was caused"<<endl;
		 gui.getWindow(combatGui).rectangle(1,4,58,10,8,0,true);
		 gui.getWindow(combatGui).drawText(1,4,"The enemy missed zero damage was caused");
		 gui.draw();
    }
    playerhp-=damageDealt;
    mana-=manaUsed;
    enemyhp+=hpAdded;
	return make_tuple(playerhp,enemyhp);
}
/*1=top right
2=top left
3=bottom right
4=bottom left
5=block right
6=block left*/


int combatMain(Map& gameMap,MainWindow& gui,int entId,int playerIndex,int& count_combat,vector<string>& chars,vector<string>& colours,vector<string>& textColours){
	/*
	Code to handle combat
	-Mark (Couts replaced with gui by Kacper)
	*/
	int combatGui=gui.addWindow(0,0,{},{},{},chars,colours,textColours,0,0,50);
	gui.msgBox(combatGui,0,0,"COMBAT",4,28,8,0,58,21);
	gui.getWindow(combatGui).drawText(1,2,"Player HP: "+to_string((int) floor(gameMap.NPCs[get<0>(gameMap.entityData[playerIndex])].currentHealth))+"/"+to_string((int) floor(gameMap.NPCs[get<0>(gameMap.entityData[playerIndex])].maxHealth))+" | Enemy: "+gameMap.NPCs[get<0>(gameMap.entityData[entId])].name+" HP: "+to_string((int) floor(gameMap.NPCs[get<0>(gameMap.entityData[entId])].maxHealth))+"/"+to_string((int) floor(gameMap.NPCs[get<0>(gameMap.entityData[entId])].maxHealth))+"       ");
	gui.draw();
	int playerhp=gameMap.NPCs[get<0>(gameMap.entityData[playerIndex])].currentHealth;
	int enemyhp=gameMap.NPCs[get<0>(gameMap.entityData[entId])].maxHealth;
    int count=1;
	char choice=' ';
	char attackType=' ';
    do
    {
        //cout<<endl<<"Turn Number: "<<count<<endl;
		gui.getWindow(combatGui).drawText(1,3,"Turn Number: "+to_string(count));
		gui.draw();
        tuple<char,char> temp=chooseattack(gui,combatGui);
		choice=get<0>(temp);
		attackType=get<1>(temp);
        temp=attack(gui,combatGui,attackType,choice,playerhp,enemyhp,gameMap.NPCs[get<0>(gameMap.entityData[playerIndex])].attack,gameMap.NPCs[get<0>(gameMap.entityData[playerIndex])].magic);
		playerhp=get<0>(temp);
		enemyhp=get<1>(temp);
		gui.getWindow(combatGui).drawText(1,2,"Player HP: "+to_string((int) floor(playerhp))+"/"+to_string((int) floor(gameMap.NPCs[get<0>(gameMap.entityData[playerIndex])].maxHealth))+" | Enemy: "+gameMap.NPCs[get<0>(gameMap.entityData[entId])].name+" HP: "+to_string((int) floor(enemyhp))+"/"+to_string((int) floor(gameMap.NPCs[get<0>(gameMap.entityData[entId])].maxHealth))+"       ");
		if (enemyhp>0){
			temp=enemyattack(gui,combatGui,choice,playerhp,enemyhp);
			playerhp=get<0>(temp);
			enemyhp=get<1>(temp);
		}
		gui.getWindow(combatGui).drawText(1,2,"Player HP: "+to_string((int) floor(playerhp))+"/"+to_string((int) floor(gameMap.NPCs[get<0>(gameMap.entityData[playerIndex])].maxHealth))+" | Enemy: "+gameMap.NPCs[get<0>(gameMap.entityData[entId])].name+" HP: "+to_string((int) floor(enemyhp))+"/"+to_string((int) floor(gameMap.NPCs[get<0>(gameMap.entityData[entId])].maxHealth))+"       ");
        count+=1;
		gui.draw();
		waitForKeyPress();
    }
    while(playerhp>=0&&enemyhp>=0);
	if (enemyhp<0){
		gameMap.NPCs[get<0>(gameMap.entityData[playerIndex])].get_exp();
		gameMap.NPCs[get<0>(gameMap.entityData[playerIndex])].currentHealth=playerhp;
		count_combat++;
		gameMap.entityData.erase(gameMap.entityData.begin()+entId); //so delet the monster
		gui.closeWindow(combatGui);
	}
	else {
		gameOver(gui,chars,colours,textColours);
	}
    //cout<<"YOU WON!!!!!!!!!!!!!!!!!!!!!";  
}



/*will have 6 options user will be promted to enter 6 moves.
# AI will also randomely make 6 moves
# some sort of formula ot calculate damage to take away
#data updated in database
#data saved to database
#damage taken away dependant on weapon, level, expierience*/
