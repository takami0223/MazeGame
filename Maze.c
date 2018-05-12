#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*迷路の大きさ*/
#define MAZE_PAZE   3
#define MAZE_HEIGHT 15
#define MAZE_WIDTH  25

/*迷路で使う文字*/
#define CHAR_WALL   '#'
#define CHAR_ROAD   ' '
#define CHAR_START  'S'
#define CHAR_GOAL   'G'
#define CHAR_PLAYER 'P'
#define CHAR_COIN   '$'
#define CHAR_WARP   'W'
#define CHAR_HOLE   '*'
#define CHAR_SCOOP  '!'

/*色指定*/
#define ANSI_RESET      "\x1b[0m"      /* 色のリセット */
#define ANSI_WHITE      "\x1b[47m"     /*背景白*/ //(道フォント)
#define ANSI_RED        "\x1b[47m"     /*  赤  */ //(プレイヤーフォント)
#define ANSI_GREEN      "\x1b[32m"     /*  緑  */ //(ゴールフォント)
#define ANSI_YELLOW     "\x1b[33m"     /*  黄  */ //(コインフォント)
#define ANSI_BLUE       "\x1b[34m"     /*  青  */ //(ワープフォント)
#define ANSI_PINK       "\x1b[35m"     /*ピンク*/ //(スタートフォント)
#define ANSI_CYAN       "\x1b[36m"     /*シアン*/ //(GOALフォント)
#define ANSI_CYANS      "\x1b[9;96;36m"/*シアン*/ //(落とし穴フォント)
#define ANSI_BRIGHT_RED "\x1b[5;255;38m"

/*方向*/
#define LEFT  0
#define DOWN  1
#define RIGHT 2
#define UP    3

/*その他*/
#define MAX_COIN  10
#define MAX_HOLE  2
#define MAX_SCOOP 1
#define MAX_FLOOR 3
#define MAX_PLAY  30

/*-----------------------------------------------------------------------*/
/*経過時間の測定に用いる関数                                             */
/*-----------------------------------------------------------------------*/
double microtime(void){
    struct timeval tv;
    int ret;
    ret = gettimeofday(&tv,NULL);
    if(ret == 0){
        return tv.tv_sec + (double)tv.tv_usec/1000000;
    }else{
        return -1;
    }
}

/*------------------------------------------------------------------------*/
/*特定の文字について，個数(found)，座標(*x,*y)を返す関数                  */
/*------------------------------------------------------------------------*/
int getloc(char maze[MAZE_PAZE][MAZE_HEIGHT][MAZE_WIDTH], char c, int *x, int *y,int p){

    int i,j;
    int found = 0;

    for(i = 0;i < MAZE_HEIGHT;i++){
        for(j = 0;j < MAZE_WIDTH;j++){
            if(c == maze[p][i][j]){
                *x = j;
                *y = i;
                found++;
            }
        }
    }
    return found;
}

/*------------------------------------------------------------------------*/
/*特定の文字について，個数(found)，座標(*x,*y)を返す関数                  */
/*------------------------------------------------------------------------*/
int getcount(char maze[MAZE_PAZE][MAZE_HEIGHT][MAZE_WIDTH], char c, int p){

    int i,j;
    int found = 0;

    for(i = 0;i < MAZE_HEIGHT;i++){
        for(j = 0;j < MAZE_WIDTH;j++){
            if(c == maze[p][i][j]){
                found++;
            }
        }
    }
    return found;
}

/*------------------------------------------------------------------------*/
/*壁を棒倒し法で増やす関数                                                */
/*------------------------------------------------------------------------*/
void putnextwall(char maze[MAZE_PAZE][MAZE_HEIGHT][MAZE_WIDTH],int x,int y,int p,int direction){

    int xloc = x,yloc = y;

    do{
        xloc = x;
        yloc = y;
        switch(random() % direction){
            case LEFT:  /*左に増築*/
                xloc--;
                break;
            case DOWN:  /*下に増築*/
                yloc++;
                break;
            case RIGHT: /*右に増築*/
                xloc++;
                break;
            case UP:    /*上に増築*/
                yloc--;
                break;
        }
    }while(maze[p][yloc][xloc] == CHAR_WALL);
    maze[p][yloc][xloc] = CHAR_WALL;
}

