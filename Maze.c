#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*���H�̑傫��*/
#define MAZE_PAZE   3
#define MAZE_HEIGHT 15
#define MAZE_WIDTH  25

/*���H�Ŏg������*/
#define CHAR_WALL   '#'
#define CHAR_ROAD   ' '
#define CHAR_START  'S'
#define CHAR_GOAL   'G'
#define CHAR_PLAYER 'P'
#define CHAR_COIN   '$'
#define CHAR_WARP   'W'
#define CHAR_HOLE   '*'
#define CHAR_SCOOP  '!'

/*�F�w��*/
#define ANSI_RESET      "\x1b[0m"      /* �F�̃��Z�b�g */
#define ANSI_WHITE      "\x1b[47m"     /*�w�i��*/ //(���t�H���g)
#define ANSI_RED        "\x1b[47m"     /*  ��  */ //(�v���C���[�t�H���g)
#define ANSI_GREEN      "\x1b[32m"     /*  ��  */ //(�S�[���t�H���g)
#define ANSI_YELLOW     "\x1b[33m"     /*  ��  */ //(�R�C���t�H���g)
#define ANSI_BLUE       "\x1b[34m"     /*  ��  */ //(���[�v�t�H���g)
#define ANSI_PINK       "\x1b[35m"     /*�s���N*/ //(�X�^�[�g�t�H���g)
#define ANSI_CYAN       "\x1b[36m"     /*�V�A��*/ //(GOAL�t�H���g)
#define ANSI_CYANS      "\x1b[9;96;36m"/*�V�A��*/ //(���Ƃ����t�H���g)
#define ANSI_BRIGHT_RED "\x1b[5;255;38m"

/*����*/
#define LEFT  0
#define DOWN  1
#define RIGHT 2
#define UP    3

/*���̑�*/
#define MAX_COIN  10
#define MAX_HOLE  2
#define MAX_SCOOP 1
#define MAX_FLOOR 3
#define MAX_PLAY  30

/*-----------------------------------------------------------------------*/
/*�o�ߎ��Ԃ̑���ɗp����֐�                                             */
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
/*����̕����ɂ��āC��(found)�C���W(*x,*y)��Ԃ��֐�                  */
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
/*����̕����ɂ��āC��(found)�C���W(*x,*y)��Ԃ��֐�                  */
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
/*�ǂ�_�|���@�ő��₷�֐�                                                */
/*------------------------------------------------------------------------*/
void putnextwall(char maze[MAZE_PAZE][MAZE_HEIGHT][MAZE_WIDTH],int x,int y,int p,int direction){

    int xloc = x,yloc = y;

    do{
        xloc = x;
        yloc = y;
        switch(random() % direction){
            case LEFT:  /*���ɑ��z*/
                xloc--;
                break;
            case DOWN:  /*���ɑ��z*/
                yloc++;
                break;
            case RIGHT: /*�E�ɑ��z*/
                xloc++;
                break;
            case UP:    /*��ɑ��z*/
                yloc--;
                break;
        }
    }while(maze[p][yloc][xloc] == CHAR_WALL);
    maze[p][yloc][xloc] = CHAR_WALL;
}

