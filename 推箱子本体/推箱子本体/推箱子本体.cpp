#include <graphics.h>
#include <conio.h>
#include <string>
#include <iostream>


using namespace std;

#define LINE 9  //行数为9
#define COLUMN 12  //列数为12

#define SCREEN_WIDTH 780  //舞台宽度
#define SCREEN_HEIGHT 585  //舞台高度

#define Normalsize 65  //道具一般尺寸，也是每个位置的偏移量

#define START_X  0    //地图的起始坐标
#define START_Y  0    //地图的起始坐标

//热键
#define KEY_UP  'w'    //char 'w'
#define KEY_LEFT  'a'    //char 'a'
#define KEY_RIGHT  'd'    //char 'd'
#define KEY_DOWN  's'    //char 's'
#define KEY_QUIT  'q'    //char 'q'

//宏判断
#define isValid(pos) (pos.x>=0&&next_pos.x<COLUMN&&pos.y>=0&&next_pos.y<LINE)
struct POS {
    int x;//小人所在的二维数组的列
    int y;//小人所在的二维数组的行
}man;//记录小人的当前坐标

//函数声明
void gameControl(char direct);
void changeMap(POS* pos, enum _PROPS prop);
void gameOverScene(IMAGE* bg);
bool isGameOver();
//枚举，会默认从0开始给其赋值
enum _PROPS {
    WALL,  //墙 0
    FLOOR, //地板 1
    BOX_des,//箱子目的地 2
    MAN,    //小人 3
    BOX,  //箱子4
    _BOX,  //到达目的地的箱子5
    _MAN  //此时人站在目的地上6
};

