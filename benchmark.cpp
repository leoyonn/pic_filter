/*****************************************************************************
 * @(#)benchmark.cpp, 2011-5-13.                                             *
 *                                                                           *
 * @author leo                                                               *
 * @desc   提供对PicFilter中的方法进行简单的benchmark的方法，以方便根据结果  *
 *         判断哪些用在某些客户端上可行                                       *
 *****************************************************************************/

#include "pic_filter.h"
#include "xinclude/ximage.h"
#include "color_utils.h"
#include <iostream>
using namespace std;
 
double getTime()
{
    static bool init = false;
    static bool hires = false;
    static __int64 freq = 1;
    if(!init)
    {
        hires = !QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
        if(!hires)
            freq = 1000;
        init = true;
    }
    
    __int64 now;
    
    if(hires)
        QueryPerformanceCounter((LARGE_INTEGER *)&now);
    else
        now = GetTickCount();
    
    return (double)((double)now / (double)freq);
}

#define BENCH_MARK_BEGIN(type, msg) _benchMarkBegin(type, msg)
#define BENCH_MARK_END _benchMarkEnd
static double timeBegin = 0;
static double timeEnd = 0;
static double timeElapse = 0;

inline void _benchMarkBegin(FILTER_TYPE type, const char *msg = null)
{
    printf("--> type: %d: ", type);
    if(msg)
        printf("%-16s: ", msg);
    timeBegin = getTime();
}

inline void _benchMarkEnd(int count)
{
    timeElapse = getTime() - timeBegin;
    printf("%6d 次, 共 %6.3f 秒.\n", count, timeElapse);
}

void benchMark(const char *infile, int count)
{
    // 1. read data
    CubePS ps;
    int width, height;
    Byte *oriData, *resData;
    oriData = loadImageToMem(infile, width, height, CXIMAGE_FORMAT_JPG, 3);
    resData = new Byte[width * height * 3];
    memset(resData, 0, width * height * 3);
    ps.setData(oriData, resData, width, height, 3);
    // 加载纹理
//     ps.loadTexture(GRAIN_PAPER, "E:/_cps/-t/tx-paper2.tex");
//     ps.loadTexture(GRAIN_CLOTH, "E:/_cps/-t/tx-cloth2.tex");
//     ps.loadTexture(GRAIN_WALL, "E:/_cps/-t/tx-wall2.tex");
//     ps.loadTexture(TILING, "E:/_cps/-t/tile.tex");
//     ps.loadTexture(TILING2, "E:/_cps/-t/tile2.tex");
//     ps.loadTexture(TILING3, "E:/_cps/-t/tile3.tex");


    printf("%s: %4d x %4d.\n", infile, width, height);
    // 2. process
    int i;
    

    //     "1. 黑白效果",        //     GRAY,
    BENCH_MARK_BEGIN(GRAY, "不直方图均衡");
    for(i = 0; i < count; i ++)
        ps.doGray(false);
    BENCH_MARK_END(count);
    
    BENCH_MARK_BEGIN(GRAY, "直方图均衡");
    for(i = 0; i < count; i ++)
        ps.doGray(true);
    BENCH_MARK_END(count);
    
    //     "2. 铅笔素描",        //     BINARY,
    BENCH_MARK_BEGIN(BINARY, " ");
    for(i = 0; i < count; i ++)
        ps.doBinary(0.5);
    BENCH_MARK_END(count);

    //     "3. 单色效果",        //     1COLOR,
    BENCH_MARK_BEGIN(1COLOR, "不直方图均衡");
    for(i = 0; i < count; i ++)
        ps.do1Color(0x8c3300, false);
    BENCH_MARK_END(count);
    
    //  其他效果
    // ...

    // 3. save and release data
/*
    CxImage xImg;
    xImg.CreateFromArray(resData, width, height, 24, 3 * width, false);
    xImg.SetJpegQuality(90);
    xImg.Save(outfile, CXIMAGE_FORMAT_JPG);
    */
    delete []oriData;    oriData = null;
    delete []resData;    oriData = null;
}
