/*****************************************************************************
 * @(#)pic_filter.h, 2011-5-13.                                              *
 *                                                                           *
 * @author leo                                                               *
 * @desc PicFilter的类及成员函数与属性声明                                   *
 *       PicFilter是一个开源的图像滤镜模块                                   *
 *****************************************************************************/

#pragma once

#include "colorUtils.h"

#define TRAVERSE_IMG_BEGIN \
    int x, y; \
    for(y = 0; y < imgData.height; y ++) \
    { \
    for(x = 0; x < imgData.width; x ++) 
#define TRAVERSE_IMG_END \
    }

/**
 * 滤镜类型
 */
enum FILTER_TYPE {
    GRAY,
    BINARY,
    1COLOR,
    SCANLINE,
    GRAIN,
    TILING,
    LOMO,
    REVERSE,
    PAINTING,
    REVERSEFILM,
    USM,
    LEGEND,
    OLDFILM,
    NOISE,
    GALLERY,
    SMOOTH,
    LOMO_EX,
    HDR,
    SOFTLIGHT,
    GRAY_EX,
    THE80S,
    TILTSHIFT,
    GOLDEN,
    EARLYBIRD,
    FADE,
    PINK,
    SUMMER,
    SANDSTORM,
    NIGHT,
    ANTIQUE,
    OVEREXPOSE,
    NASHVILLE,
    TEST,
};

/**
 * 图片在内存中的数据
 */
struct ImageData {
    int     width;          // 宽度
    int     height;         // 高度
    int     bytesPerPixel;  // 每个pixel的byte数(暂仅支持3，其它如4请自行扩展)
    Byte    *oriData;       // 原数据
    Byte    *resData;       // 处理结果
    long    nBytes;         // 字节数
    E_RGB_ORDER corder;     // 0xRRGGBB or 0xBBRRGG
};

/**
 * 使用到纹理的滤镜纹理类型
 * @deprecated
 */
enum E_GRAIN_TYPE {
    GRAIN_PAPER = 0,
    GRAIN_CLOTH = 1,
    GRAIN_WALL = 2,
    TILING = 3,
    TILING2 = 4,
    TILING3 = 5,
};

/**
 * 纹理个数
 * @deprecated
 */
const int nGrainType = 6;

/**
 * 纹理数据
 * @deprecated
 */
struct TextureData {
    E_GRAIN_TYPE    type;
    int             width;
    int             height;
    Byte            *data;
};

class PicFilter  
{
public:
    PicFilter();
    virtual ~PicFilter();

    void setData(Byte *in, Byte *out, int width, int height, int bytesPerPixel = 3, E_RGB_ORDER order = CO_0xRRGGBB);
    void setParameters(FILTER_TYPE type, Color color, Real ratio, Real weight, Real density);
    void doFilter(FILTER_TYPE type);

    void doGray(bool histEqual = true);
    void doBinary(Real threshold = 0.4);
    void doLomo(Color color = COLOR_BLACK, Real ratio = 0.5, Real weight = 0.5, Real noiseWeight = 0.3f);
    void doTiltShift(Real ratio = 0.5, Real weight = 1);
    void do1Color(Color color, bool histEqual = true);
    void doLegend(Real weight);
    void doOldFilm(Color color, Real weight, Real density);
    void doReverse();
    void doScanLine(Color color = 0x000000, Real lineWidth = 1, Real weight = 0.75, Real density = 0.5);
    void doGallery(Color color, Real weight);
    void doGrain(E_GRAIN_TYPE gtype, Real weight, Real scale, int deltaH);
    void doNoise(Real weight, Real density);
    void doTiling(int size, int shift);
    void doSmoothGauss(int tsize = 5, Real sigma = 1);
    void doSmoothAve(int tsize = 5);
    void doReversalFilm(Real weight = 1);
    void doUnSharpMask(Real radius, Real amount, Real threshold);
    void doSoftLight(Real radius, Real weight, Real contrast);
    void doPainting(int radius = 3, bool histEqual = true);
    void doHDR();
    void doThe80s();
    void doGolden();
    void doFade();
    void doPink();
    void doBlackAndWhite();
    void doLomoEx();
    void doEarlyBirds();
    void doLily();
    void doSummer();
    void doNashvilleCurve();
    void doNight();
    void doSandStorm();
    void doOverExposure();    
    void doRedNashville();
    void doXProPlusNashville();
    void doAntique();

private:
    Color   getPixel(int x, int y, Byte *data = null);
    void    setPixel(Color color, int x, int y, Byte *data = null);
    Color   computeTileColor(int x, int y, int tsize = 8);
    void    setTileColor(Color color, Color upperColor, int x, int y, int tsize = 8);
    void    setTileColor3D(Color color, Color upperColor, int x, int y, int tsize = 8, int shift = 2);
    void    makeGauss(Real *gtemp, int tsize = 3, Real sigma = 1);
    void    applyTemplateAve(int tsize, int x, int y);
    void    applyTemplate(Real *gtemp, int tsize, int x, int y);
    void    histogramEqualization(Byte *data, int width, int height, int bytesPerPixel = 3);
    void    histogramEqualizationRGB(Byte *data, int width, int height, int bytesPerPixel = 3);
    void    loadTexture(E_GRAIN_TYPE type, const char *file);
    void    setTexture(E_GRAIN_TYPE type, Byte *data, int width, int height);
    void    saveTexture(E_GRAIN_TYPE type, const char *file);
    void    releaseTexture(E_GRAIN_TYPE type);
    void    computeReversalMap(Real weight = 1);
    void    multiply(Byte *data, Real daugR, Real daugG, Real daugB);
    void    crossProcessCurve(Byte *input, Byte *output);
    void    curveForBlurFilter(Byte *in, Byte * out, int w, int h);
    void    highContrastCurve(Byte * data);
    void    darkenCorners1(Byte *data, Real weight, Real radius);
    void    darkenCorners2(Byte *data, Real weight, Real radius);
    void    setSaturation(Byte *input, Byte *output, float weight);
    void    lightShadowWithYellowCast(Byte *input, Byte *output);
    void    sepiaCast(Byte * data);

private:
    ImageData       imgData;
    Real            weight;
    Byte            emap[256];

    // @deprecated
    TextureData     texData[nGrainType];
// 
//     FILTER_TYPE    type;
//     Color        color;
//     Real        ratio;
//     Real        lineWidth;
//     Real        density;
// 
//     int            tsize;
//     int            shift;
// 

};

extern Byte *loadImageToMem(const char* file, int &width, int &height, int imgType, int channel);
