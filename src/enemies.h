#define TYPE_MOONRABBIT 0
#define TYPE_EYE 1

static void checkEnemyVisibility(u8 i){
	if(enemies[i].tilePos.x < MAP_WIDTH && enemies[i].tilePos.y < MAP_HEIGHT) {
		if(visibilityMap[enemies[i].tilePos.y][enemies[i].tilePos.x] != 0 && !enemies[i].seen) {
			SPR_setVisibility(enemies[i].image, VISIBLE);
			enemies[i].seen = TRUE;
		}
	}
}

static void spawnEnemy(u8 i, s16 spawnX, s16 spawnY){
	enemies[i].seen = FALSE;
	enemies[i].pos.x = spawnX;
	enemies[i].pos.y = spawnY;
	enemies[i].lastPos.x = spawnX;
	enemies[i].lastPos.y = spawnY;
	enemies[i].tilePos.x = spawnX / LOGICAL_TILE_SIZE;
	enemies[i].tilePos.y = spawnY / LOGICAL_TILE_SIZE;
	enemies[i].lastDirection = 0; // Initialize direction
	enemies[i].moveCounter = 0; // Initialize move counter


	if(i % 2 == 0){
		// moon rabbit
		enemies[i].type = TYPE_MOONRABBIT;
		enemies[i].hp = 3;
		enemies[i].image = SPR_addSprite(&moonRabbitSprite, enemies[i].pos.x - cameraX, enemies[i].pos.y - cameraY + bounceOffset, TILE_ATTR(PAL0, 0, 0, 0));
	} else {
		// eye
		enemies[i].type = TYPE_EYE;
		enemies[i].image = SPR_addSprite(&eyeSprite, enemies[i].pos.x - cameraX, enemies[i].pos.y - cameraY + bounceOffset, TILE_ATTR(PAL0, 0, 0, 0));
	}

	SPR_setAnim(enemies[i].image, random() % 3);

	SPR_setVisibility(enemies[i].image, HIDDEN);
	checkEnemyVisibility(i);
}

