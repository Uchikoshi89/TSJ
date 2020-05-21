#include <math.h >	
#include "main.h"
#include "player.h"
#include "shot.h"
#include "enemy.h"
#include "bullet.h"
#include "blast.h"
#include "key.h"
#include "effects.h"
#include "Score.h"
#include "DataSaveLoad.h"

typedef enum {
	GMODE_INIT,
	GMODE_TITLE,
	GMODE_GAME,
	GMODE_OVER,
	GMODE_CLEAR
}GAME_MODE;

GAME_MODE gameMode;

// title
int titleImage;
int titlecnt;

// bright
int bright;		// 明るさ
int changeMode;	// 0:ﾌｪｰﾄﾞしていない,1: ﾌｪｰﾄﾞ中

int pause;

// gameover
int overImage;
int overcnt;

// hiscore
int hiscore;


// stage
int stageLImage;
int stageRImage;
int backImage;

CHARACTER bzero;	// 真ん中の背景
CHARACTER bcMove;	// 一つ上の背景
CHARACTER bcMove2;	// 一つ下の背景



CHARACTER lstage;
CHARACTER rstage;
CHARACTER lstagemove;
CHARACTER rstagemove;


int  GameSystemInit(void);
void GameInit(void);
void GameMain(void);
void GameDraw(void);
void GameTitle(void);
void GameOver(void);
void BulletCtr(void);
void PlayerCtr(void);
void ShotCtr(void);
void BlastCtr(void);
void BlastCtr2(void);
//void BlastCtr3(void);
void BlastGenerator(xy pos);
void SparkCtr(void);

// ========== WinMain関数 
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	GameSystemInit();
	GameInit();

   // ---------- ｹﾞｰﾑﾙｰﾌﾟ  
	while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0) 
	{
		ClsDrawScreen();

		KeyCheck();

		switch (gameMode)
		{
		case GMODE_INIT:
			GameInit();
			break;
		case GMODE_TITLE:
			GameTitle();
			break;
		case GMODE_GAME:
			GameMain();
			break;
		case GMODE_OVER:
			GameOver();
			break;
		case GMODE_CLEAR:
			break;
		default:
			break;
		}
		// ﾒｲﾝ処理 

		ScreenFlip(); // 裏画面を表画面に瞬間ｺﾋﾟｰ  
	}
	DxLib_End(); // DXﾗｲﾌﾞﾗﾘの終了処理 
	return 0;  // このﾌﾟﾛｸﾞﾗﾑの終了 
}

// ---------- グラフィックの登録 ----------- 
int GameSystemInit(void)
{
	// ｼｽﾃﾑ処理  
	SetGraphMode(SCREEN_SIZE_X, SCREEN_SIZE_Y, 16); // 640×480ﾄﾞｯﾄ65536色ﾓｰﾄﾞに設定  
	ChangeWindowMode(true);     // true:window false:ﾌﾙｽｸﾘｰﾝ  
	SetWindowText("SPACE_TRAVELER");

	// DXﾗｲﾌﾞﾗﾘ初期化処理 
	if (DxLib_Init() == -1)
	{
		return -1;
	}

	SetDrawScreen(DX_SCREEN_BACK);  // ひとまずﾊﾞｯｸﾊﾞｯﾌｧに描画 


	// title
	titleImage = LoadGraph("image/title.png");

	// over
	overImage = LoadGraph("image/GameOver.png");

	// spark
	ChargeEffectSystemInit();

	// player
	PlayerSystemInit();

	// shot
	ShotSystemInit();

	// enemy(2ｺﾏ)
	EnemySystemInit();

	// bullet(2ｺﾏ)
	BulletSystemInit();

	// blast
	BlastSystemInit();

	//number 
	NumberFirstInit();

	// hiscore 
	hiscore = 0;
	HighScoreSaveB();

	// back
	backImage = LoadGraph("image/haikei2.png");

	gameMode = GMODE_INIT;	// INITへ行く
}


// ----------- 変数初期化 ----------
void GameInit(void)
{
	if (hiscore < player.score)
	{
		hiscore = player.score;
		HighScoreSaveB();
	}

	// title
	titlecnt = 0;

	// bright 
	bright = 255;
	pause = 0;

	// over
	overcnt = 0;

	// スクロール座標
	bzero.pos.x = 0;
	bzero.pos.y = 0;
	bcMove.pos.x = 0;
	bcMove.pos.y = -600;
	bcMove2.pos.x = 0;
	bcMove2.pos.y = 600;

	// spark
	ChargeEffectInit();

	// player 
	PlayerInit();

	// shot
	ShotInit();

	// enemy 
	EnemyInit();
	
	// bullet
	BulletInit();

	// blast
	BlastInit();

	gameMode = GMODE_TITLE;	// TITLEへ行く
}

