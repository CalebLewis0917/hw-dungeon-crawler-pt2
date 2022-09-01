#include <iostream>
#include <fstream>
#include <string>
#include "logic.h"

using std::cout, std::endl, std::ifstream, std::string;

/**
 * TODO: Student implement this function
 * Load representation of the dungeon level from file into the 2D map.
 * Calls createMap to allocate the 2D array.
 * @param   fileName    File name of dungeon level.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object by reference to set starting position.
 * @return  pointer to 2D dynamic array representation of dungeon map with player's location., or nullptr if loading fails for any reason
 * @updates  maxRow, maxCol, player
 */
char** loadLevel(const string& fileName, int& maxRow, int& maxCol, Player& player) {
    ifstream inFS;
    inFS.open(fileName);
    if(inFS.is_open() && !inFS.eof()){
        inFS >> maxRow;
        inFS >> maxCol;
        inFS >> player.row;
        inFS >> player.col;
    }
    char** level = createMap(maxRow,maxCol);
    for(int row = 0; row < maxRow; row++){
        for(int col = 0; col < maxCol; col++){
            if(row==player.row && col==player.col){
                inFS >> level[row][col];
                level[row][col] = 'o';
            }
            else{
                inFS >> level[row][col];
            }
        }
    }
    return level;
}

/**
 * TODO: Student implement this function
 * Translate the character direction input by the user into row or column change.
 * That is, updates the nextRow or nextCol according to the player's movement direction.
 * @param   input       Character input by the user which translates to a direction.
 * @param   nextRow     Player's next row on the dungeon map (up/down).
 * @param   nextCol     Player's next column on dungeon map (left/right).
 * @updates  nextRow, nextCol
 */
void getDirection(char input, int& nextRow, int& nextCol) {
    if(input=='w'){
        nextRow--;
    }
    else if(input=='a'){
        nextCol--;
    }
    else if(input=='s'){
        nextRow++;
    }
    else if(input=='d'){
        nextCol++;
    }
}

/**
 * TODO: [suggested] Student implement this function
 * Allocate the 2D map array.
 * Initialize each cell to TILE_OPEN.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @return  2D map array for the dungeon level, holds char type.
 */
char** createMap(int maxRow, int maxCol) {
    char** level = new char*[maxRow]; 
    for(int i = 0; i < maxRow; i++){
        level[i] = new char[maxCol];
    }
    return level;
}

/**
 * TODO: Student implement this function
 * Deallocates the 2D map array.
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @return None
 * @update map, maxRow
 */
void deleteMap(char**& map, int& maxRow) {
    if (map) {
        for (int row = 0; row < maxRow; row++) {
            delete[] map[row];
        }
        delete[] map;
    }
    map = nullptr;
    maxRow = 0;
}

/**
 * TODO: Student implement this function
 * Resize the 2D map by doubling both dimensions.
 * Copy the current map contents to the right, diagonal down, and below.
 * Do not duplicate the player, and remember to avoid memory leaks!
 * You can use the STATUS constants defined in logic.h to help!
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height), to be doubled.
 * @param   maxCol      Number of columns in the dungeon table (aka width), to be doubled.
 * @return  pointer to a dynamically-allocated 2D array (map) that has twice as many columns and rows in size.
 * @update maxRow, maxCol
 */
char** resizeMap(char** map, int& maxRow, int& maxCol) {
    if(map==nullptr || map==NULL){
        return nullptr;
    }
    else if(maxRow==0 || maxCol==0 || INT32_MAX/2 < maxRow || INT32_MAX/2 < maxCol){
        return nullptr;
    }
    char** level = createMap(maxRow*2,maxCol*2);
    for(int row = 0; row < maxRow; row++){
        for(int col = 0; col <maxCol; col++){
            if(map[row][col]=='o'){
                level[row][col] = 'o';
                level[row+maxRow][col] = '-';
                level[row][col+maxCol] = '-';
                level[row+maxRow][col+maxCol] = '-';
            }
            else{
                level[row][col] = map[row][col];
                level[row+maxRow][col] = map[row][col];
                level[row][col+maxCol] = map[row][col];
                level[row+maxRow][col+maxCol] = map[row][col];
            }
        }
    }
    int temp = maxRow;
    deleteMap(map,temp);
    maxRow *= 2;
    maxCol *= 2;
    return level;
}