/*------------------------------------------------------------------------*/
/*迷路を自動生成する関数(棒倒し法)                                        */
/*------------------------------------------------------------------------*/
void generatemaze(char maze[MAZE_PAZE][MAZE_HEIGHT][MAZE_WIDTH]){

    int p,i,j;

    /*迷路の外壁，２マスごとに壁(CHAR_WALL)を生成*/
    for(p = 0;p < MAZE_PAZE;p++){
        i = 0;j = 0;
        for(i = 0;i < MAZE_HEIGHT;i++){
            for(j = 0;j < MAZE_WIDTH;j++){
                if(i == 0||j == 0||i == MAZE_HEIGHT-1||j == MAZE_WIDTH-1||(i % 2 == 0 && j % 2 == 0)){
                    maze[p][i][j] = CHAR_WALL;
                }else{
                    maze[p][i][j] = CHAR_ROAD;
                }
            }
        }
        /*面ごとにスタート，ワープまたはゴールを配置*/
        maze[p][1][1] = CHAR_START;
        if(p == MAZE_PAZE-1){
            maze[p][MAZE_HEIGHT-2][MAZE_WIDTH-2] = CHAR_GOAL;
        }else{
            maze[p][MAZE_HEIGHT-2][MAZE_WIDTH-2] = CHAR_WARP;
        }
    }

    /*棒倒し法で壁を増やす*/
    for(p = 0;p < MAZE_PAZE;p++){
        i = 2;j = 2;
        for(i = 2;i < MAZE_HEIGHT-1;i+=2){
            for(j = 2;j < MAZE_WIDTH-1;j+=2){
                if(i == 2){
                    putnextwall(maze,j,i,p,4);
                }else{
                    putnextwall(maze,j,i,p,3);
                }
            }
        }
    }
}


/*------------------------------------------------------------------------*/
/*迷路を出力し，プレイヤーの位置(plx,ply)も判定する関数                   */
/*------------------------------------------------------------------------*/
void showmaze(char maze[MAZE_PAZE][MAZE_HEIGHT][MAZE_WIDTH], int plx, int ply,int p){

    int i,j;

    for(i = 0;i < MAZE_HEIGHT;i++){
        for(j = 0;j < MAZE_WIDTH;j++){
            if(plx == j && ply == i){
                printf(ANSI_RED"%c"ANSI_RESET,CHAR_PLAYER);
            }else{
                if(maze[p][i][j] == CHAR_COIN){     //コイン出力
                    printf(ANSI_YELLOW"%c"ANSI_RESET,maze[p][i][j]);
                }else
                    if(maze[p][i][j] == CHAR_GOAL){ //ゴール出力
                        printf(ANSI_GREEN"%c"ANSI_RESET,maze[p][i][j]);
                    }else
                    if(maze[p][i][j] == CHAR_START){//スタート出力
                        printf(ANSI_PINK"%c"ANSI_RESET,maze[p][i][j]);
                    }else
                    if(maze[p][i][j] == CHAR_WALL){ //壁出力
                        printf(ANSI_BRIGHT_RED"%c"ANSI_RESET,maze[p][i][j]);
                    }else
                    if(maze[p][i][j] == CHAR_WARP){ //ワープ出力
                        printf(ANSI_BLUE"%c"ANSI_RESET,maze[p][i][j]);
                    }else
                    if(maze[p][i][j] == CHAR_HOLE){ //落とし穴出力
                        printf(ANSI_CYAN"%c"ANSI_RESET,maze[p][i][j]);
                    }else
                    if(maze[p][i][j] == CHAR_SCOOP){ //スコップ出力
                        printf(ANSI_BLUE"%c"ANSI_RESET,maze[p][i][j]);
                    }else{
                        putchar(maze[p][i][j]);     //道出力
                }
            }
        }
        printf("\n");
    }
}

