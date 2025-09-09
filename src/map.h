u16 roomCount;

typedef struct {
	u16 x, y;
	u16 width, height; 
} Room;
Room rooms[MAX_ROOMS];

static bool roomsOverlap(Room* r1, Room* r2) {
	return !(r1->x + r1->width < r2->x || r1->x > r2->x + r2->width ||
		r1->y + r1->height < r2->y || r1->y > r2->y + r2->height);
}

static void createRoom(Room* room) {
	for(int y = room->y; y < room->y + room->height; y++) {
		for(int x = room->x; x < room->x + room->width; x++) {
			if(x < MAP_WIDTH && y < MAP_HEIGHT) {
				mapData[y][x] = TILE_FLOOR;
			}
		}
	}
}

static void createHorizontalCorridor(int x1, int x2, int y) {
	int startX = (x1 < x2) ? x1 : x2;
	int endX = (x1 < x2) ? x2 : x1;
	// Clamp to map boundaries (leave 1-tile border)
	if(startX < 1) startX = 1;
	if(endX >= MAP_WIDTH - 1) endX = MAP_WIDTH - 2;
	if(y < 1 || y >= MAP_HEIGHT - 1) return; // Invalid Y position
	for(int x = startX; x <= endX; x++) {
		mapData[y][x] = TILE_FLOOR;
		if(y+1 < MAP_HEIGHT - 1) {
			mapData[y+1][x] = TILE_FLOOR; // Floor (second row)
		}
	}
}

static void createVerticalCorridor(int x, int y1, int y2) {
	int startY = (y1 < y2) ? y1 : y2;
	int endY = (y1 < y2) ? y2 : y1;
	if(startY < 1) startY = 1;
	if(endY >= MAP_HEIGHT - 1) endY = MAP_HEIGHT - 2;
	if(x < 1 || x >= MAP_WIDTH - 1) return; // Invalid X position
	for(int y = startY; y <= endY; y++) {
		mapData[y][x] = TILE_FLOOR;
		if(x+1 < MAP_WIDTH - 1) {
			mapData[y][x+1] = TILE_FLOOR; // Floor (second column)
		}
	}
}