void spawnEnemies(){
	enemyCount = 0;
	for(int i = 0; i < ENEMY_COUNT; i++) {
		u16 enemyX, enemyY;
		bool validPosition = FALSE;
		int attempts = 0;
		
		// Try to find a valid spawn position
		while(!validPosition && attempts < 100) {
			enemyX = random() % MAP_WIDTH;
			enemyY = random() % MAP_HEIGHT;
			
			// Use isWalkable to check if position is valid
			if(isWalkable(enemyX, enemyY)) {
				// Check distance from player (spawn at least 3 tiles away)
				u16 playerTileX = player.pos.x / LOGICAL_TILE_SIZE;
				u16 playerTileY = player.pos.y / LOGICAL_TILE_SIZE;
				s16 distanceX = abs(enemyX - playerTileX);
				s16 distanceY = abs(enemyY - playerTileY);
				if(distanceX > 3 || distanceY > 3) {
					validPosition = TRUE;
				}
			}
			attempts++;
		}
		
		// Spawn enemy at valid position, or use fallback position
		if(validPosition) {
			s16 enemyPixelX = enemyX * LOGICAL_TILE_SIZE;
			s16 enemyPixelY = enemyY * LOGICAL_TILE_SIZE;
			spawnEnemy(i, enemyPixelX, enemyPixelY);
		} else {
			// Fallback: spawn at a safe default position (center of map)
			spawnEnemy(i, (MAP_WIDTH/2) * LOGICAL_TILE_SIZE, (MAP_HEIGHT/2) * LOGICAL_TILE_SIZE);
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
	// Ensure only cardinal movement (only one coordinate can change at a time)
	bool isCardinalMove = ((newGridX != enemies[i].tilePos.x) && (newGridY == enemies[i].tilePos.y)) || 
	                      ((newGridX == enemies[i].tilePos.x) && (newGridY != enemies[i].tilePos.y));
	
	if(isCardinalMove && 
	   (newGridX != player.tilePos.x || newGridY != player.tilePos.y) &&
	   isWalkable(newGridX, newGridY)) {
		enemies[i].pos.x = newGridX * LOGICAL_TILE_SIZE;
		enemies[i].pos.y = newGridY * LOGICAL_TILE_SIZE;
	}
}

static void wanderEnemy(u8 i){
	u16 currentX = enemies[i].tilePos.x;
	u16 currentY = enemies[i].tilePos.y;
	u16 newX = currentX;
	u16 newY = currentY;
	
	// Check available directions (0=down, 1=up, 2=left, 3=right)
	bool canMoveDown = isWalkable(currentX, currentY + 1);
	bool canMoveUp = isWalkable(currentX, currentY - 1);
	bool canMoveLeft = isWalkable(currentX - 1, currentY);
	bool canMoveRight = isWalkable(currentX + 1, currentY);
	
	// Count available directions
	u8 availableDirections = 0;
	if(canMoveDown) availableDirections++;
	if(canMoveUp) availableDirections++;
	if(canMoveLeft) availableDirections++;
	if(canMoveRight) availableDirections++;
	
	// If no directions available, don't move
	if(availableDirections == 0) return;
	
	// If only one direction available, take it (dead end)
	if(availableDirections == 1) {
		if(canMoveDown) {
			newY = currentY + 1;
			enemies[i].lastDirection = 0; // Down
		} else if(canMoveUp) {
			newY = currentY - 1;
			enemies[i].lastDirection = 1; // Up
		} else if(canMoveLeft) {
			newX = currentX - 1;
			enemies[i].lastDirection = 2; // Left
		} else if(canMoveRight) {
			newX = currentX + 1;
			enemies[i].lastDirection = 3; // Right
		}
		enemies[i].moveCounter = 0; // Reset counter when forced to change direction
	} else {
		// Multiple directions available - use wander logic with persistence
		bool shouldChangeDirection = false;
		
		// Check if we should consider changing direction
		// Change direction if moveCounter reaches a threshold (3-6 moves)
		u8 persistenceThreshold = 3 + simpleRandom(4); // 3-6 moves
		if(enemies[i].moveCounter >= persistenceThreshold) {
			shouldChangeDirection = true;
		}
		
		// Try to continue in same direction if we haven't reached threshold
		if(!shouldChangeDirection) {
			bool canContinue = false;
			switch(enemies[i].lastDirection) {
				case 0: // Down
					if(canMoveDown) {
						newY = currentY + 1;
						canContinue = true;
					}
					break;
				case 1: // Up
					if(canMoveUp) {
						newY = currentY - 1;
						canContinue = true;
					}
					break;
				case 2: // Left
					if(canMoveLeft) {
						newX = currentX - 1;
						canContinue = true;
					}
					break;
				case 3: // Right
					if(canMoveRight) {
						newX = currentX + 1;
						canContinue = true;
					}
					break;
			}
			
			// If can continue in same direction, do it and increment counter
			if(canContinue) {
				enemies[i].moveCounter++;
			} else {
				// Can't continue, must change direction
				shouldChangeDirection = true;
			}
		}
		
		// Change direction if needed
		if(shouldChangeDirection) {
			// Create an array of available directions and randomly select one
			u8 availableDirs[4];
			u8 dirCount = 0;
			
			// Build array of available directions
			if(canMoveDown) availableDirs[dirCount++] = 0; // Down
			if(canMoveUp) availableDirs[dirCount++] = 1;   // Up
			if(canMoveLeft) availableDirs[dirCount++] = 2; // Left
			if(canMoveRight) availableDirs[dirCount++] = 3; // Right
			
			// Randomly select one of the available directions
			u8 selectedDir = availableDirs[simpleRandom(dirCount)];
			
			// Apply the selected direction
			switch(selectedDir) {
				case 0: // Down
					newY = currentY + 1;
					enemies[i].lastDirection = 0;
					break;
				case 1: // Up
					newY = currentY - 1;
					enemies[i].lastDirection = 1;
					break;
				case 2: // Left
					newX = currentX - 1;
					enemies[i].lastDirection = 2;
					break;
				case 3: // Right
					newX = currentX + 1;
					enemies[i].lastDirection = 3;
					break;
			}
			enemies[i].moveCounter = 0; // Reset counter when changing direction
		}
	}
	
	// Move the enemy if the new position is valid and not occupied by player
	// Ensure only cardinal movement (only one coordinate can change at a time)
	bool isCardinalMove = ((newX != currentX) && (newY == currentY)) || 
	                      ((newX == currentX) && (newY != currentY));
	
	if(isCardinalMove && 
	   (newX != player.tilePos.x || newY != player.tilePos.y) &&
	   isWalkable(newX, newY)) {
		enemies[i].pos.x = newX * LOGICAL_TILE_SIZE;
		enemies[i].pos.y = newY * LOGICAL_TILE_SIZE;
	}
}

static void lookEnemy(u8 i){
	if(enemies[i].pos.x < enemies[i].lastPos.x)
		SPR_setFrame(enemies[i].image, 2);
	else if(enemies[i].pos.x > enemies[i].lastPos.x)
		SPR_setFrame(enemies[i].image, 3);
	else if(enemies[i].pos.y < enemies[i].lastPos.y)
		SPR_setFrame(enemies[i].image, 1);
	else if(enemies[i].pos.y > enemies[i].lastPos.y)
		SPR_setFrame(enemies[i].image, 0);
	enemies[i].lastPos.x = enemies[i].pos.x;
	enemies[i].lastPos.y = enemies[i].pos.y;
}

static void updateEnemy(u8 i){
	if(enemies[i].type == TYPE_MOONRABBIT){
		if(currentTurn % 2 == 0) pathfindEnemy(i);
		lookEnemy(i);
	} else if(enemies[i].type == TYPE_EYE){
		wanderEnemy(i);
		lookEnemy(i);
	}
}

void updateEnemies(){
	for(int i = 0; i < ENEMY_COUNT; i++) {
		if(enemies[i].seen) updateEnemy(i);
		checkEnemyVisibility(i);
		enemies[i].tilePos.x = enemies[i].pos.x / LOGICAL_TILE_SIZE;
		enemies[i].tilePos.y = enemies[i].pos.y / LOGICAL_TILE_SIZE;
		SPR_setPosition(enemies[i].image, enemies[i].pos.x - cameraX, enemies[i].pos.y - cameraY + (i % 4 < 2 ? bounceOffset : -bounceOffset));
	}
}

void bounceEnemies(){
	if(clock % 32 == 0){
		bounceOffset = bounceOffset == 1 ? -1 : 1;
		for(int i = 0; i < ENEMY_COUNT; i++) {
			SPR_setPosition(enemies[i].image, enemies[i].pos.x - cameraX, enemies[i].pos.y - cameraY + (i % 4 < 2 ? bounceOffset : -bounceOffset));
		}
	}
}

void clearEnemies(){
	for(int i = 0; i < ENEMY_COUNT; i++) {
		SPR_releaseSprite(enemies[i].image);
	}
}