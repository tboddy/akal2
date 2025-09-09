u16 turnClock;
bool isTurning;

void loadPlayer(){
	player.image = SPR_addSprite(&playerSprite, 0, 0, TILE_ATTR(PAL0, 0, 0, 0));
	player.hp = 10;
	player.maxHp = 10;
	player.atk = 0;
	player.def = 0;
	player.wpn = 2;
	player.arm = 0;
	player.currentItem = 0;
	turnClock = 0;
}

Vect2D_s16 newPlayerTile;
bool playerCanMove, playerDidAttack, playerTriedAttack;
u8 playerFrame;

u8 attackingEnemy;

static void checkPlayerAttackCollision(bool firstTime){
	if(playerTriedAttack){
		for(int i = 0; i < ENEMY_COUNT; i++) {
			if(firstTime) enemies[i].wasJustHit = FALSE;
			if(enemies[i].hp > 0 && !enemies[i].wasJustHit && (
				(enemies[i].tilePos.x == newPlayerTile.x - 1 && enemies[i].tilePos.y == newPlayerTile.y && playerFrame == 2) || // enemy to left of player
				(enemies[i].tilePos.x == newPlayerTile.x + 1 && enemies[i].tilePos.y == newPlayerTile.y && playerFrame == 3) || // enemy to right of player
				(enemies[i].tilePos.x == newPlayerTile.x  && enemies[i].tilePos.y == newPlayerTile.y - 1 && playerFrame == 1) || // enemy to top of player
				(enemies[i].tilePos.x == newPlayerTile.x && enemies[i].tilePos.y == newPlayerTile.y + 1 && playerFrame == 0) // enemy to bottom of player
				)){
				attackPlayerAgainstEnemy(i);
				playerDidAttack = TRUE;
			}
		}
		if(!playerDidAttack) strcpy(logStr, "HIT NOTHING");
	}
}

static void changeTurnFromPlayer(){
	currentTurn++;
	if(mapData[newPlayerTile.y][newPlayerTile.x] == TILE_STAIRWELL){
		levelTransitionClock = LEVEL_TRANSITION_LIMIT;
	} else {
		if(playerCanMove){
		  updateVisibilityWithTracking(newPlayerTile.x, newPlayerTile.y);
		  renderVisibleTilesOnly();
			updateCamera();
			SPR_setPosition(player.image, player.pos.x - cameraX, player.pos.y - cameraY);
		}
		SPR_setFrame(player.image, playerFrame);
		checkPlayerAttackCollision(TRUE);
		updateEnemies(FALSE);
		checkPlayerAttackCollision(FALSE);
		updateEnemies(TRUE);
		updateUi();
		player.lastPos.x = player.pos.x;
		player.lastPos.y = player.pos.y;
	}
}

static void forcePlayerPosition(){
	newPlayerTile.x = player.pos.x >> 4;
	newPlayerTile.y = player.pos.y >> 4;
}

void movePlayer(){
	forcePlayerPosition();
	playerCanMove = FALSE;
	if(ctrl.left){
		newPlayerTile.x--;
		playerFrame = 2;
	} else if(ctrl.right){
		newPlayerTile.x++;
		playerFrame = 3;
	} else if(ctrl.up){
		newPlayerTile.y--;
		playerFrame = 1;
	} else if(ctrl.down){
		newPlayerTile.y++;
		playerFrame = 0;
	} if((mapData[newPlayerTile.y][newPlayerTile.x] == TILE_FLOOR || mapData[newPlayerTile.y][newPlayerTile.x] == TILE_STAIRWELL) &&
		(mapData[newPlayerTile.y - 1][newPlayerTile.x] == TILE_FLOOR || mapData[newPlayerTile.y - 1][newPlayerTile.x] == TILE_STAIRWELL))
		playerCanMove = TRUE;
	for(int i = 0; i < ENEMY_COUNT; i++) {
		if(enemies[i].tilePos.x == newPlayerTile.x && enemies[i].tilePos.y == newPlayerTile.y && enemies[i].hp > 0)
			playerCanMove = FALSE;
	}
	if(playerCanMove){
		player.pos.x = newPlayerTile.x << 4;
		player.pos.y = newPlayerTile.y << 4;
		player.tilePos.x = newPlayerTile.x;
		player.tilePos.y = newPlayerTile.y;
	}
	changeTurnFromPlayer();
}

static void attackPlayer(){
	forcePlayerPosition();
	playerDidAttack = FALSE;
	playerTriedAttack = TRUE;
	changeTurnFromPlayer();
}

static void healPlayer(){
	forcePlayerPosition();
	player.justHealed = TRUE;
	player.hp += 4;
	if(player.hp > player.maxHp) player.hp = player.maxHp;
	changeTurnFromPlayer();
}

static void changePlayerItem(){
	// forcePlayerPosition();
	player.currentItem++;
	if(player.currentItem > 3) player.currentItem = 0;
	updateUi();
	// changeTurnFromPlayer();
	// VDP_clearText(8, 3, 4);
}

void updatePlayer(){
	if(!isTurning && turnClock == 0 && (ctrl.left || ctrl.right || ctrl.up || ctrl.down || ctrl.a || ctrl.b)){
		player.justHealed = FALSE;
		playerTriedAttack = FALSE;
		if((ctrl.left || ctrl.right || ctrl.up || ctrl.down)) movePlayer();
		else if(ctrl.a){
			if(player.currentItem < 2)
				attackPlayer();
			else
				healPlayer();
		} else if(ctrl.b){
			changePlayerItem();
		}
		turnClock = 12;
	}
	if(turnClock > 0) turnClock--;
}

void killPlayer(){
	// SYS_hardReset();
	player.hp = 0;
	// strcpy(logStr, "KILLED PLAYER");
}