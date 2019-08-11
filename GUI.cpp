//GUI.cpp, utility for creating and manipulating simple text based GUI in the terminal
//-Kacper

#include <iostream>
#include <vector>
#include <tuple>
#include <map>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <stdio.h>
using namespace std;

int waitForKeyPress(){ // this function causes the program to wait for an input and returns an int representing the key pressed, adapted and modified from code taken from https://stackoverflow.com/questions/31925911/getch-not-working-without-initscr
  struct termios oldt, newt; //using termios, the terminal is put into canonical mode which means it wont buffer cin input
  char ch, command[20];
  int oldf;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);

  while(true)
  {
    ch = getchar(); //get the character from cin
    if ((int)ch==27){ //arrow keys span multiple characters, cycle through them
      getchar();
      ch=getchar();
      tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
      return ch;
    }
    else if (ch!=-1){ //every other character can be returned on its own
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return ch;
    }
  }
}

class DisplayWindow { //define Display Window Class
	public:
		int width, height; //define all variables.int width and height of the display, int vector maps of colour and character data, string vector maps of different character and colours
		vector<int> colourMap, charMap, charColourMap;
		vector<string> chars, colours, textColours;
		tuple <int,int,int> position;
		DisplayWindow (int a,int b,vector<int> c,vector<int> d,vector<int> e,const vector<string> &f,const vector<string> &g,const vector<string> &h,int xPos=0,int yPos=0,int zPos=0){ //defining the init function (constructor) for the class
			width=a; //set each variable to its parameter
			height=b;
			colourMap = c;
			charMap = d;
      charColourMap = e;
			chars = f;
			colours = g;
     	textColours = h;
			position=make_tuple(xPos,yPos,zPos);
			for (int i=colourMap.size();i<width*height;i++){ //fill in any missing parts in the colour map with 0
				colourMap.emplace_back(0);
			};
			for (int i=charMap.size();i<width*height;i++){ //fill in any missing parts in the character map with 0
				charMap.emplace_back(0); 
			};
      		for (int i=charColourMap.size();i<width*height;i++){ //fill in any missing parts in the character colour map with 7
				charColourMap.emplace_back(7); 
			};
		}
	
		void placeColour (int x,int y,int col){ //overwrite the colour at position (x,y) on the map
			colourMap[y*width +x]=col;
		};
  
    void placeTextColour (int x,int y,int col){ //overwrite the colour at position (x,y) on the map
			charColourMap[y*width +x]=col;
		};
	
		void placeChar (int x,int y,int chr){ //overwrite the character at position (x,y) on the map
			charMap[y*width +x]=chr;
		};
	
		string getChar (int x, int y){ //return the character at positon (x,y) on the map
			return chars[charMap[y*width +x]];
		}
	
		int getColour (int x, int y){ //return the colour code at position (x,y) on the map
			return colourMap[y*width +x];
		}
  
    	string getTextColour (int x, int y){ //return the colour code at position (x,y) on the map
			return textColours[charColourMap[y*width +x]];
		}
	
		void rectangle (int x, int y, int rWidth, int rHeight, int col, int chr, bool fill=false, int txtCol=7){ 
		/*
		draw a rectangle at position (x,y) with dimensions rWidthxrHeight with the colour and character of the rectangle being decided by col and chr 
		fill decides if the rectangle will be filled or just an outline
		*/
			if (x+rWidth>width){ //check if the rectangle is too big to fit on the screen (x-axis wise)
				rWidth=width-x; //if it is, make the rectangle smaller so it just fits the screen
			}
			if (y+rHeight>height){ //same as above but for the y-axis
				rHeight=height-y;
			}
			for (int j=y;j<y+rHeight;j++){ //iterate through each column
				for (int i=x;i<x+rWidth;i++){ //iterate through each row within the column
					if ((fill==false && (i==x || i==x+rWidth-1) || (j==y || j==y+rHeight-1)) || fill){ //if fill==true then the rectangle will always overwrite any position within its area, if fill==false then it will only overwrite the position if its at the very edges of the rectangle
						colourMap[j*width+i]=col; //overwrite the colour at position (x+i,y+j) with the colour id specified in the col argument
						charMap[j*width+i]=chr; //overwrite the character at position (x+i,y+j) with the character id specified in the chr argument
            charColourMap[j*width+i]=txtCol; //overwrite the colour at position (x+i,y+j) with the colour id specified in the col argument
          };
				};
			};
		};
		
