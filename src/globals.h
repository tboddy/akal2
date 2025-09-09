#define FRAME_INDEX TILE_USER_INDEX + 128

bool started, paused;

// controls
struct controls {
	bool left, right, up, down, a, b, c, start;
};
struct controls ctrl;

void updateControls(u16 joy, u16 changed, u16 state){
	if(changed){}
	if(joy == JOY_1){
		ctrl.left = (state & BUTTON_LEFT);
		ctrl.right = (state & BUTTON_RIGHT);
		ctrl.up = (state & BUTTON_UP);
		ctrl.down = (state & BUTTON_DOWN);
		ctrl.a = (state & BUTTON_A);
		ctrl.b = (state & BUTTON_B);
		ctrl.c = (state & BUTTON_C);
		ctrl.start = (state & BUTTON_START);
	}
}

// main
void loadGame();
void movePlayer();
void updatePlayer();
void updateCamera();
void updateVisibilityWithTracking(s16 playerX, s16 playerY);
void renderVisibleTilesOnly();
void transitionToNextLevel();
void updateUi();

char debugStrOne[2];
char debugStrTwo[2];

// map
#define MAP_WIDTH     32 
#define MAP_HEIGHT    32
u8 currentLevel;
u16 currentTurn;
u8 mapData[MAP_HEIGHT][MAP_WIDTH];
u8 visibilityMap[MAP_HEIGHT][MAP_WIDTH];
u8 previousVisibilityMap[MAP_HEIGHT][MAP_WIDTH]; 
bool visibilityChanged[MAP_HEIGHT][MAP_WIDTH]; 

#define TILE_WALL  0
#define TILE_FLOOR 1
#define TILE_SHADOW 2 
#define TILE_STAIRWELL 3 
#define TILE_FOG 4

#define LOGICAL_TILE_SIZE 16

#define MIN_ROOM_SIZE 4
#define MAX_ROOM_SIZE 8
#define MAX_ROOMS 6

#define LEVEL_TRANSITION_LIMIT 480
u16 levelTransitionClock;

#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 224

#define GAMEPLAY_AREA_WIDTH  208
#define GAMEPLAY_AREA_HEIGHT 208
#define GAMEPLAY_OFFSET_X    48
#define GAMEPLAY_OFFSET_Y    0
#define GAMEPLAY_CENTER_X    (GAMEPLAY_OFFSET_X + (GAMEPLAY_AREA_WIDTH / 2))  
#define GAMEPLAY_CENTER_Y    (GAMEPLAY_OFFSET_Y + (GAMEPLAY_AREA_HEIGHT / 2)) 

u16 simpleRandom(u16 max) {
	return random() % max;
}

u8 rollDice(){
	return (random() % 6) + 1;
}

u16 clock;

// player
s16 cameraX, cameraY;
struct playerStruct {
	Vect2D_s16 pos, tilePos, lastPos;
	Sprite* image;
	s8 hp, atk, def, wpn, arm, maxHp;
	u8 currentItem;
};
struct playerStruct player;
void killPlayer();

// enemies
#define ENEMY_COUNT 10
u8 enemyCount;
struct enemyStruct {
	bool seen, wasJustHit;
	Vect2D_s16 pos, tilePos,
		lastPos;
	Sprite* image;
	s8 hp, atk, def, wpn, arm, maxHp;
	u8 lastDirection, type, moveCounter;
};
struct enemyStruct enemies[ENEMY_COUNT];
void spawnEnemies();
void updateEnemies(bool canAttack);
void bounceEnemies();
void clearEnemies();
void killEnemy(u8 i);
void attackPlayerAgainstEnemy(u8 i);
s16 bounceOffset;

// items
#define ITEM_COUNT 8
struct itemStruct {
	Vect2D_s16 pos;
	Sprite* image;
	u8 type;
};
struct itemStruct items[ITEM_COUNT];
#define ITEM_TYPE_TONIC 0
#define ITEM_TYPE_ELIXIR 1
#define ITEM_TYPE_ANTIDOTE 2
#define ITEM_TYPE_PHIAL 3
#define ITEM_TYPE_STAFF 4
#define ITEM_TYPE_AXE 5
#define ITEM_TYPE_BOW 6
#define ITEM_TYPE_CBOW 7
#define ITEM_TYPE_ROBE 8
#define ITEM_TYPE_MAIL 9
#define ITEM_TYPE_PLATE 10
// utility
bool isWalkable(s16 tileX, s16 tileY){
	// Check bounds first
	if(tileX < 0 || tileX >= MAP_WIDTH || tileY < 0 || tileY >= MAP_HEIGHT)
		return FALSE;
	
	// Check if tile is floor and tile above is also floor (for proper collision)
	if(mapData[tileY][tileX] == TILE_FLOOR && mapData[tileY - 1][tileX] == TILE_FLOOR) {
		// Check if any enemy is already at this position
		for(int i = 0; i < ENEMY_COUNT; i++) {
			if(enemies[i].tilePos.x == tileX && enemies[i].tilePos.y == tileY) {
				return FALSE;
			}
		}
		return TRUE;
	}
	return FALSE;
}

// log
char logStr[24];
