// #define SIDEBAR_X 28
// #define SIDEBAR_Y 2

#define FRAME_X 6

void drawFrame(){
  VDP_loadTileSet(frameFade.tileset, FRAME_INDEX, DMA);

	VDP_drawImageEx(BG_A, &frameL, TILE_ATTR_FULL(PAL1, 1, 0, 0, FRAME_INDEX + 8), 0, 0, 0, 1);
	VDP_drawImageEx(BG_A, &frameR, TILE_ATTR_FULL(PAL1, 1, 0, 0, FRAME_INDEX + 8 + 196), 7 + 26, 0, 0, 1);
	VDP_drawImageEx(BG_A, &frameT, TILE_ATTR_FULL(PAL1, 1, 0, 0, FRAME_INDEX + 8 + 196 + 196), 7, 0, 0, 1);
	VDP_drawImageEx(BG_A, &frameB, TILE_ATTR_FULL(PAL1, 1, 0, 0, FRAME_INDEX + 8 + 196 + 196 + 26), 7, 27, 0, 1);

  VDP_fillTileMapRect(BG_A, TILE_ATTR_FULL(PAL1, 1, 0, 0, FRAME_INDEX), 7, 2, 1, 24);
  VDP_fillTileMapRect(BG_A, TILE_ATTR_FULL(PAL1, 1, 0, 0, FRAME_INDEX + 1), 7 + 25, 2, 1, 24);
  VDP_fillTileMapRect(BG_A, TILE_ATTR_FULL(PAL1, 1, 0, 0, FRAME_INDEX + 2), 8, 1, 24, 1);
  VDP_fillTileMapRect(BG_A, TILE_ATTR_FULL(PAL1, 1, 0, 0, FRAME_INDEX + 3), 8, 1 + 25, 24, 1);

  VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, 0, 0, FRAME_INDEX + 4), 7, 1);
  VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, 0, 0, FRAME_INDEX + 5), 7 + 25, 1);
  VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, 0, 0, FRAME_INDEX + 6), 7, 1 + 25);
  VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, 0, 0, FRAME_INDEX + 7), 7 + 25, 1 + 25);
}

s8 lastHp, lastMaxHp;
u8 lastCurrentItem;

void loadUi(){
	lastHp = 99;
	lastMaxHp = 99;
	lastCurrentItem = 99;
	drawFrame();
}

u8 lessTen(u8 input){
	return input < 10 ? 1 : 2;
}

char levelStr[2];
char hpStr[2];
char maxHpStr[2];
char atkStr[2];
char defStr[2];
char wpnStr[2];
char armStr[2];


static void updateStats(){
	// hp
	if(lastHp != player.hp || lastMaxHp != player.maxHp){
		VDP_clearText(11, 2, 5);
		VDP_drawText("hp", 8, 2);
		intToStr(player.hp, hpStr, lessTen(player.hp));
		VDP_drawText(hpStr, 11, 2);
		VDP_drawText("/", 11 + lessTen(player.hp), 2);
		intToStr(player.maxHp, maxHpStr, 2);
		VDP_drawText(maxHpStr, 12 + lessTen(player.hp), 2);
		lastHp = player.hp;
		lastMaxHp = player.maxHp;
	}

	// other stats

	VDP_drawText("atk+", 18, 2);
	intToStr(player.atk, atkStr, lessTen(player.atk));
	VDP_drawText(atkStr, 22, 2);

	VDP_drawText("def+", 26 + (player.def < 10 ? 1 : 0), 2);
	intToStr(player.def, defStr, lessTen(player.def));
	VDP_drawText(defStr, 30 + (player.def < 10 ? 1 : 0), 2);

	if(lastCurrentItem != player.currentItem){
		VDP_clearText(8, 4, 8);
		switch(player.currentItem){
			case 0:
				VDP_drawText("staff+2", 8, 4);
				break;
			case 1:
				VDP_drawText("bow+2", 8, 4);
				break;
			case 2:
				VDP_drawText("potion", 8, 4);
				break;
			case 3:
				VDP_drawText("potion", 8, 4);
				break;
		}
		for(u8 i = 0; i < 4; i++){
			VDP_drawText(i == player.currentItem ? "@" : "`", 8 + i, 3);
		}
		lastCurrentItem = player.currentItem;
	}

	VDP_drawText("plate+2", 25, 3);
	VDP_drawText("lvl", 26 + (currentLevel < 10 ? 1 : 0), 25);
	intToStr(currentLevel, levelStr, lessTen(currentLevel));
	VDP_drawText(levelStr, 30 + (currentLevel < 10 ? 1 : 0), 25);

	// VDP_drawText("DAGGER", SIDEBAR_X, SIDEBAR_Y + 6);
	// intToStr(player.wpn, wpnStr, charLessTen(player.wpn));
	// VDP_drawText("+", SIDEBAR_X + strLessTen(player.wpn) - 1, SIDEBAR_Y + 6);
	// VDP_drawText(wpnStr, SIDEBAR_X + strLessTen(player.wpn), SIDEBAR_Y + 6);

	// VDP_drawText("NO ARMOR", SIDEBAR_X, SIDEBAR_Y + 7);
	// intToStr(player.arm, armStr, charLessTen(player.arm));
	// VDP_drawText("+", SIDEBAR_X + strLessTen(player.arm) - 1, SIDEBAR_Y + 7);
	// VDP_drawText(armStr, SIDEBAR_X + strLessTen(player.arm), SIDEBAR_Y + 7);
}