/*------------------------------------------------------------------------*/
/*���H��������������֐�(�_�|���@)                                        */
/*------------------------------------------------------------------------*/
void generatemaze(char maze[MAZE_PAZE][MAZE_HEIGHT][MAZE_WIDTH]){

    int p,i,j;

    /*���H�̊O�ǁC�Q�}�X���Ƃɕ�(CHAR_WALL)�𐶐�*/
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
        /*�ʂ��ƂɃX�^�[�g�C���[�v�܂��̓S�[����z�u*/
        maze[p][1][1] = CHAR_START;
        if(p == MAZE_PAZE-1){
            maze[p][MAZE_HEIGHT-2][MAZE_WIDTH-2] = CHAR_GOAL;
        }else{
            maze[p][MAZE_HEIGHT-2][MAZE_WIDTH-2] = CHAR_WARP;
        }
    }

    /*�_�|���@�ŕǂ𑝂₷*/
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
/*���H���o�͂��C�v���C���[�̈ʒu(plx,ply)�����肷��֐�                   */
/*------------------------------------------------------------------------*/
void showmaze(char maze[MAZE_PAZE][MAZE_HEIGHT][MAZE_WIDTH], int plx, int ply,int p){

    int i,j;

    for(i = 0;i < MAZE_HEIGHT;i++){
        for(j = 0;j < MAZE_WIDTH;j++){
            if(plx == j && ply == i){
                printf(ANSI_RED"%c"ANSI_RESET,CHAR_PLAYER);
            }else{
                if(maze[p][i][j] == CHAR_COIN){     //�R�C���o��
                    printf(ANSI_YELLOW"%c"ANSI_RESET,maze[p][i][j]);
                }else
                    if(maze[p][i][j] == CHAR_GOAL){ //�S�[���o��
                        printf(ANSI_GREEN"%c"ANSI_RESET,maze[p][i][j]);
                    }else
                    if(maze[p][i][j] == CHAR_START){//�X�^�[�g�o��
                        printf(ANSI_PINK"%c"ANSI_RESET,maze[p][i][j]);
                    }else
                    if(maze[p][i][j] == CHAR_WALL){ //�Ǐo��
                        printf(ANSI_BRIGHT_RED"%c"ANSI_RESET,maze[p][i][j]);
                    }else
                    if(maze[p][i][j] == CHAR_WARP){ //���[�v�o��
                        printf(ANSI_BLUE"%c"ANSI_RESET,maze[p][i][j]);
                    }else
                    if(maze[p][i][j] == CHAR_HOLE){ //���Ƃ����o��
                        printf(ANSI_CYAN"%c"ANSI_RESET,maze[p][i][j]);
                    }else
                    if(maze[p][i][j] == CHAR_SCOOP){ //�X�R�b�v�o��
                        printf(ANSI_BLUE"%c"ANSI_RESET,maze[p][i][j]);
                    }else{
                        putchar(maze[p][i][j]);     //���o��
                }
            }
        }
        printf("\n");
    }
}