		void drawOnto (int x, int y,const DisplayWindow& win){
		/*
		draw the window object win onto the object the method is being called on at position (x,y)
		*/
			int rWidth=win.width; //set rWidth to the win objects width
			int rHeight=win.height; //set rHeight to the win objects height
			if (x+rWidth>width){ //check if the object would overflow off the screen because of its position and size
				rWidth=width-x; //crop the object so it fits on the window its being drawn on
			}
			if (y+rHeight>height){ //ditto for height
				rHeight=height-y;
			}
			for (int j=0;j<rHeight;j++){ //iterate through each row of win (y axis)
				for (int i=0;i<rWidth;i++){ //iterate through each column of each row of win(x axis)
					      colourMap[(j+y)*width+(i+x)]=win.colourMap[(j*rWidth)+i]; //overwrite the colour at position with the colour at the same position in win
					      charMap[(j+y)*width+(i+x)]=win.charMap[(j*rWidth)+i]; //overwrite the character at position with the character at the same position in win
          			charColourMap[(j+y)*width+(i+x)]=win.charColourMap[(j*rWidth)+i];
        		};
			};
		};
		
		void drawInto (int x, int y, DisplayWindow& win){
		/*
		draw window object the method is called on into the argument window object win at position (x,y)
		*/
			int rWidth=get<0>(win.getSize()); //for some reason tuples in c++ dont support [] indexing so you just put get<n> before to get the nth item
			int rHeight=get<1>(win.getSize());
			int sWidth=width; //self width
			int sHeight=height; //self height
			if (x+sWidth>rWidth){ //once again, check if it will overflow through the window, if it does just crop it
				sWidth=rWidth-x;
			}
			if (y+sHeight>rHeight){//same for y axis
				sHeight=rHeight-y;
			}
			for (int j=0;j<sHeight;j++){ //iterate through each row...
				for (int i=0;i<sWidth;i++){ //iterate through each column in each row...
					win.placeColour((i+x),(j+y),colourMap[(j*sWidth)+i]); //overwrite the colour in every position within the win dimensions
					win.placeChar((i+x),(j+y),charMap[(j*sWidth)+i]); //overwrite the char in every position within the win dimensions
          win.placeTextColour((i+x),(j+y),charColourMap[(j*sWidth)+i]);
        };
			};
		};
	
		tuple<int,int> getSize(){ //return tuple (width,height) of the object
			return make_tuple(width,height); //self explanatory imo
		};
	
		int drawText(int x, int y, string text ,int widthLimit=0,int heightLimit=0,int textCol=7){
		/*
		Draws text (text) onto the screen, at position (x,y) with widthLimit specifying when the text should go onto a new line (widthLimit 0 means the text will go on until it hits the edge of the screen), returns the number of rows taken by the text
		*/
			int rows; //you have to declare variables within the main function and not within if statements etc
			if (widthLimit<1){ //self explanatory
				rows=1;
				widthLimit=text.size();
			}
			else{
				rows=(text.size()/widthLimit)+1; //dividing ints is like floor division, the number of rows is the length of the text vs how many times it will hit the width limit at each column
			}
			if (widthLimit>(width-(x+(text.size()/widthLimit)))){ //check if it will overflow off the screen etc etc note:im not checking the y axis this time so if the text is too long the thing will just crash or idk like start overwriting random memory locations which probably isnt good
				widthLimit=width-(x+(text.size()/widthLimit));
			}
			if (rows>=heightLimit && heightLimit>1){
				rows=heightLimit;
			}
			for (int j=0;j<rows;j++){ //iterate thorugh each row
				for (int i=0;i<widthLimit;i++){ //and then column
					if (j*widthLimit+i==text.size()){ //check if this is the last character
						break; //stop trying to finish the column if theres no text left
					}
				placeChar(x+i,y+j,getIdFromChar(text[j*widthLimit+i])); //overwrite the character at (x+i,y+j) with whatever character is in the current position of the text
          		placeTextColour(x+i,y+j,textCol);
       			}
			}
			if (widthLimit>=1){
				return rows-1;
			}
			else{
				return rows+1;
			}
		}
	
