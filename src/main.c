#include <genesis.h>
#include <resources.h>

#include "globals.h"
#include "start.h"
#include "items.h"
#include "enemies.h"
#include "map.h"
#include "player.h"
#include "camera.h"
#include "ui.h"

void loadInternals() {
	JOY_init();
	JOY_setEventHandler(&updateControls);
  SPR_init();
  VDP_setPlaneSize(64, 64, TRUE);
  VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);
  PAL_setPalette(PAL0, playerSprite.palette->data, DMA);
  PAL_setPalette(PAL1, startFade.palette->data, DMA);
	PAL_setPalette(PAL2, mapTiles.palette->data, DMA);
	VDP_loadFont(font.tileset, DMA);
	VDP_setTextPriority(1);
	VDP_setTextPalette(PAL0);
}

void loadGame(){
	VDP_clearPlane(BG_A, TRUE);
	VDP_clearPlane(BG_B, TRUE);
  VDP_loadTileSet(mapTiles.tileset, TILE_USER_INDEX, DMA);
	loadUi();
	loadPlayer();
	loadMap();
	bounceOffset = 1;
  started = true;
}

void updateGame(){
	if(levelTransitionClock > 0) updateLevelTransition();
	else{
		updatePlayer();
		bounceEnemies();
	}
	clock++;
	if(clock >= 61440) clock = 60;
}

int main(bool hardReset) {
	loadInternals();
	loadStart();
  while(1) {
  	if(started) updateGame();
  	else updateStart();
		SPR_update();
		SYS_doVBlankProcess();
  }
	return 0;
}