static void generateDungeon() {
	for(int y = 0; y < MAP_HEIGHT; y++) {
		for(int x = 0; x < MAP_WIDTH; x++) {
			mapData[y][x] = TILE_WALL;
		}
	}
	for(int x = 0; x < MAP_WIDTH; x++) {
		mapData[0][x] = TILE_WALL;  // Top border wall
		mapData[MAP_HEIGHT-1][x] = TILE_WALL;  // Bottom border wall
	}
	for(int y = 0; y < MAP_HEIGHT; y++) {
		mapData[y][0] = TILE_WALL;  // Left border wall
		mapData[y][MAP_WIDTH-1] = TILE_WALL;  // Right border wall
	}
	roomCount = 0;
	for(int attempts = 0; attempts < 100 && roomCount < MAX_ROOMS; attempts++) {
		Room newRoom;
		newRoom.width = MIN_ROOM_SIZE + simpleRandom(MAX_ROOM_SIZE - MIN_ROOM_SIZE + 1);
		newRoom.height = MIN_ROOM_SIZE + simpleRandom(MAX_ROOM_SIZE - MIN_ROOM_SIZE + 1);
		newRoom.x = 1 + simpleRandom(MAP_WIDTH - newRoom.width - 2);
		newRoom.y = 1 + simpleRandom(MAP_HEIGHT - newRoom.height - 2);
		bool canPlace = TRUE;
		for(int i = 0; i < roomCount; i++) {
			if(roomsOverlap(&newRoom, &rooms[i])) {
				canPlace = FALSE;
				break;
			}
		}
		if(canPlace) {
			rooms[roomCount] = newRoom;
			createRoom(&newRoom);
			roomCount++;
		}
	}
	for(int i = 1; i < roomCount; i++) {
		int prevCenterX = rooms[i-1].x + rooms[i-1].width / 2;
		int prevCenterY = rooms[i-1].y + rooms[i-1].height / 2;
		int currCenterX = rooms[i].x + rooms[i].width / 2;
		int currCenterY = rooms[i].y + rooms[i].height / 2;
		createHorizontalCorridor(prevCenterX, currCenterX, prevCenterY);
		createVerticalCorridor(currCenterX, prevCenterY, currCenterY);
	}
	for(int i = 0; i < roomCount; i++) {
		if(simpleRandom(100) < 50) {
			int targetRoom = simpleRandom(roomCount);
			if(targetRoom != i) {
				int room1CenterX = rooms[i].x + rooms[i].width / 2;
				int room1CenterY = rooms[i].y + rooms[i].height / 2;
				int room2CenterX = rooms[targetRoom].x + rooms[targetRoom].width / 2;
				int room2CenterY = rooms[targetRoom].y + rooms[targetRoom].height / 2;
				if(simpleRandom(100) < 70) {
					createHorizontalCorridor(room1CenterX, room2CenterX, room1CenterY);
					createVerticalCorridor(room2CenterX, room1CenterY, room2CenterY);
				} else {
					if(simpleRandom(100) < 50) {
						createHorizontalCorridor(room1CenterX, room2CenterX, room1CenterY);
					} else {
						createVerticalCorridor(room1CenterX, room1CenterY, room2CenterY);
					}
				}
			}
		}
	}
	for(int i = 0; i < 8; i++) {
		if(roomCount > 0) {
			int roomIndex = simpleRandom(roomCount);
			int roomCenterX = rooms[roomIndex].x + rooms[roomIndex].width / 2;
			int roomCenterY = rooms[roomIndex].y + rooms[roomIndex].height / 2;
			int corridorLength = 2 + simpleRandom(4); // 2-5 tiles long (shorter for safety)
			int direction = simpleRandom(4); // 0=up, 1=down, 2=left, 3=right
			bool canCreate = TRUE;
			switch(direction) {
				case 0: // Up
					if(roomCenterY - corridorLength < 1) canCreate = FALSE;
					break;
				case 1: // Down
					if(roomCenterY + corridorLength >= MAP_HEIGHT - 1) canCreate = FALSE;
					break;
				case 2: // Left
					if(roomCenterX - corridorLength < 1) canCreate = FALSE;
					break;
				case 3: // Right
					if(roomCenterX + corridorLength >= MAP_WIDTH - 1) canCreate = FALSE;
					break;
			}
			if(canCreate) {
				switch(direction) {
					case 0: // Up
						createVerticalCorridor(roomCenterX, roomCenterY - corridorLength, roomCenterY);
						break;
					case 1: // Down
						createVerticalCorridor(roomCenterX, roomCenterY, roomCenterY + corridorLength);
						break;
					case 2: // Left
						createHorizontalCorridor(roomCenterX - corridorLength, roomCenterX, roomCenterY);
						break;
					case 3: // Right
						createHorizontalCorridor(roomCenterX, roomCenterX + corridorLength, roomCenterY);
						break;
				}
			}
		}
	}
}

static void initVisibility(){
	for(int y = 0; y < MAP_HEIGHT; y++) {
		for(int x = 0; x < MAP_WIDTH; x++) {
			visibilityMap[y][x] = 0;  
			previousVisibilityMap[y][x] = 0;
			visibilityChanged[y][x] = FALSE;
		}
	}
}