		DisplayWindow crop(int x, int y, int cWidth, int cHeight){
    /*
    crops the window into the specified dimensions
    */
			vector<int> returnColourMap, returnCharMap, returnCharColourMap;
			for (int j=y;j<cHeight+y;j++){
				for (int i=x;i<cWidth+x;i++){
					returnColourMap.emplace_back(colourMap[j*width+i]);
					returnCharMap.emplace_back(charMap[j*width+i]);
					returnCharColourMap.emplace_back(charColourMap[j*width+i]);
				}
			}
			DisplayWindow returnWindow (cWidth,cHeight,returnColourMap,returnCharMap,returnCharColourMap,chars,colours,textColours);
			return returnWindow;
		}
	
		void fill(int chr=0,int col=0,int chrCol=7){
     /*
     fill the screen with character (chr) colour (col) and character colour (chrCol)
     */
			rectangle(0,0,width,height,chr,col,true,chrCol);
		}
  
    int getIdFromChar(char chr){
		/*
		converts a char, chr, into its respective id depending on the charmap of the window
		*/
			for (int i=0;i<chars.size();i++){ //linear search
				if (chars[i][0]==chr){ //if the character at position i is the same as the character in the argument (note: its chars[i][0] instead of chars[i] because chars is a vector of 1 letter strings and not chars, and c++ throws a hissy fit if you dont specify explicitly you want to use the 1st character in a 1 letter string)
					return i; //return the position in which it is
				}
			}
		}
		
	void resizeWindow(int nWidth=-1,int nHeight=-1,int colDef=0,int charDef=0, int charColDef=14){
  /*
  resize the window into nWidthxnHeight (-1 means keep default). col,char and charCol Def are the colour character and character colour to use as fill when extending window above its size
  */
		if (nWidth==-1){
			nWidth=width;
		}
		if (nHeight==-1){
			nHeight=height;
		}
		vector<int> newColMap={};
		vector<int> newCharMap={};
		vector<int> newCharColMap={};
		for (int j=0;j<nHeight;j++){
			for (int i=0;i<nWidth;i++){
				if (i<width && j<height){
					newColMap.emplace_back(colourMap[(j*width)+i]);
					newCharMap.emplace_back(charMap[(j*width)+i]);
					newCharColMap.emplace_back(charColourMap[(j*width)+i]);
				}
				else{
					newColMap.emplace_back(colDef);
					newCharMap.emplace_back(charDef);
					newCharColMap.emplace_back(charColDef);
				}
			}
		}
		width=nWidth;
		height=nHeight;
		colourMap=newColMap;
		charMap=newCharMap;
		charColourMap=newCharColMap;
	}
	
	void moveWindow(int newX=-1,int newY=-1,int newZ=-1){
  /*
  change the (x,y,z) position of the window
  */
		if (newX==-1){
			newX=get<0>(position);
		}
		if (newY==-1){
			newY=get<1>(position);
		}
		if (newZ==-1){
			newZ=get<2>(position);
		}
		position=make_tuple(newX,newY,newZ);
	}
};

class MainWindow: public DisplayWindow{ //This is the main GUI container that acts as the console screen itself
  using DisplayWindow::DisplayWindow;
  public:
	vector<int> rootColMap=colourMap;
	vector<int> rootCharMap=charMap;
	vector<int> rootCharColMap=charColourMap;
	map<int,DisplayWindow> windows;
  vector<tuple<int,int,int,int,int>> oneOffs={};  //one-off x,y,char,col,txtCol
  string debugText="";
	
		int msgBox(int winId,int x,int y,string text,int outlineCol,int outlineChr,int fillCol,int fillChr,int mWidth=0,int mHeight=0,int extraWidth=0,int extraHeight=0){
    /*
    Create an outlined message box. winId: ID of the window to use as container for the message box. (x,y) position, text is the message to be displayed, 3 outline style arguments, 3 fill style arguments, the width and height of the box itself, the extra width and height to add on top of that that wont be used by the text
    */
			DisplayWindow &win=windows.find(winId)->second; //find the window referred to by the winId
			if (mWidth==0){ //Make sure the msgBox doesnt attempt to overflow out of its window size
				mWidth=text.size();
			}
			if (mWidth+x>width){
				mWidth=width-x-2;
			}
			if (mHeight==0){
				mHeight=(text.size()/mWidth +1);
			}
			if (mHeight+y>height){
				mHeight=height-y-2;
			}
			win.resizeWindow(mWidth+2+extraWidth,mHeight+1+extraHeight); //make the window the required size
			win.moveWindow(x,y); //move the window to the desired position
			win.fill(); //clear the window of any previous data
			win.rectangle(0,0,mWidth+2+extraWidth,mHeight+1+extraHeight,outlineCol,outlineChr,false);//create the outline
			win.rectangle(1,1,mWidth+extraWidth,mHeight+extraHeight-1,fillCol,fillChr,true); //create the fill
			win.drawText(1,1,text,mWidth,mHeight); //draw the text in the body of the message box
			int retrn=mHeight;
			return retrn; //return how many lines the text required
		}
  
