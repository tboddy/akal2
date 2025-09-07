u16 moveClock;
bool isMoving;

void loadPlayer(){
	player.image = SPR_addSprite(&playerSprite, 0, 0, TILE_ATTR(PAL0, 0, 0, 0));
	player.hp = 10;
	player.maxHp = 10;
	player.atk = 0;
	player.def = 0;
	player.wpn = 2;
	player.arm = 0;
	moveClock = 0;
}

Vect2D_s16 newPlayerTile;
bool playerCanMove;
u8 playerFrame;
void movePlayer(){
	isMoving = TRUE;
	newPlayerTile.x = player.pos.x >> 4;
	newPlayerTile.y = player.pos.y >> 4;
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
		if(enemies[i].tilePos.x == newPlayerTile.x && enemies[i].tilePos.y == newPlayerTile.y)
			playerCanMove = FALSE;
	}
	if(playerCanMove){
		player.pos.x = newPlayerTile.x << 4;
		player.pos.y = newPlayerTile.y << 4;
		player.tilePos.x = newPlayerTile.x;
		player.tilePos.y = newPlayerTile.y;
	}
	currentTurn++;
	if(mapData[newPlayerTile.y][newPlayerTile.x] == TILE_STAIRWELL){
		levelTransitionClock = LEVEL_TRANSITION_LIMIT;
		isMoving = FALSE;
	} else {
		if(playerCanMove){
		  updateVisibilityWithTracking(newPlayerTile.x, newPlayerTile.y);
		  renderVisibleTilesOnly();
			updateCamera();
			SPR_setPosition(player.image, player.pos.x - cameraX, player.pos.y - cameraY);
		}
		SPR_setFrame(player.image, playerFrame);
		updateEnemies();
		updateUi();
		isMoving = FALSE;
	}
}


void updatePlayer(){
	if(moveClock == 0 && (ctrl.left || ctrl.right || ctrl.up || ctrl.down) && !isMoving){
		movePlayer();
		moveClock = 8;
	}
	if(moveClock > 0) moveClock--;
}