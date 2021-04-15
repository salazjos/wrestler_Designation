/****************
 Name: Joseph Salazar
 Email: salazjos@oregonstate.edu
 Class: CS 325
 Assignment: HW 5 - Problem #3 Wrestler Designation
 File name: wrestler.cpp
 Description: This program reads data from a text file passed by command line argument.
 The program reads in name of the wrestlers, their rivalries, and uses BFS to determine
 if babyface/heel designation for all wrestlers is possible.
 *****************/
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <iterator>
#include <queue>
#include <stdexcept>
using std::ifstream;
using std::ofstream;
using std::string;
using std::cout;
using std::vector;
using std::ios;
using std::endl;
using std::map;
using std::queue;

/*
 Enun to hold the colors used the graph vector structs.
 blue = babyface
 red = heel
 white = unfilled
 */
enum Colors{blue, red, white};

/*
 Struct to represent the vectors of the graph.
 name = name of the wrestler
 graphVectColor = the color of the vector
 edgesName = string vector to hold the names of
 it's adjacent wrestlers (graph vectors)
 */
struct GraphVectorStruct{
    string name;
    Colors graphVectColor; //enum to hold the color of this struct
    vector<string> edgesName;
};

/************
 Function description: Reads a line of numbers as a string from the ifstream
 object. Strings is then added to the vector string.
 Return type: void
 Parameters: vector<string> &vectString, reference to string vector to hold
 each line of numbers.
 ifstream &ifs, refernece to ifstream object to read the input file.
 **********/
void readStringsFromFile(vector<string> &vectString, ifstream &ifs){

    string number_line;

    //copy the line of numbers into the string vector
    while(!ifs.eof()){
        getline(ifs, number_line);
        vectString.push_back(number_line);
    }
    //close the input file
    ifs.close();
}

/************
 Function description: Creates new string array to hold the names of the wrestlers. Fills the array
 with wrestler's names from vectString.
 Return type: void
 Parameters: vector<string> &vectString, reference to string vector with lines from input file.
             int &index, int reference to the line index of the text file.
             string *&strArray, pointer to the string array that holds wrestlers names.
             int &wAmt, int reference to the amount of wrestlers.
 **********/
void populateWrestlerArray(vector<string> &vectString, int &lineIndex, string *&strArray, int &wAmt){
    //first line in vector is number of wrestlers
    int i = 0, wrestlerAmount = 0;
    string firstNumberStr = vectString.at(lineIndex);
    while(i < 1){
        std::istringstream iss(firstNumberStr);
        iss >> wrestlerAmount;
        i++;
    }
    wAmt = wrestlerAmount;
    lineIndex++;

    //create the strArray
    strArray = new string[wrestlerAmount];
    //fill the array with wrestler names
    string name;
    for(int j = 0; j < wrestlerAmount; j++){
        name = vectString.at(lineIndex);
        int length = (int)name.length();
        if(name.at(length - 1) == '\r')
            name.erase(length - 1);
        strArray[j] = name;
        lineIndex++;
    }
    //lineIndex = i;
}

/************
 Function description: Fills the GraphVectorStruct structs with the name of the wrestlers.
 Return type: void
 Parameters: GraphVectorStruct *graphVectArr, pointer the array of GraphVectorStruct
             structs
             string *strArray, pointer to the string with wrestler names.
             int arrSize, size of the strArray
 **********/
void fillGraphVectStructArray(GraphVectorStruct *graphVectArr, string *strArray, int arrSize){
    string name;
    for(int i = 0; i < arrSize; i++){
        name = strArray[i];
        graphVectArr[i].name = name;
        graphVectArr[i].graphVectColor = white;
    }
}

/************
 Function description: Fills graphMap with pointers to GraphVectorStruct structs.
 The key is wrestler name, value is the address of GraphVectorStruct struct at index location.
 Parameters: map<string, GraphVectorStruct*> &graphMap, reference to the map of wrestler names
             and pointers to GraphVectorStruct structs.
             GraphVectorStruct *graphVectArr, pointer the array of GraphVectorStruct
             structs
             int arrSize, the number of wrestlers
 **********/
