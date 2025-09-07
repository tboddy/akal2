void updateCamera(){
	s16 targetCameraX = player.pos.x - (GAMEPLAY_AREA_WIDTH / 2) - GAMEPLAY_OFFSET_X;
	s16 targetCameraY = player.pos.y - (GAMEPLAY_AREA_HEIGHT / 2) - GAMEPLAY_OFFSET_Y;
	
	s16 mapWidthPixels = MAP_WIDTH * LOGICAL_TILE_SIZE;
	s16 mapHeightPixels = MAP_HEIGHT * LOGICAL_TILE_SIZE;

	if(targetCameraX < -56)
		targetCameraX = -56;
	else if(targetCameraX > mapWidthPixels - GAMEPLAY_AREA_WIDTH - 56)
		targetCameraX = mapWidthPixels - GAMEPLAY_AREA_WIDTH - 56;
	
	if(targetCameraY < -8)
		targetCameraY = -8;
	else if(targetCameraY > mapHeightPixels - GAMEPLAY_AREA_HEIGHT - 8)
		targetCameraY = mapHeightPixels - GAMEPLAY_AREA_HEIGHT - 8;
	
	cameraX = targetCameraX;
	cameraY = targetCameraY;
	
	VDP_setHorizontalScroll(BG_B, -cameraX);
	VDP_setVerticalScroll(BG_B, cameraY);
}