/*------------------------------------------------------------------------*/
/*swap�֐��i�N���A�L�^�̕��ёւ��j                                        */
/*------------------------------------------------------------------------*/
void swap(double *a, double *b){
    double tmp;

    tmp = *a;
    *a = *b;
    *b = tmp;
}
/*------------------------------------------------------------------------*/
/*���C���֐��i�X�^�[�g�n�_�̐ݒ�C�v���C���[�̈ړ��Ȃǁj                  */
/*------------------------------------------------------------------------*/
int main(int argc, char *argv[]){

    char maze[MAZE_PAZE][MAZE_HEIGHT][MAZE_WIDTH];  //���H�z��
    double start = 0.0,end = 0.0,playtime[MAX_PLAY];//���Ԋ֌W
    int coin,hole,scoop = 0,old_plp = -1;           //�M�~�b�N�֌W
    int play = 0,min = 0;       //�N���A�L�^�֌W
    int i,j,x,y,plx,ply,plp;    //�v���C���[�̍��W�֘A
    int nenu;   //���j���[�֌W
    char c;     //�����֌W

    srandom(time(NULL));

    do{
        do{
            puts("\t\t*---------------------*");
            puts("\t\t\t1:�V��");
            puts("\t\t\t2:����");
            puts("\t\t\t0:�I��");
            puts("\t\t*---------------------*");
            printf(">>");
            scanf("%d",&nenu);
        }while(nenu != 1 && nenu != 2  && nenu != 0);

        switch(nenu){
            case 1:  //���H���o��

                generatemaze(maze);  //���H����

                start = microtime();
                old_plp = -1;  //�v���C���ƂɃ��Z�b�g�K�{

                /*�S�[������܂ňړ����J��Ԃ�*/
                for(plp = 0;plp < MAZE_PAZE;plp++){

                    /*�R�C���̈ʒu�������_���ɐݒ肷��(�e�K�w���̂�)*/
                    if(plp > old_plp){  //���ō��w���X�V�����ꍇ���s
                        for(i = 0;i < MAX_COIN;i++){
                            do{
                                x = random() % MAZE_HEIGHT;
                                y = random() % MAZE_WIDTH;
                            }while(maze[plp][x][y] != CHAR_ROAD);
                            maze[plp][x][y] = CHAR_COIN;
                        }
                    }

                    /*���Ƃ����̈ʒu�������_���ɐݒ肷��(�e�K�w���̂�)*/
                    if(plp > 0 && plp > old_plp){   //���P�K�ȏォ�ō��w���X�V�����ꍇ�̂ݎ��s
                        for(i = 0;i < MAX_HOLE;i++){
                            do{
                                x = random() % MAZE_HEIGHT;
                                y = random() % MAZE_WIDTH;
                            }while(maze[plp][x][y] != CHAR_ROAD || maze[plp-1][x][y] != CHAR_ROAD);
                            maze[plp][x][y] = CHAR_HOLE;
                        }
                    }

                    /*�X�R�b�v�̈ʒu�������_���ɐݒ肷��(�e��K�w���̂�)*/
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

                    /*�v���C���̌��݈ʒu���X�^�[�g�n�_�ɐݒ肷��*/
                    if(getloc(maze, CHAR_START, &plx, &ply, plp) == 0){
                        puts("�X�^�[�g�n�_��������܂���.");
                        return -1;
                    }

                    while(1){

                        system("clear");

                        if(plp+1 == MAX_FLOOR){
                            printf("���ŏI�t���A��\n");
                        }else{
                            printf("����%d�t���A��\n",plp+1);
                        }
                        showmaze(maze, plx, ply, plp);
                        coin = getcount(maze, CHAR_COIN, plp);

                        /*�c��̃R�C���������o��*/
                        if(coin == 0){
                            if(plp < old_plp){
                                printf(ANSI_YELLOW"���̃t���A�̏����͊��ɃN���A���Ă��܂��I\n"ANSI_RESET);
                            }else{
                                printf(ANSI_YELLOW"�t���A�����N���A�I\n"ANSI_RESET);
                            }
                        }else{
                            printf(ANSI_YELLOW"���̃t���A�̎c��R�C���F%d\n"ANSI_RESET,coin);
                        }

                        /*�X�R�b�v���������o��*/
                        if(scoop != 0)printf(ANSI_YELLOW"�X�R�b�v�������F%d\n"ANSI_RESET,scoop);

                        /*���[�v�ɓ����,�R�C��������Ă��邩�`�F�b�N*/
                        if(maze[plp][ply][plx] == CHAR_WARP){
                            if(coin != 0){
                                puts("�R�C��������܂���");
                            }else{
                                break;
                            }
                        }

                        /*�S�[���ɓ����,�R�C��������Ă��邩�`�F�b�N*/
                        if(maze[plp][ply][plx] == CHAR_GOAL){
                            if(coin != 0){
                                puts("�R�C��������܂���");
                            }else{
                                printf(ANSI_CYAN"GOAL!\n"ANSI_RESET);
                                end = microtime();
                                printf("time�F%4.2f[s]\n",end-start);
                                playtime[play] = end-start;     //�N���A�^�C�����L�^
                                play++;
                                break;
                            }
                        }

                        puts("��������������͂��Ă��������i [w]�� [s]�� [d]�E [a]�� [q]�I���j");
                        scanf("%c",&c);
                        switch(c){
                            case 'w':   /*��ֈړ�*/
                                if(ply > 0 && maze[plp][ply-1][plx] != CHAR_WALL)ply--;
                                break;
                            case 's':   /*���ֈړ�*/
                                if(ply < MAZE_HEIGHT && maze[plp][ply+1][plx] != CHAR_WALL)ply++;
                                break;
                            case 'd':   /*�E�ֈړ�*/
                                if(plx < MAZE_WIDTH && maze[plp][ply][plx+1] != CHAR_WALL)plx++;
                                break;
                            case 'a':   /*���ֈړ�*/
                                if(plx > 0 && maze[plp][ply][plx-1] != CHAR_WALL)plx--;
                                break;
                            case 'q':
                                exit(1);

                        }

                        /*�R�C�������o���폜*/
                        if(maze[plp][ply][plx] == CHAR_COIN){
                            coin--;
                            maze[plp][ply][plx] = CHAR_ROAD;
                        }

                        /*���[�v�z�[�������o���폜*/
                        if(maze[plp][ply][plx] == CHAR_HOLE){
                            if(scoop > 0){  //�X�R�b�v������Ȃ�
                                maze[plp][ply][plx] = CHAR_ROAD;
                                scoop--;
                            }else{  //�X�R�b�v���Ȃ��Ȃ�
                                maze[plp][ply][plx] = CHAR_ROAD;
                                plp--;
                            }
                        }

                        /*�X�R�b�v�����o���폜*/
                        if(maze[plp][ply][plx] == CHAR_SCOOP){
                            maze[plp][ply][plx] = CHAR_ROAD;
                            scoop++;
                        }
                    }
                }
                break;

            case 2:  //�L�^�o��
                if(play == 0){
                    puts("*�L�^������܂���*");
                    break;
                }
                /*���������Ƀ\�[�g(�I���\�[�g)*/
                for(i = 0; i < play-1; i++){
                    min = i;
                    for(j = i + 1; j < play; j++){
                        if(playtime[j] < playtime[min])min = j;
                    }
                    swap(&playtime[i],&playtime[min]);
                }

                /*���ʂ��o��*/
                for(i = 0;i < play;i++){
                    printf("%d��:%4.2f[s]\n",i+1,playtime[i]);
                }
                break;

            case 0:
                exit(1);
                break;
        }
    }while(nenu != 0);
    return 0;
}