/**
 * TODO: Student implement this function
 * Checks if the player can move in the specified direction and performs the move if so.
 * Cannot move out of bounds or onto TILE_PILLAR or TILE_MONSTER.
 * Cannot move onto TILE_EXIT without at least one treasure. 
 * If TILE_TREASURE, increment treasure by 1.
 * Remember to update the map tile that the player moves onto and return the appropriate status.
 * You can use the STATUS constants defined in logic.h to help!
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object to by reference to see current location.
 * @param   nextRow     Player's next row on the dungeon map (up/down).
 * @param   nextCol     Player's next column on dungeon map (left/right).
 * @return  Player's movement status after updating player's position.
 * @update map contents, player
 */
int doPlayerMove(char** map, int maxRow, int maxCol, Player& player, int nextRow, int nextCol) {
    if(nextRow>=maxRow || nextRow<0 || nextCol>=maxCol || nextCol<0){
        return STATUS_STAY;
    }
    else if(map[nextRow][nextCol]=='M' || map[nextRow][nextCol]=='+'){
        return STATUS_STAY;
    }
    else if(map[nextRow][nextCol]=='!' && player.treasure<1){
        return STATUS_STAY;
    }
    else{
        char tempTile = map[nextRow][nextCol];
        map[nextRow][nextCol] = 'o';
        map[player.row][player.col] = '-';
        player.row = nextRow;
        player.col = nextCol;
        if(tempTile=='$'){
            player.treasure++;
            return STATUS_TREASURE;
        }
        else if(tempTile=='@'){
            return STATUS_AMULET;
        }
        else if(tempTile=='?'){
            return STATUS_LEAVE;
        }
        else if(tempTile=='!'){
            return STATUS_ESCAPE;
        }
        return STATUS_MOVE;
    }
    return 0;
}

/**
 * TODO: Student implement this function
 * Update monster locations:
 * We check up, down, left, right from the current player position.
 * If we see an obstacle, there is no line of sight in that direction, and the monster does not move.
 * If we see a monster before an obstacle, the monster moves one tile toward the player.
 * We should update the map as the monster moves.
 * At the end, we check if a monster has moved onto the player's tile.
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object by reference for current location.
 * @return  Boolean value indicating player status: true if monster reaches the player, false if not.
 * @update map contents
 */
bool doMonsterAttack(char** map, int maxRow, int maxCol, const Player& player) {
    char tempTile = map[player.row][player.col];
    // up
    for(int row = player.row-1; row >= 0; row--){
        tempTile = map[row][player.col];
        if(tempTile=='+'){
            break;
        }
        else if(tempTile=='M'){
            map[row+1][player.col] = 'M';
            map[row][player.col] = '-';
        }
    }
    // down
    for(int row = player.row+1; row < maxRow; row++){
        tempTile = map[row][player.col];
        if(tempTile=='+'){
            break;
        }
        else if(tempTile=='M'){
            map[row-1][player.col] = 'M';
            map[row][player.col] = '-';
        }
    }
    // left
    for(int col = player.col-1; col >= 0; col--){
        tempTile = map[player.row][col];
        if(tempTile=='+'){
            break;
        }
        else if(tempTile=='M'){
            map[player.row][col+1] = 'M';
            map[player.row][col] = '-';
        }
    }
    // right
    for(int col = player.col+1; col < maxCol; col++){
        tempTile = map[player.row][col];
        if(tempTile=='+'){
            break;
        }
        else if(tempTile=='M'){
            map[player.row][col-1] = 'M';
            map[player.row][col] = '-';
        }
    }
    if(map[player.row][player.col] == 'M'){
        return true;
    }
    return false;
}