void renderTile(u16 x, u16 y) {
	if(visibilityMap[y][x] == 0) {
		VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 1, FALSE, FALSE, TILE_USER_INDEX + 0), x*2, y*2);
		VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 1, FALSE, FALSE, TILE_USER_INDEX + 1), x*2 + 1, y*2);
		VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 1, FALSE, FALSE, TILE_USER_INDEX + 14), x*2, y*2 + 1);
		VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 1, FALSE, FALSE, TILE_USER_INDEX + 15), x*2 + 1, y*2 + 1);
	} else if(visibilityMap[y][x] == 1) {
		if(mapData[y][x] == TILE_FLOOR) {
			bool hasWallAbove = (y > 0 && mapData[y-1][x] == TILE_WALL);
			if(hasWallAbove) {
				VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 0, FALSE, FALSE, TILE_USER_INDEX + 4), x*2, y*2);
				VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 0, FALSE, FALSE, TILE_USER_INDEX + 5), x*2 + 1, y*2);
				VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 0, FALSE, FALSE, TILE_USER_INDEX + 18), x*2, y*2 + 1);
				VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 0, FALSE, FALSE, TILE_USER_INDEX + 19), x*2 + 1, y*2 + 1);
			} else {
				VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 0, FALSE, FALSE, TILE_USER_INDEX + 2), x*2, y*2);
				VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 0, FALSE, FALSE, TILE_USER_INDEX + 3), x*2 + 1, y*2);
				VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 0, FALSE, FALSE, TILE_USER_INDEX + 16), x*2, y*2 + 1);
				VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 0, FALSE, FALSE, TILE_USER_INDEX + 17), x*2 + 1, y*2 + 1);
			}
		} else if(mapData[y][x] == TILE_STAIRWELL) {
			VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 0, FALSE, FALSE, TILE_USER_INDEX + 6), x*2, y*2);
			VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 0, FALSE, FALSE, TILE_USER_INDEX + 7), x*2 + 1, y*2);
			VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 0, FALSE, FALSE, TILE_USER_INDEX + 20), x*2, y*2 + 1);
			VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 0, FALSE, FALSE, TILE_USER_INDEX + 21), x*2 + 1, y*2 + 1);
		} else {
			VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 0, FALSE, FALSE, TILE_USER_INDEX + 0), x*2, y*2);
			VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 0, FALSE, FALSE, TILE_USER_INDEX + 1), x*2 + 1, y*2);
			VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 0, FALSE, FALSE, TILE_USER_INDEX + 14), x*2, y*2 + 1);
			VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 0, FALSE, FALSE, TILE_USER_INDEX + 15), x*2 + 1, y*2 + 1);
		}
	} else if(visibilityMap[y][x] == 3) {
		if(mapData[y][x] == TILE_FLOOR) {
			bool hasWallAbove = (y > 0 && mapData[y-1][x] == TILE_WALL);
			if(hasWallAbove) {
				VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 0, FALSE, FALSE, TILE_USER_INDEX + 10), x*2, y*2);
				VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 0, FALSE, FALSE, TILE_USER_INDEX + 11), x*2 + 1, y*2);
				VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 0, FALSE, FALSE, TILE_USER_INDEX + 24), x*2, y*2 + 1);
				VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 0, FALSE, FALSE, TILE_USER_INDEX + 25), x*2 + 1, y*2 + 1);
			} else {
				VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 0, FALSE, FALSE, TILE_USER_INDEX + 8), x*2, y*2);
				VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 0, FALSE, FALSE, TILE_USER_INDEX + 9), x*2 + 1, y*2);
				VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 0, FALSE, FALSE, TILE_USER_INDEX + 22), x*2, y*2 + 1);
				VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 0, FALSE, FALSE, TILE_USER_INDEX + 23), x*2 + 1, y*2 + 1);
			}
		} else if(mapData[y][x] == TILE_STAIRWELL) {
			VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 0, FALSE, FALSE, TILE_USER_INDEX + 12), x*2, y*2);
			VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 0, FALSE, FALSE, TILE_USER_INDEX + 13), x*2 + 1, y*2);
			VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 0, FALSE, FALSE, TILE_USER_INDEX + 26), x*2, y*2 + 1);
			VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 0, FALSE, FALSE, TILE_USER_INDEX + 27), x*2 + 1, y*2 + 1);
		} else {
			VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 0, FALSE, FALSE, TILE_USER_INDEX + 0), x*2, y*2);
			VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 0, FALSE, FALSE, TILE_USER_INDEX + 1), x*2 + 1, y*2);
			VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 0, FALSE, FALSE, TILE_USER_INDEX + 14), x*2, y*2 + 1);
			VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 0, FALSE, FALSE, TILE_USER_INDEX + 15), x*2 + 1, y*2 + 1);
		}
	} else {
		if(mapData[y][x] == TILE_FLOOR) {
			bool hasWallAbove = (y > 0 && mapData[y-1][x] == TILE_WALL);
			if(hasWallAbove) {
				VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 0, FALSE, FALSE, TILE_USER_INDEX + 4), x*2, y*2);
				VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 0, FALSE, FALSE, TILE_USER_INDEX + 5), x*2 + 1, y*2);
				VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 0, FALSE, FALSE, TILE_USER_INDEX + 18), x*2, y*2 + 1);
				VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 0, FALSE, FALSE, TILE_USER_INDEX + 19), x*2 + 1, y*2 + 1);
			} else {
				VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 0, FALSE, FALSE, TILE_USER_INDEX + 2), x*2, y*2);
				VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 0, FALSE, FALSE, TILE_USER_INDEX + 3), x*2 + 1, y*2);
				VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 0, FALSE, FALSE, TILE_USER_INDEX + 16), x*2, y*2 + 1);
				VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 0, FALSE, FALSE, TILE_USER_INDEX + 17), x*2 + 1, y*2 + 1);
			}
		} else if(mapData[y][x] == TILE_STAIRWELL) {
			VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 0, FALSE, FALSE, TILE_USER_INDEX + 6), x*2, y*2);
			VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 0, FALSE, FALSE, TILE_USER_INDEX + 7), x*2 + 1, y*2);
			VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 0, FALSE, FALSE, TILE_USER_INDEX + 20), x*2, y*2 + 1);
			VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 0, FALSE, FALSE, TILE_USER_INDEX + 21), x*2 + 1, y*2 + 1);
		} else {
			VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 0, FALSE, FALSE, TILE_USER_INDEX + 0), x*2, y*2);
			VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 0, FALSE, FALSE, TILE_USER_INDEX + 1), x*2 + 1, y*2);
			VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 0, FALSE, FALSE, TILE_USER_INDEX + 14), x*2, y*2 + 1);
			VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 0, FALSE, FALSE, TILE_USER_INDEX + 15), x*2 + 1, y*2 + 1);
		}
	}
}