IMAGE images[7];
//地图
int map[LINE][COLUMN] = {
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
    { 0, 1, 4, 1, 0, 2, 1, 0, 2, 1, 0, 0 },
    { 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0 },
    { 0, 1, 0, 2, 0, 1, 1, 4, 1, 1, 1, 0 },
    { 0, 1, 1, 1, 0, 3, 1, 1, 1, 4, 1, 0 },
    { 0, 1, 2, 1, 1, 4, 1, 1, 1, 1, 1, 0 },
    { 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
};
//地图标记目的地的点
int flag[LINE][COLUMN] = { {0} };
/************************
*实现两个坐标的图形双方单向转换
*
************************/
void changeMap(POS* pos, enum _PROPS prop) {
    //在地图map的指定坐标修改值，对应新图形
    map[pos->y][pos->x] = prop;
    //在新的位置生成对应的图形
    putimage(START_X + pos->x * Normalsize, START_Y + pos->y * Normalsize, &images[prop]);
}
/************************
*实现游戏四个方向的控制
* 输入：
* direct - 人前进方向
* 输出：无
*
************************/
void gameControl(char direct) {
    POS next_pos = man;//人的前一格位置
    POS next_next_pos = man;//人的前两格位置

    //计算人的前一个位置和前两格位置
    switch (direct) {
    case KEY_UP:
        next_pos.y = man.y - 1;
        next_next_pos.y = man.y - 2;
        break;
    case KEY_DOWN:
        next_pos.y = man.y + 1;
        next_next_pos.y = man.y + 2;
        break;
    case KEY_LEFT:
        next_pos.x = man.x - 1;
        next_next_pos.x = man.x - 2;
        break;
    case KEY_RIGHT:
        next_pos.x = man.x + 1;
        next_next_pos.x = man.x + 2;
        break;
    default:  //不做处理
        return;
    };

    //如果人的前面是地板，且地板位置是合法的，则人的坐标记录改变，前一位置改成人
    if (isValid(next_pos) && map[next_pos.y][next_pos.x] == FLOOR) {
        changeMap(&next_pos, MAN);  //将前面位置的地板改成人
        if(flag[man.y][man.x]==0)       //判断人原来所站位置是否是目的地
            changeMap(&man, FLOOR);     //将人原来位置改成地板
        else changeMap(&man, BOX_des);     //将人原来位置改成目的地
        man = next_pos;             //人目前记录的坐标改变
    }
    //人的前方是箱子，两种情况，箱子前面是地板或者是箱子目的地 
    else if (isValid(next_next_pos) && map[next_pos.y][next_pos.x] == BOX)
    {
        if (map[next_next_pos.y][next_next_pos.x] == FLOOR) {
            changeMap(&next_next_pos, BOX);
            changeMap(&next_pos, MAN); //小人前进一格
            if (flag[man.y][man.x] == 0)//判断人原来所站位置是否是目的地
                changeMap(&man, FLOOR);     //将人原来位置改成地板
            else changeMap(&man, BOX_des);     //将人原来位置改成目的地
            man = next_pos;
        }
        else if (map[next_next_pos.y][next_next_pos.x] == BOX_des)
        {
            changeMap(&next_next_pos, _BOX);
            changeMap(&next_pos, MAN); //小人前进一格 
            if (flag[man.y][man.x] == 0)//判断人原来所站位置是否是目的地
                changeMap(&man, FLOOR);     //将人原来位置改成地板
            else changeMap(&man, BOX_des);     //将人原来位置改成目的地
            man = next_pos;
        }
    }
    //人前面是目的地
    else if (isValid(next_next_pos) && map[next_pos.y][next_pos.x] == BOX_des)
    {
        changeMap(&next_pos, _MAN);  //将前面位置的目的地改成（既有人且有目的地）
        if (flag[man.y][man.x] == 0)//判断人原来所站位置是否是目的地
            changeMap(&man, FLOOR);     //将人原来位置改成地板
        else changeMap(&man, BOX_des);     //将人原来位置改成目的地
        man = next_pos;             //人目前记录的坐标改变
    }
    //人前面（既是目的地又是箱子）,两种情况，箱子前面是地板或者是箱子目的地 
    else if (isValid(next_next_pos) && map[next_pos.y][next_pos.x] == _BOX)
    {
        if (map[next_next_pos.y][next_next_pos.x] == FLOOR) {
            changeMap(&next_next_pos, BOX);  //箱子前进
            changeMap(&next_pos, _MAN); //小人前进一格，变成既有人又有目的地
            if (flag[man.y][man.x] == 0)//判断人原来所站位置是否是目的地
                changeMap(&man, FLOOR);     //将人原来位置改成地板
            else changeMap(&man, BOX_des);     //将人原来位置改成目的地
            man = next_pos;
        }
        else if (map[next_next_pos.y][next_next_pos.x] == BOX_des)
        {
            changeMap(&next_next_pos, _BOX);//箱子前进，既有箱子又有目的地
            changeMap(&next_pos, _MAN); //小人前进一格 ，变成既有人又有目的地
            if (flag[man.y][man.x] == 0)//判断人原来所站位置是否是目的地
                changeMap(&man, FLOOR);     //将人原来位置改成地板
            else changeMap(&man, BOX_des);     //将人原来位置改成目的地
            man = next_pos;
        }
    }
}

/************************
*实现游戏结束
* 输入：背景图变量
* 输出：背景图变量与祝贺话语。
************************/
void gameOverScene(IMAGE* bg) {
    putimage(0, 0, bg);
    settextcolor(WHITE); //字体颜色
    RECT rec = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };
    settextstyle(20, 0, _T("宋体"));  //字体样式
    drawtext(_T("恶作剧的对象……是你哦！"),
        & rec, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
//DT_SINGLELINE :单行显示。DT_CENTER:居中显示。DT_VCENTER : 垂直居中显示
}
/************************
*实现判断游戏结束
************************/
bool isGameOver() {
    for (int i = 0; i < LINE; i++) {
        for (int j = 0; j < COLUMN; j++) {
            if (map[i][j] == BOX_des)
                return false;
        }
    }
    return true;
}
int main()
{

    /************************
    *构建图形界面
    * 
    ************************/

    //设定一个图的变量（作为背景图）
    IMAGE bg_image;

    //搭台唱戏(创建舞台，设定宽高)
    initgraph(SCREEN_WIDTH, SCREEN_HEIGHT);
    
    //将图片文件加载到图变量内（给图变量指针，文件名必须要有双眼号，图片要在工程文件下）
    loadimage(&bg_image, _T("QQ图片20220305233231.png"), SCREEN_WIDTH, SCREEN_HEIGHT,true);
    //将已加载图片的图变量(背景)放到舞台上
    putimage(0, 0, &bg_image);


    //加载所有道具给图变量数组
    //注意：文件字符串的格式多样，需要用加上 _T("")的方法统一接口格式。
    //对于字符串，例如 “abc” 用 _T(“abc”) 表示。就是加上 _T("")
    loadimage(&images[WALL], _T("wall_right.bmp"), Normalsize, Normalsize, true);
    loadimage(&images[FLOOR], _T("floor.bmp"), Normalsize, Normalsize, true);
    loadimage(&images[BOX_des], _T("des.bmp"), Normalsize, Normalsize, true);
    loadimage(&images[MAN], _T("man.bmp"), Normalsize, Normalsize, true);
    loadimage(&images[BOX], _T("box.bmp"), Normalsize, Normalsize, true);
    loadimage(&images[_BOX], _T("box.bmp"), Normalsize, Normalsize, true);
    loadimage(&images[_MAN], _T("man.bmp"), Normalsize, Normalsize, true);
    //初始化
    //显示每个位置的图标
    for (int i = 0; i < LINE; i++) {
        for (int j = 0; j < COLUMN; j++) {
            if (map[i][j] == MAN) {  //如果遍历到人，就记录人的坐标
                man.y = i;
                man.x = j;
            }
            if (map[i][j] == BOX_des) {  //如果遍历到目的地，则标记为1
                flag[i][j] = 1;
            }
            putimage(START_X+j* Normalsize, START_Y+i*Normalsize,&images[map[i][j]]);
        }
    }

    /************************
    *游戏环节:热键控制
    * 
    ************************/
    //给热键输入的循环条件
    bool quit = false;  
    do {
        //keyborder函数，如果有检测到热键，返回true
        if (_kbhit()) {
            char ch = _getch();  //获取热键字符
            switch (ch) {
            case KEY_UP:  
                gameControl(KEY_UP);   //操控
                if (isGameOver()) {    //判断是否游戏结束
                    gameOverScene(&bg_image); quit = true; }
                break;
            case KEY_LEFT:
                gameControl(KEY_LEFT);
                if (isGameOver()) {
                    gameOverScene(&bg_image); quit = true;}
                break;
            case KEY_RIGHT:
                gameControl(KEY_RIGHT);
                if (isGameOver()) {
                    gameOverScene(&bg_image); quit = true;}
                break;
            case KEY_DOWN:
                gameControl(KEY_DOWN);
                if (isGameOver()) {
                    gameOverScene(&bg_image); quit = true;}
                break;
            case KEY_QUIT:
                quit=true;  //遇到q键退出
                break;
            }
        }
    } while (!quit);
    ::system("pause");
    return 0;
}