/*------------------------------------------------------------------------*/
/*swap関数（クリア記録の並び替え）                                        */
/*------------------------------------------------------------------------*/
void swap(double *a, double *b){
    double tmp;

    tmp = *a;
    *a = *b;
    *b = tmp;
}
/*------------------------------------------------------------------------*/
/*メイン関数（スタート地点の設定，プレイヤーの移動など）                  */
/*------------------------------------------------------------------------*/
int main(int argc, char *argv[]){

    char maze[MAZE_PAZE][MAZE_HEIGHT][MAZE_WIDTH];  //迷路配列
    double start = 0.0,end = 0.0,playtime[MAX_PLAY];//時間関係
    int coin,hole,scoop = 0,old_plp = -1;           //ギミック関係
    int play = 0,min = 0;       //クリア記録関係
    int i,j,x,y,plx,ply,plp;    //プレイヤーの座標関連
    int nenu;   //メニュー関係
    char c;     //方向関係

    srandom(time(NULL));

    do{
        do{
            puts("\t\t*---------------------*");
            puts("\t\t\t1:遊ぶ");
            puts("\t\t\t2:成績");
            puts("\t\t\t0:終了");
            puts("\t\t*---------------------*");
            printf(">>");
            scanf("%d",&nenu);
        }while(nenu != 1 && nenu != 2  && nenu != 0);

        switch(nenu){
            case 1:  //迷路を出力

                generatemaze(maze);  //迷路生成

                start = microtime();
                old_plp = -1;  //プレイごとにリセット必須

                /*ゴールするまで移動を繰り返す*/
                for(plp = 0;plp < MAZE_PAZE;plp++){

                    /*コインの位置をランダムに設定する(各階層一回のみ)*/
                    if(plp > old_plp){  //←最高層を更新した場合実行
                        for(i = 0;i < MAX_COIN;i++){
                            do{
                                x = random() % MAZE_HEIGHT;
                                y = random() % MAZE_WIDTH;
                            }while(maze[plp][x][y] != CHAR_ROAD);
                            maze[plp][x][y] = CHAR_COIN;
                        }
                    }

                    /*落とし穴の位置をランダムに設定する(各階層一回のみ)*/
                    if(plp > 0 && plp > old_plp){   //←１階以上かつ最高層を更新した場合のみ実行
                        for(i = 0;i < MAX_HOLE;i++){
                            do{
                                x = random() % MAZE_HEIGHT;
                                y = random() % MAZE_WIDTH;
                            }while(maze[plp][x][y] != CHAR_ROAD || maze[plp-1][x][y] != CHAR_ROAD);
                            maze[plp][x][y] = CHAR_HOLE;
                        }
                    }

                    /*スコップの位置をランダムに設定する(各奇数階層一回のみ)*/
                    if(plp%2 == 0 && plp > old_plp){
                        for(i = 0;i < MAX_SCOOP;i++){
                            do{
                                x = random() % MAZE_HEIGHT;
                                y = random() % MAZE_WIDTH;
                            }while(maze[plp][x][y] != CHAR_ROAD);
                            maze[plp][x][y] = CHAR_SCOOP;
                        }
                    }
                    old_plp = plp;

                    /*プレイヤの現在位置をスタート地点に設定する*/
                    if(getloc(maze, CHAR_START, &plx, &ply, plp) == 0){
                        puts("スタート地点が見つかりません.");
                        return -1;
                    }

                    while(1){

                        system("clear");

                        if(plp+1 == MAX_FLOOR){
                            printf("＜最終フロア＞\n");
                        }else{
                            printf("＜第%dフロア＞\n",plp+1);
                        }
                        showmaze(maze, plx, ply, plp);
                        coin = getcount(maze, CHAR_COIN, plp);

                        /*残りのコイン枚数を出力*/
                        if(coin == 0){
                            if(plp < old_plp){
                                printf(ANSI_YELLOW"このフロアの条件は既にクリアしています！\n"ANSI_RESET);
                            }else{
                                printf(ANSI_YELLOW"フロア条件クリア！\n"ANSI_RESET);
                            }
                        }else{
                            printf(ANSI_YELLOW"このフロアの残りコイン：%d\n"ANSI_RESET,coin);
                        }

                        /*スコップ所持数を出力*/
                        if(scoop != 0)printf(ANSI_YELLOW"スコップ所持数：%d\n"ANSI_RESET,scoop);

                        /*ワープに入る際,コインを取っているかチェック*/
                        if(maze[plp][ply][plx] == CHAR_WARP){
                            if(coin != 0){
                                puts("コインが足りません");
                            }else{
                                break;
                            }
                        }

                        /*ゴールに入る際,コインを取っているかチェック*/
                        if(maze[plp][ply][plx] == CHAR_GOAL){
                            if(coin != 0){
                                puts("コインが足りません");
                            }else{
                                printf(ANSI_CYAN"GOAL!\n"ANSI_RESET);
                                end = microtime();
                                printf("time：%4.2f[s]\n",end-start);
                                playtime[play] = end-start;     //クリアタイムを記録
                                play++;
                                break;
                            }
                        }

                        puts("動かす方向を入力してください（ [w]上 [s]下 [d]右 [a]左 [q]終了）");
                        scanf("%c",&c);
                        switch(c){
                            case 'w':   /*上へ移動*/
                                if(ply > 0 && maze[plp][ply-1][plx] != CHAR_WALL)ply--;
                                break;
                            case 's':   /*下へ移動*/
                                if(ply < MAZE_HEIGHT && maze[plp][ply+1][plx] != CHAR_WALL)ply++;
                                break;
                            case 'd':   /*右へ移動*/
                                if(plx < MAZE_WIDTH && maze[plp][ply][plx+1] != CHAR_WALL)plx++;
                                break;
                            case 'a':   /*左へ移動*/
                                if(plx > 0 && maze[plp][ply][plx-1] != CHAR_WALL)plx--;
                                break;
                            case 'q':
                                exit(1);

                        }

                        /*コインを検出＆削除*/
                        if(maze[plp][ply][plx] == CHAR_COIN){
                            coin--;
                            maze[plp][ply][plx] = CHAR_ROAD;
                        }

                        /*ワープホールを検出＆削除*/
                        if(maze[plp][ply][plx] == CHAR_HOLE){
                            if(scoop > 0){  //スコップがあるなら
                                maze[plp][ply][plx] = CHAR_ROAD;
                                scoop--;
                            }else{  //スコップがないなら
                                maze[plp][ply][plx] = CHAR_ROAD;
                                plp--;
                            }
                        }

                        /*スコップを検出＆削除*/
                        if(maze[plp][ply][plx] == CHAR_SCOOP){
                            maze[plp][ply][plx] = CHAR_ROAD;
                            scoop++;
                        }
                    }
                }
                break;

            case 2:  //記録出力
                if(play == 0){
                    puts("*記録がありません*");
                    break;
                }
                /*小さい順にソート(選択ソート)*/
                for(i = 0; i < play-1; i++){
                    min = i;
                    for(j = i + 1; j < play; j++){
                        if(playtime[j] < playtime[min])min = j;
                    }
                    swap(&playtime[i],&playtime[min]);
                }

                /*順位を出力*/
                for(i = 0;i < play;i++){
                    printf("%d位:%4.2f[s]\n",i+1,playtime[i]);
                }
                break;

            case 0:
                exit(1);
                break;
        }
    }while(nenu != 0);
    return 0;
}