static void renderMap() {
	for(int y = 0; y < MAP_HEIGHT; y++) {
		for(int x = 0; x < MAP_WIDTH; x++) {
			renderTile(x, y);
		}
	}
}

static void findRoomBasedSpawnPosition(s16* spawnX, s16* spawnY) {
	if(roomCount == 0) {
		*spawnX = MAP_WIDTH * LOGICAL_TILE_SIZE / 2;
		*spawnY = MAP_HEIGHT * LOGICAL_TILE_SIZE / 2;
		return;
	}
	int spawnRoomIndex = simpleRandom(roomCount);
	Room* spawnRoom = &rooms[spawnRoomIndex];
	int roomCenterX = spawnRoom->x + (spawnRoom->width / 2);
	int roomCenterY = spawnRoom->y + (spawnRoom->height / 2);
	*spawnX = roomCenterX * LOGICAL_TILE_SIZE;
	*spawnY = roomCenterY * LOGICAL_TILE_SIZE;
}

static void placeStairwellAfterPlayerSpawn(s16 playerSpawnX, s16 playerSpawnY) {
	if(roomCount == 0) return;
	u16 playerGridX = playerSpawnX / LOGICAL_TILE_SIZE;
	u16 playerGridY = playerSpawnY / LOGICAL_TILE_SIZE;
	int farthestRoomIndex = 0;
	int maxDistance = 0;
	for(int i = 0; i < roomCount; i++) {
		Room* room = &rooms[i];
		int roomCenterX = room->x + (room->width / 2);
		int roomCenterY = room->y + (room->height / 2);
		int dx = roomCenterX - playerGridX;
		int dy = roomCenterY - playerGridY;
		int distance = dx * dx + dy * dy; // Square distance for efficiency
		if(distance > maxDistance) {
			maxDistance = distance;
			farthestRoomIndex = i;
		}
	}
	Room* stairwellRoom = &rooms[farthestRoomIndex];
	int stairwellX = stairwellRoom->x + (stairwellRoom->width / 2);
	int stairwellY = stairwellRoom->y + (stairwellRoom->height / 2);
	if(stairwellX >= 1 && stairwellX < MAP_WIDTH - 1 && 
	   stairwellY >= 1 && stairwellY < MAP_HEIGHT - 1) {
		mapData[stairwellY][stairwellX] = TILE_STAIRWELL;
	}
}

static void clearCurrentVisibility(int minX, int maxX, int minY, int maxY) {
	for(int y = minY; y <= maxY; y++) {
		for(int x = minX; x <= maxX; x++) {
			if(visibilityMap[y][x] == 2) {  // Currently visible
				visibilityMap[y][x] = 1;    // Now just seen (permanent memory)
			}
		}
	}
}

static void setVisibilityInRadius(int playerX, int playerY, int minX, int maxX, int minY, int maxY, int maxDistance, bool trackChanges) {
	for(int y = minY; y <= maxY; y++) {
		for(int x = minX; x <= maxX; x++) {
			int dx = x - playerX;
			int dy = y - playerY;
			int distance = dx * dx + dy * dy;  // Squared distance for efficiency
			if(distance <= maxDistance) {
				u8 oldVisibility = visibilityMap[y][x];
				visibilityMap[y][x] = 2;  // Currently visible
				if(trackChanges && oldVisibility != 2) {
					visibilityChanged[y][x] = TRUE;
				}
			}
		}
	}
}