    int selectList(int winId,int x,int y,int nextKey,int prevKey,int confirmKey,int outlineCol,int outlineChr,int fillCol,int fillChr,vector<string> options,string text="",int offsetX=0,int offsetY=0,int selectCol=-1,int selectStart=0,int selectEnd=0){
    /*
    A gui window that displays different options the user can select from and returns an int specifying which option was chosen 
    */
      
      DisplayWindow &win=windows.find(winId)->second; //find the window referred to by then winId
      int selection=0;
      int xtraHeight=0;
      int xtraWidth=0;
      for (int i=0;i<options.size();i++){  //make sure the selectList is large enough to contain the largest option
        if (options[i].size()>xtraWidth){
          xtraWidth=options[i].size();
        }
      }
      xtraWidth-=5;
      string prefix,suffix;
      int lastKey; //which keyboard key was pressed last
      bool first=true; //if this is the first iteration of the loop
      while (lastKey!=confirmKey){ //make the window function until an option is chosen
        if (!first){ //if this isnt the first time wait for key press, the purpose of this is to allow the program to draw the main body of the selection list at least once before pausing
          lastKey=waitForKeyPress();
        }
        if (lastKey==nextKey && selection<options.size()-1){ //scroll the selection if possible
          selection++;
        }
        else if (lastKey==prevKey && selection>0){
          selection--;
        }
        xtraHeight=options.size();
        msgBox(winId,x,y,text,outlineCol,outlineChr,fillCol,fillChr,0,0,xtraWidth,xtraHeight);//create the body of the selection box
        for (int i=0;i<options.size();i++){ //for every option supplied
          prefix=" ";
          suffix=" ";
          if (i==selection){ //draw the selected item in the specified style
            prefix=chars[selectStart];
            suffix=chars[selectEnd];
            if (selectCol!=-1){
            win.rectangle(offsetX+1,offsetY+i+2,options[i].size()+2,1,selectCol,fillChr);
            }
          }
          else if (selectCol!=-1){ //draw the rest regularly
            win.rectangle(offsetX+1,offsetY+i+2,options[i].size()+2,1,fillCol,fillChr);
          }
          win.drawText(offsetX+1,offsetY+i+2,prefix+options[i]+suffix,0,0);//draw the text for each option
         }
         if (first){ //make sure after the first run the first varaible is set to false
           first=false;
         }
         draw(); //refresh the screen
       }
      return selection; //return the index of the selected option
     }
	
	
		string textEntry(int x,int y){
    /*
    create a cin text entry at position (x,y)
    */
			string clearCode="\033["+to_string(y+1)+";"+to_string(x+1)+"H";
			cout << "\033[s";
			cout << clearCode;
			string output;
			getline(cin,output);
			cout << "\033[u";
			return output;
		}
	
		string textPrompt(int winId,int x,int y,int outlineCol,int outlineChr,int fillCol,int fillChr,int sWidth=0,int sHeight=0,string text=""){
    /*
    create a msgBox with a text entry
    */
			int off=msgBox(winId,x,y,text,outlineCol,outlineChr,fillCol,fillChr,sWidth,sHeight,0,1); //create a standard message box
			draw();
			return textEntry(x+1,y+(off)); //and follow it through with a text entry
		}
  