void fillMap(map<string, GraphVectorStruct*> &graphMap, GraphVectorStruct *graphVectArr,
             int arrSize){

    for(int i = 0; i < arrSize; i++){
        graphMap.insert(std::pair<string, GraphVectorStruct*>(graphVectArr[i].name, &graphVectArr[i]));
    }
}

/************
 Function description: Read in the rivalaries from the vector string and puts into 2d string array.
 Return type: void
 Parameters: vector<string> &vectString, vector holding all the strings read from input file
             string *strArray, pointer to string array with wrestler names.
             int arrSize, size of the *stArray.
             string **&r_array, 2d string array of the wrestler rivalries.
             int &rival_amt, the row amount of the 2d string array.
 **********/
void populateRivalArray(vector<string> &vectString, int &index, int arrSize, string **&r_array, int &rival_amt) {
    //get the number of rivalries
    int rivalryAmount = 0, i = 0;
    string rivalryAmtLine = vectString.at(index);
    std::istringstream iss(rivalryAmtLine);
    iss >> rivalryAmount;
    rival_amt = rivalryAmount;
    index++;

    //Create the 2d rivalry array
    r_array = new string*[rivalryAmount];
    for(int tempIndex1 = 0; tempIndex1 < arrSize; tempIndex1++)
        r_array[tempIndex1] = new string[2];

    //fill the 2d string array with rivalries.
    string rivalryStrLine, wrestlerName;
    int j = 0, g = 0;
    for(int tempIndex2 = 0; tempIndex2 < rivalryAmount; tempIndex2++){
        rivalryStrLine = vectString.at(index);
        index++;
        std::istringstream numberStringStream(rivalryStrLine);
        while(numberStringStream >> wrestlerName){
            r_array[j][g] = wrestlerName;
            g++;
        }
        j++;
        g = 0;
    }
}

/************
 Function description: Looks for the wrestler name in the 2d array at a given row.
 If found in either column, return the rival name. If not found, return the string "no".
 Return type: string
 Parameters: string wrestler, the name of the wrestler being searched for
             string **r_array, pointer to 2d string array that has rivals names.
             int row, row in the r_array array
             int col, col in the r_array array
 **********/
string opposingWrestler(const string& wrestler, string **r_array, int row, int col){

    if(wrestler == r_array[row][col])
        return r_array[row][col+1];
    else if(wrestler == r_array[row][col + 1])
        return r_array[row][col];
    else
        return "no";
}

/************
 Function description: Fills the adjacency list. Loop through GraphVectorStruct array of
 structs, find wrestler name rivalry array, then find opposing wrestler and add opposing
 wrestler name to the string vactor of the current GraphVectorStruct struct.
 Return type: void
 Parameters: GraphVectorStruct *graphVectArr, pointer the array of GraphVectorStruct
             structs
             int arrSize, amount of wrestlers
             string **r_array, pointer to 2d string array told hold wrestler rivalries
             int rivalAmount, the number of row in the **r_array 2d array.
 **********/
void fillAdjacenyList(GraphVectorStruct *graphVectArr, int arrSize, string **r_array,
                      int rivalAmount){

    string wrestler_name, rival_wrestler_name;

    for(int i = 0; i < arrSize; i++){
        wrestler_name = graphVectArr[i].name;
        for(int j = 0; j < rivalAmount; j++){
            rival_wrestler_name = opposingWrestler(wrestler_name,r_array, j, 0);
            if(rival_wrestler_name != "no"){
                graphVectArr[i].edgesName.push_back(rival_wrestler_name);
            }
        }
    }
}

/************
 Function description: Loop through array of GraphVectorStruct structs to find first struct
 that has color white. Return struct pointer.
 Return type: GraphVectorStruct*
 Parameters: GraphVectorStruct *graphVectArr, pointer the array of GraphVectorStruct
             structs
             int arrSize, amount of wrestlers.

 **********/