static void calculateShadowEffects(int minX, int maxX, int minY, int maxY, bool trackChanges) {
	for(int y = minY; y <= maxY; y++) {
		for(int x = minX; x <= maxX; x++) {
			if(visibilityMap[y][x] == 0) {
				bool shouldBeShadow = false;
				for(int dy = -1; dy <= 1; dy++) {
					for(int dx = -1; dx <= 1; dx++) {
						if(dx == 0 && dy == 0) continue; // Skip current tile
						int nx = x + dx;
						int ny = y + dy;
						if(nx >= 0 && nx < MAP_WIDTH && ny >= 0 && ny < MAP_HEIGHT) {
							if(visibilityMap[ny][nx] == 2) {
								shouldBeShadow = true;
								break;
							}
						}
					}
					if(shouldBeShadow) break;
				}
				if(shouldBeShadow && (mapData[y][x] == TILE_FLOOR || mapData[y][x] == TILE_STAIRWELL)) {
					u8 oldVisibility = visibilityMap[y][x];
					visibilityMap[y][x] = 3;  // Shadow zone (temporary)
					if(trackChanges && oldVisibility != 3) {
						visibilityChanged[y][x] = TRUE;
					}
				}
			}
		}
	}
}

static void updateVisibility(u16 playerX, u16 playerY) {
	clearCurrentVisibility(0, MAP_WIDTH - 1, 0, MAP_HEIGHT - 1);
	setVisibilityInRadius(playerX, playerY, 0, MAP_WIDTH - 1, 0, MAP_HEIGHT - 1, 16, false);
	calculateShadowEffects(0, MAP_WIDTH - 1, 0, MAP_HEIGHT - 1, false);
}

void updateVisibilityWithTracking(s16 playerX, s16 playerY) {
	const int VISIBILITY_RADIUS = 4;
	const int SHADOW_BUFFER = 1;
	const int MAX_PROCESS_RADIUS = VISIBILITY_RADIUS + SHADOW_BUFFER + 1; // 6 tiles total
	
	int minX = playerX - MAX_PROCESS_RADIUS;
	int maxX = playerX + MAX_PROCESS_RADIUS;
	int minY = playerY - MAX_PROCESS_RADIUS;
	int maxY = playerY + MAX_PROCESS_RADIUS;
	
	if(minX < 0) minX = 0;
	if(maxX >= MAP_WIDTH) maxX = MAP_WIDTH - 1;
	if(minY < 0) minY = 0;
	if(maxY >= MAP_HEIGHT) maxY = MAP_HEIGHT - 1;
	
	for(int y = minY; y <= maxY; y++)
		for(int x = minX; x <= maxX; x++)
			visibilityChanged[y][x] = FALSE;
	
	clearCurrentVisibility(minX, maxX, minY, maxY);
	setVisibilityInRadius(playerX, playerY, minX, maxX, minY, maxY, 16, true);
	calculateShadowEffects(minX, maxX, minY, maxY, true);
	
	for(int y = minY; y <= maxY; y++)
		for(int x = minX; x <= maxX; x++)
			previousVisibilityMap[y][x] = visibilityMap[y][x];
}

void renderVisibleTilesOnly() {
	for(int y = 0; y < MAP_HEIGHT; y++)
		for(int x = 0; x < MAP_WIDTH; x++)
			if(visibilityChanged[y][x])
				renderTile(x, y);
}

void loadLevel(){
	currentTurn = 0;
	generateDungeon();
  initVisibility();
  renderMap();
	s16 spawnX, spawnY;
  findRoomBasedSpawnPosition(&spawnX, &spawnY);
  placeStairwellAfterPlayerSpawn(spawnX, spawnY);
  player.pos.x = spawnX;
  player.pos.y = spawnY;
  u16 spawnGridX = spawnX / LOGICAL_TILE_SIZE;
  u16 spawnGridY = spawnY / LOGICAL_TILE_SIZE;
  updateVisibility(spawnGridX, spawnGridY);
  renderMap();
  movePlayer();
  spawnEnemies();
  // updateUi();
}

void loadMap(){
	currentLevel = 1;
	loadLevel();
}

u16 transitionClock;

void transitionToNextLevel(){
	currentLevel++;
	loadLevel();
	SPR_setVisibility(player.image, VISIBLE);
}