// Function to add a new log entry
void addLogEntry(const char* message) {
	char turnStr[4];
	char formattedMessage[LOG_ENTRY_LENGTH];
	
	// Convert turn number to string
	intToStr(currentTurn, turnStr, currentTurn < 10 ? 1 : (currentTurn < 100 ? 2 : 3));
	
	// Format message with turn number
	strcpy(formattedMessage, turnStr);
	strcat(formattedMessage, "-");
	strcat(formattedMessage, message);
	
	if(logQueueCount < MAX_LOG_ENTRIES) {
		strcpy(logQueue[logQueueCount], formattedMessage);
		logQueueCount++;
	} else {
		// Shift all entries up, remove oldest
		for(u8 i = 0; i < MAX_LOG_ENTRIES - 1; i++) {
			strcpy(logQueue[i], logQueue[i + 1]);
		}
		strcpy(logQueue[MAX_LOG_ENTRIES - 1], formattedMessage);
	}
}

static void updateLog(){
	VDP_clearTextArea(8, 23, 18, 3);
	
	// Display the most recent entries (up to 3)
	if(logQueueCount > 0) {
		u8 displayIndex = logQueueCount - 1;
		VDP_drawText(logQueue[displayIndex], 8, 25); // Most recent (bottom line)
		
		if(logQueueCount > 1) {
			VDP_setTextPalette(PAL1);
			VDP_drawText(logQueue[displayIndex - 1], 8, 24); // Second most recent (middle line)
			VDP_setTextPalette(PAL0);
		}
		
		if(logQueueCount > 2) {
			VDP_setTextPalette(PAL1);
			VDP_drawText(logQueue[displayIndex - 2], 8, 23); // Third most recent (top line)
			VDP_setTextPalette(PAL0);
		}
	}
}

void updateUi(){
	updateStats();
	updateLog();
}

#define TRANSITION_X 7
#define TRANSITION_Y 1 + 10

s8 levelUpSelector;
bool levelUpSelecting;

void updateLevelTransition(){
	if(levelTransitionClock == LEVEL_TRANSITION_LIMIT){
		// Clear log queue
		logQueueCount = 0;
		for(u8 i = 0; i < MAX_LOG_ENTRIES; i++) {
			strclr(logQueue[i]);
		}
		VDP_clearTileMapRect(BG_A, 8, 2, 24, 24);
		levelUpSelector = 0;
		levelUpSelecting = TRUE;
		VDP_clearPlane(BG_B, TRUE);
		SPR_setVisibility(player.image, HIDDEN);
		clearEnemies();
		VDP_drawText("LEVEL", TRANSITION_X + 5, TRANSITION_Y);
		VDP_drawText(levelStr, TRANSITION_X + 5 + 6, TRANSITION_Y);
		VDP_drawText("COMPLETE", TRANSITION_X + 5 + 6 + (currentLevel < 10 ? 2 : 3), TRANSITION_Y);
		VDP_drawText("choose stat up", TRANSITION_X + 6, TRANSITION_Y + 1);
		VDP_drawText(">", TRANSITION_X + 10, TRANSITION_Y + 3);
		VDP_drawText("HP", TRANSITION_X + 11, TRANSITION_Y + 3);
		VDP_drawText("ATK", TRANSITION_X + 11, TRANSITION_Y + 4);
		VDP_drawText("DEF", TRANSITION_X + 11, TRANSITION_Y + 5);
		levelTransitionClock--;
	}
	else if(ctrl.up && !levelUpSelecting){
		VDP_clearTileMapRect(BG_A, TRANSITION_X + 10, TRANSITION_Y + 3, 1, 3);
		levelUpSelecting = TRUE;
		levelUpSelector--;
		if(levelUpSelector < 0) levelUpSelector = 2;
		VDP_drawText(">", TRANSITION_X + 10, TRANSITION_Y + 3 + levelUpSelector);
	}
	else if(ctrl.down && !levelUpSelecting){
		VDP_clearTileMapRect(BG_A, TRANSITION_X + 10, TRANSITION_Y + 3, 1, 3);
		levelUpSelecting = TRUE;
		levelUpSelector++;
		if(levelUpSelector > 2) levelUpSelector = 0;
		VDP_drawText(">", TRANSITION_X + 10, TRANSITION_Y + 3 + levelUpSelector);
	}
	else if((ctrl.start || ctrl.a || ctrl.b || ctrl.c) && !levelUpSelecting){
		if(levelUpSelector == 0) player.maxHp++;
		else if(levelUpSelector == 1) player.atk++;
		else if(levelUpSelector == 2) player.def++;
		levelTransitionClock = 0;
		VDP_clearTileMapRect(BG_A, 8, 2, 24, 24);
		transitionToNextLevel();
	}
	else if(levelUpSelecting && (!ctrl.up && !ctrl.down && !ctrl.a && !ctrl.b && !ctrl.c))
		levelUpSelecting = FALSE;
}