void GameTitle(void)
{
	if (changeMode == 1)
	{
		bright -=4;		// 明るさを減らす
		if (bright < 0) {
			gameMode = GMODE_GAME;
			bright = 0;
		}
	}
	else {
		bright = 255;	// 明るさを最大にする
		if (trgkey[P1_CENTER]) {
			changeMode = 1;		// ﾌｪｰﾄﾞを開始する
		}
	}

	SetDrawBright(bright, bright, bright);

	DrawGraph(SCREEN_SIZE_X / 2 - 360 / 2, 100, titleImage, true);	// ﾀｲﾄﾙ画面

	titlecnt++;
	if (titlecnt / 35 % 2 == 0) {
		DrawString(400 - 8 * 6, 460, "PUSH_SPACE_KEY", 0xffffff);	// 文字の消滅
	}
	DrawString(0, 0, "TITLE", 0xffff00);
}

void GameMain(void)
{
	if(pause == 0){
	if (changeMode == 1)
	{
		bright += 5;
		if (bright > 255) {
			bright = 255;
			changeMode = 0;
		}
		SetDrawBright(bright, bright, bright);
	}

	else {
		PlayerUpData();
		ShotUpData();
		BulletUpData();
		BlastCtr();
		BlastCtr2();
	//	BlastCtr3();
		ChargeEffectUpData();
	}

		GameDraw();
	if ((trgkey[P1_PAUSE]) || (trgkey[P2_PAUSE]))
		{
		pause++;
		pause = pause % 2;
		}
	}

	for (int no = 0; no < 10; no++)
	{
		NumberDraw(800 - 32, 300, player.score);
	}

	NumberDraw(800-32, 0, hiscore);//ハイスコア


	if (playerLife < 0)
	{
		gameMode = GMODE_OVER;
	}
	
	if (playerLife < 0)
	{
		playerLife += 10;
	}
}

void GameDraw(void)
{

	int speed = 5;
	
	DrawString(0, 0, "MAIN", 0xffff00);

	// 背景表示
	DrawGraph(bzero.pos.x, bzero.pos.y, backImage, true);
	DrawGraph(bcMove.pos.x, bcMove.pos.y, backImage, true);
	DrawGraph(bcMove2.pos.x, bcMove2.pos.y, backImage, true);

	bzero.pos.y += 5;
	bcMove.pos.y += 5;
	bcMove2.pos.y += 5;

	//背景スクロール表示
	if (bzero.pos.y >= SCREEN_SIZE_Y)	// Y座標が画面下にいったら
	{
		bzero.pos.y = bcMove.pos.y - 600;	// 画面上にもってくる
	}

	if (bcMove.pos.y >= 600) // Y座標が0にいったら
	{
		bcMove.pos.y = bzero.pos.y -600;	// 一枚目の上にもってくる
	}

	if (bcMove2.pos.y >= 600)
	{
		if (bcMove.pos.y - 600)
		{
			bcMove2.pos.y = bzero.pos.y - 600;	// 
		}
	}

	// spark
	ChargeEffectDraw();

	// player
	PlayerDraw();

	// shot
	ShotDraw();

	// enemy
	EnemyDraw();

	// bullet
	BulletDraw();

	// blast 
	BlastDraw();
	
}

void SparkCtr(void)
{
	ChargeEffectUpData();
}


void PlayerCtr(void)
{
	PlayerUpData();
}

void ShotCtr(void)
{
	ShotUpData();
}

void BulletCtr(void)
{
	BulletUpData();
}

void GameOver(void)
{
	if (trgkey[P1_CENTER])
	{
		if (hiscore < player.score)
		{
			hiscore = player.score;
			HighScoreSaveB();
		}
			gameMode = GMODE_TITLE;	// TITLEへ行く
	}

	DrawGraph(SCREEN_SIZE_X / 2 - 430 / 2, 100, overImage, true);

	overcnt++;
	if (overcnt / 15 % 2 == 0) {
		DrawString(400 - 8 * 6, 460, "PUSH_SPACE_KEY", 0xffffff);	// 文字の消滅
	}
	DrawString(0, 0, "GAMEOVER", 0xffff00);

}


}