    void draw(){
    /*
    outputs all GUI data into the console and displays it
    */
			vector<int> drawOrder={}; //vector which holds the ids of windows in the order they should be drawn in (if one window is drawn after another it will overwrite the overlapping sections making it seem like its on top of it)
			for (auto const& x : windows){ //this iterator goes through every window and sorts it into the drawOrder vector depending on the zPos (3rd item in position tuple) so windows with a higher zPos will be drawn on top of windows with lower zPos
				if (drawOrder.size()==0){ //if theres nothing in the list yet just put it in
					drawOrder.emplace_back(x.first);
					continue;
				}
        if (drawOrder.size()==1){ //if theres only one thing in drawOrder theres only 2 possible places you could insert it
					if (get<2>(x.second.position)<=get<2>((windows.find(drawOrder[0])->second).position)){ //if its smaller than the one item in the drawOrder place it at the back
						drawOrder.emplace_back(x.first);
					}
					else{
						drawOrder.emplace(drawOrder.begin(),x.first); //if its larger place it at the front
					}
					continue;
				}
				for (int i=0;i<drawOrder.size();i++){ //when there is more items run a loop to put the window id in the correct place
						if (i==drawOrder.size()-1 && drawOrder.size()>1){ //does the item belong after this entry
							drawOrder.emplace_back(x.first);
							break;
						}
						if (get<2>(x.second.position)>=get<2>((windows.find(drawOrder[i])->second).position)){ //or before it
							drawOrder.emplace(drawOrder.begin()+(i),x.first);
							break;
						}
				}
			}
			for (int i=drawOrder.size()-1;i>-1;i--){ //cycle through draw order and draw the windows in reverse order as they were sorted in reverse
				DisplayWindow win=windows.find(drawOrder[i])->second;
				drawOnto(get<0>(win.position),get<1>(win.position),win);
			}
      for (int i=0;i<oneOffs.size();i++){ //place any one off characters, colours and text colours in their desired position on the screen
        if (get<2>(oneOffs[i])!=-1){placeChar(get<0>(oneOffs[i]),get<1>(oneOffs[i]),get<2>(oneOffs[i]));}
        if (get<3>(oneOffs[i])!=-1){placeColour(get<0>(oneOffs[i]),get<1>(oneOffs[i]),get<3>(oneOffs[i]));}
		    if (get<4>(oneOffs[i])!=-1){placeTextColour(get<0>(oneOffs[i]),get<1>(oneOffs[i]),get<4>(oneOffs[i]));}
      }
			cout << "\033[2J\033[1;1H"; //ANSI code for clearing the screen and moving the cursor to the top
			for (int j=0;j<height;j++){ //iterate through each row
				for (int i=0;i<width;i++){ //iterate through each column within the row
					cout << textColours[charColourMap[(j*width)+i]] << colours[colourMap[(j*width)+i]] << chars[charMap[(j*width)+i]]; //add to the console the character at that position (j=y coordinate i=x coordinate)
				}
				cout << endl; //when each row is finished add an endline character to start the next row
			}
			cout << "\033[40m\033[97m"; //change colour back to black
      if (debugText!=""){cout << debugText << endl;} //if there is debug text output it after the screen is drawn
			colourMap=rootColMap; //reset the screen
			charMap=rootCharMap;
			charColourMap=rootCharColMap;
		};
	
		int addWindow(int a,int b,vector<int> c,vector<int> d,vector<int> e,const vector<string> &f,const vector<string> &g,const vector<string> &h,int xPos=0,int yPos=0,int zPos=0){
    /*
    create and insert a window into the GUI windows map
    */
			int id=a+b+xPos+yPos+zPos; //ideally the id should be a random int but its much more efficient to just add random arbitrary values that should be at least somewhat unique to each window
			while (windows.find(id)!=windows.end()){ //make sure the id isnt taken
				id++;
			}
			windows.emplace(id,DisplayWindow (a,b,c,d,e,f,g,h,xPos,yPos,zPos));
			return id;
		}
  
    int addWindow(DisplayWindow& win,int xPos=0,int yPos=0,int zPos=0){
    /*
    insert an existing window into the GUI windows map
    */
			int id=get<0>(win.getSize())+get<1>(win.getSize())+xPos+yPos+zPos; //ideally the id should be a random int but its much more efficient to just add random arbitrary values that should be at least somewhat unique to each window
			while (windows.find(id)!=windows.end()){ //make sure the id isnt taken
				id++;
			}
			windows.emplace(id,win);
			return id;
		}
		
    DisplayWindow& getWindow(int id){
    /*
    Get the reference to a window
    */
      return windows.find(id)->second;
    }
  
		void closeWindow(int id){
    /*
    close the window with specified id
    */
      if (id>0){
			  windows.erase(id);
      }
		}
  
    
};