GraphVectorStruct *findWhiteGraphStructs(GraphVectorStruct *graphVectArr, int arraySize){
    struct GraphVectorStruct *tempStruct = nullptr;
    for(int i = 0; i < arraySize; i++){
        if(graphVectArr[i].graphVectColor == white){
            tempStruct = &graphVectArr[i];
            break;
        }
    }
    return tempStruct;
}

/************
 Function description: Helper function to isWrestlerDesignationPossible(). Uses BFS to determine
 if babyface/heel designation is possible. Use 2-color graph technique as demonstrated on page 595 of
 text book. .
 Return type: bool
 Parameters: GraphVectorStruct *graphVectArr, pointer the array of GraphVectorStruct
             structs
             int arrSize, the amount of wrestlers
             int &coloredVectorsAmt, reference to the amount of vectors colored blue or red.
             GraphVectorStruct *firstStruct, pointer to the first struct to be used in BFS
             map<string, GraphVectorStruct*> &graphMap, reference to the map of wrestler names
             and pointers to GraphVectorStruct structs.
 Modified version from:
 “Chapter 22.2” Introduction to Algorithms, by Thomas H. Cormen, Charles Leiserson,
 Ronald Rivest, Clifford Stein, MIT Press, 2009.
 **********/
bool isDesignationPossible(GraphVectorStruct *graphVectArr, int &coloredVectorsAmt, GraphVectorStruct *firstStruct,
                           map<string, GraphVectorStruct *> &graphVectorMap) {

    queue<GraphVectorStruct*> vectorQueue;
    struct GraphVectorStruct *tempVectStruct = nullptr, *adjacentVector = nullptr;
    string tempName;

    tempVectStruct = firstStruct;
    tempVectStruct->graphVectColor = blue;
    coloredVectorsAmt++;
    vectorQueue.push(tempVectStruct);

    while(!vectorQueue.empty()){
        tempVectStruct = vectorQueue.front();
        vectorQueue.pop();
        for(int i = 0; i < tempVectStruct->edgesName.size(); i++){
            tempName = tempVectStruct->edgesName.at(i);
            adjacentVector = graphVectorMap[tempName];
            //test the colors between parent(tempVectStruct) and child (adjacentStruct)
            if(adjacentVector->graphVectColor == white){
                //if the child color is white then switch it to opposite its parent
                adjacentVector->graphVectColor = (tempVectStruct->graphVectColor == blue) ? red : blue;
                coloredVectorsAmt++;
                //add it the queue
                vectorQueue.push(adjacentVector);
            }
            else{//its adjacent already has a color
                if(adjacentVector->graphVectColor == tempVectStruct->graphVectColor)
                    return false; //designation not possible
            }
        }
    }
    return true; //designation is possible
}

/************
 Function description: Uses helped function isWrestlerDesignationPossible() to determine if
 babyface/heel designation is possible given the amount of wrestlers.
 Return type: bool
 Parameters: GraphVectorStruct *graphVectArr, pointer the array of GraphVectorStruct
             structs
             int arrSize, amount of wrestlers.
             map<string, GraphVectorStruct*> &graphVectorMap, reference to the map
             of wrestler names and pointers to GraphVectorStruct structs.
 **********/
bool isWrestlerDesignationPossible(GraphVectorStruct *graphVectArr, int arrSize,
                                   map<string, GraphVectorStruct*> &graphVectorMap){
    //bool isPossible;
    int amtColoredVectors = 0;
    struct GraphVectorStruct *firstVectorStruct;

    //continue to loop until all vectors are either blue or red
    //amtColoredVectors = the amount of vectors that are either blue or red
    while( amtColoredVectors < arrSize){
        //find the fist vector with color white
        firstVectorStruct = findWhiteGraphStructs(graphVectArr,arrSize);
        //use BSF to determine possible designation
        bool isPossible = isDesignationPossible(graphVectArr, amtColoredVectors,
                                           firstVectorStruct, graphVectorMap);
        if(!isPossible) //designation not possible
            return false;
    }
    return true;
}

