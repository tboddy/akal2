static void spawnEnemy(u8 i, s16 spawnX, s16 spawnY){
	enemies[i].seen = FALSE;
	enemies[i].pos.x = spawnX;
	enemies[i].pos.y = spawnY;
	enemies[i].tilePos.x = spawnX / LOGICAL_TILE_SIZE;
	enemies[i].tilePos.y = spawnY / LOGICAL_TILE_SIZE;
	enemies[i].image = SPR_addSprite(&fairySprite, enemies[i].pos.x - cameraX, enemies[i].pos.y - cameraY, TILE_ATTR(PAL0, 0, 0, 0));
	SPR_setVisibility(enemies[i].image, HIDDEN);
}

void spawnEnemies(){
	enemyCount = 0;
	for(int i = 0; i < ENEMY_COUNT; i++) {
		spawnEnemy(i, 64, 64);
		u16 enemyX, enemyY;
		bool validPosition = FALSE;
		int attempts = 0;
		while(!validPosition && attempts < 100) {
			enemyX = random() % MAP_WIDTH;
			enemyY = random() % MAP_HEIGHT;
			if(enemyX < MAP_WIDTH && enemyY < MAP_HEIGHT) {
				u8 tileType = mapData[enemyY][enemyX];
				if(tileType == TILE_FLOOR) {
					bool hasWallAbove = (enemyY > 0 && mapData[enemyY-1][enemyX] == TILE_WALL);
					if(!hasWallAbove) {
						u16 playerTileX = player.pos.x / LOGICAL_TILE_SIZE;
						u16 playerTileY = player.pos.y / LOGICAL_TILE_SIZE;
						s16 distanceX = abs(enemyX - playerTileX);
						s16 distanceY = abs(enemyY - playerTileY);
						if(distanceX > 3 || distanceY > 3) {
							validPosition = TRUE;
						}
					}
				}
			}
			attempts++;
		}
		if(validPosition) {
			s16 enemyPixelX = enemyX * LOGICAL_TILE_SIZE;
			s16 enemyPixelY = enemyY * LOGICAL_TILE_SIZE;
			spawnEnemy(i, enemyPixelX, enemyPixelY);
		}
	}
}

static void checkEnemyVisibility(u8 i){
	if(enemies[i].tilePos.x < MAP_WIDTH && enemies[i].tilePos.y < MAP_HEIGHT) {
		if(visibilityMap[enemies[i].tilePos.y][enemies[i].tilePos.x] != 0) {
			SPR_setVisibility(enemies[i].image, VISIBLE);
			enemies[i].seen = TRUE;
		}
	}
}

static void pathfindEnemy(u8 i){
	u16 newGridX = enemies[i].tilePos.x;
	u16 newGridY = enemies[i].tilePos.y;
	s16 dx = player.tilePos.x - enemies[i].tilePos.x;
	s16 dy = player.tilePos.y - enemies[i].tilePos.y;
	s16 absDx = (dx < 0) ? -dx : dx;
	s16 absDy = (dy < 0) ? -dy : dy;
	bool isAdjacent = ((absDx == 1 && absDy == 0) || (absDx == 0 && absDy == 1));
	bool foundPath = FALSE;
	if(!isAdjacent) {
		if(absDx > absDy) {
			if(dx > 0 && isWalkable(enemies[i].tilePos.x + 1, enemies[i].tilePos.y)) {
				newGridX = enemies[i].tilePos.x + 1;
				enemies[i].lastDirection = 3; // Right
				foundPath = TRUE;
			} else if(dx < 0 && isWalkable(enemies[i].tilePos.x - 1, enemies[i].tilePos.y)) {
				newGridX = enemies[i].tilePos.x - 1;
				enemies[i].lastDirection = 2; // Left
				foundPath = TRUE;
			}
		} else {
			if(dy > 0 && isWalkable(enemies[i].tilePos.x, enemies[i].tilePos.y + 1)) {
				newGridY = enemies[i].tilePos.y + 1;
				enemies[i].lastDirection = 0; // Down
				foundPath = TRUE;
			} else if(dy < 0 && isWalkable(enemies[i].tilePos.x, enemies[i].tilePos.y - 1)) {
				newGridY = enemies[i].tilePos.y - 1;
				enemies[i].lastDirection = 1; // Up
				foundPath = TRUE;
			}
		}
		if(!foundPath) {
			if(absDx > absDy) {
				if(dy > 0 && isWalkable(enemies[i].tilePos.x, enemies[i].tilePos.y + 1)) {
					newGridY = enemies[i].tilePos.y + 1;
					enemies[i].lastDirection = 0; // Down
					foundPath = TRUE;
				} else if(dy < 0 && isWalkable(enemies[i].tilePos.x, enemies[i].tilePos.y - 1)) {
					newGridY = enemies[i].tilePos.y - 1;
					enemies[i].lastDirection = 1; // Up
					foundPath = TRUE;
				}
			} else {
				if(dx > 0 && isWalkable(enemies[i].tilePos.x + 1, enemies[i].tilePos.y)) {
					newGridX = enemies[i].tilePos.x + 1;
					enemies[i].lastDirection = 3; // Right
					foundPath = TRUE;
				} else if(dx < 0 && isWalkable(enemies[i].tilePos.x - 1, enemies[i].tilePos.y)) {
					newGridX = enemies[i].tilePos.x - 1;
					enemies[i].lastDirection = 2; // Left
					foundPath = TRUE;
				}
			}
		}
	}
	if((newGridX != enemies[i].tilePos.x || newGridY != enemies[i].tilePos.y) && 
	   (newGridX != player.tilePos.x || newGridY != player.tilePos.y) &&
	   isWalkable(newGridX, newGridY)) {
		enemies[i].pos.x = newGridX * LOGICAL_TILE_SIZE;
		enemies[i].pos.y = newGridY * LOGICAL_TILE_SIZE;
	}
}

void updateEnemies(){
	for(int i = 0; i < ENEMY_COUNT; i++) {
		pathfindEnemy(i);
		if(!enemies[i].seen) checkEnemyVisibility(i);
		enemies[i].tilePos.x = enemies[i].pos.x / LOGICAL_TILE_SIZE;
		enemies[i].tilePos.y = enemies[i].pos.y / LOGICAL_TILE_SIZE;
		SPR_setPosition(enemies[i].image, enemies[i].pos.x - cameraX, enemies[i].pos.y - cameraY);
	}
}