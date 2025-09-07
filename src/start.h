bool pressedButtonToStart;
u16 startClock;

void loadStart(){
  VDP_loadTileSet(startFade.tileset, TILE_USER_INDEX, DMA);

  VDP_fillTileMapRect(BG_B, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, TILE_USER_INDEX), 0, 0, 40, 28);
	VDP_drawImageEx(BG_B, &startBg, TILE_ATTR_FULL(PAL1, 1, 0, 0, TILE_USER_INDEX + 1), 2, 13, 0, 1);

	VDP_drawImageEx(BG_B, &startBg2, TILE_ATTR_FULL(PAL1, 1, 0, 0, TILE_USER_INDEX + 768), 15, 10, 0, 1);

	VDP_drawImageEx(BG_A, &startLogo, TILE_ATTR_FULL(PAL0, 1, 0, 0, TILE_USER_INDEX + 960), 10, 2, 0, 1);

	// VDP_drawText("BUMP FOR COMBAT", 13, 17);
	VDP_drawText("a:USE ITEM", 13, 18);
	VDP_drawText("b:PICK UP ITEM", 13, 19);
	VDP_drawText("c:CHANGE ITEM", 13, 20);
	VDP_drawText("press any button", 12, 22);

	VDP_drawText("9.2025", 17, 26);
}

void updateStart(){
	if(pressedButtonToStart) loadGame();
	else if(ctrl.start || ctrl.a || ctrl.b || ctrl.c) pressedButtonToStart = TRUE;
	startClock++;
}