/************
 Function description: Print the names of the wrestler's whose color value is equal to
 to color.
 Return type: void
 Parameters: Colors color, enum color value
             GraphVectorStruct *graphVectArr, pointer the array of GraphVectorStruct
             structs
             int arrSize, amount of wrestlers.
 **********/
void printGraphVectorColors(Colors color,GraphVectorStruct *graphVectArr, int arrSize){
    for(int i = 0; i < arrSize; i++){
        if(graphVectArr[i].graphVectColor == color)
            cout << graphVectArr[i].name << " ";
    }
}

/************
 Function description: Print whether babyface/heel designation was possible. If possible,
 print the names of the babyfaces and the names of the heels.
 Return type: void
 Parameters: bool isPossible, bool to represent whether designation was possible.
             GraphVectorStruct *graphVectArr, pointer the array of GraphVectorStruct
             structs
             int arrSize, amount of wrestlers.
 **********/
void printResults(bool isPossible,GraphVectorStruct *graphVectArr, int arrSize){
    if(!isPossible)
        cout << "No, not possible.\n\n";
    else{
        cout << "Yes, Possible\n";
        cout << "Babyfaces: ";
        printGraphVectorColors(blue,graphVectArr,arrSize);
        cout << "\n";
        cout << "Heels: ";
        printGraphVectorColors(red,graphVectArr,arrSize);
        cout << "\n\n";
    }
}

/*
Main functions assumes the following:
a) no input file provided as command line argument
b) a correct input file relevant to the program is
   provided as command line argument.
No error checking on file is performed.
*/
int main(int argc, const char * argv[]) {

    string INPUT_FILE_NAME; //= "wrestler2.txt";

    if(argc > 1){
        INPUT_FILE_NAME = argv[1];
    }
    else{
        cout << "Input file not passed as command line argument\n";
        cout << "Exiting....\n";
        exit(0);
    }

    ifstream ifs_file;
    int lineIndex = 0, wrestlerAmount = 0, rivalAmt = 0;
    bool isDesignation;
    vector<string> inputStringsVect;

    string *wrestlerArray = nullptr;
    string **twodRivarlStrArray = nullptr;

    GraphVectorStruct *graphVectArray = nullptr;
    string line_from_file;

    map<string, GraphVectorStruct*> graphVectorMap;

    //Open the input file (INPUT_FILE_NAME)
    ifs_file.open(INPUT_FILE_NAME.c_str(), ios::in);
    if(!ifs_file){
        cout << "Could not open file: " << INPUT_FILE_NAME << endl;
        exit(1);
    }

    //read the lines of numbers from the file into a vector.
    //ifs_file closed in function.
    readStringsFromFile(inputStringsVect, ifs_file);

    //read wrestler names into array, initialize wrestlerArray
    populateWrestlerArray(inputStringsVect,lineIndex,wrestlerArray,wrestlerAmount);

    //create array of graph vector structs
    graphVectArray = new GraphVectorStruct[wrestlerAmount];

    //populate the graph vector structs array
    fillGraphVectStructArray(graphVectArray,wrestlerArray,wrestlerAmount);

    //populate map
    fillMap(graphVectorMap, graphVectArray, wrestlerAmount);

    //fill the rivalry array
    populateRivalArray(inputStringsVect, lineIndex, wrestlerAmount,
                       twodRivarlStrArray, rivalAmt);

    //fill adjacency list
    fillAdjacenyList(graphVectArray,wrestlerAmount,twodRivarlStrArray,rivalAmt);

    //test if designation is possible
    isDesignation = isWrestlerDesignationPossible(graphVectArray, wrestlerAmount, graphVectorMap);

    //print message
    printResults(isDesignation,graphVectArray, wrestlerAmount);

    //delete everything crearted with new
    delete [] wrestlerArray;
    delete [] graphVectArray;
    for(int i = 0; i < wrestlerAmount;i++)
        delete [] twodRivarlStrArray[i];
    delete [] twodRivarlStrArray;

    return 0;
}
