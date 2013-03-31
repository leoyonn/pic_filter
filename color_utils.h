/*****************************************************************************
 * @(#)color_utils.cpp, 2011-5-13.                                           *
 *                                                                           *
 * @author leo                                                               *
 * @desc 提供一些简单的像素操作内联函数                                      *
 *****************************************************************************/

#pragma once

#include <math.h>
#pragma warning(disable:4005)
#pragma warning(disable:4305)
#pragma warning(disable:4244)

#ifndef null
    #define null 0
#endif
#ifndef PI
    #define PI 3.141592653589793f
#endif
#ifndef INFINITE
    #define INFINITE 0x7FFFFFFF
#endif

// 注意：与wingdi中的定义不同，这里一个Dword中是：0xRRGGBB的格式
#define RGB(r, g, b)    ((Color)(((Byte)(b) | ((Word)((Byte)(g)) << 8)) | (((Color)(Byte)(r)) << 16)))
#define RGBF(r, g, b)    (RGB((r) * 255, (g) * 255, (b) * 255))

#define getRValue(rgb)    ((Byte)((rgb) >> 16))
#define getGValue(rgb)    ((Byte)(((Word)(rgb)) >> 8))
#define getBValue(rgb)    ((Byte)(rgb))

/** 获取a, b, c的最小值 */
#define min3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

/** 获取a, b, c的最大值 */
#define max3(a, b, c) ((a) > (b) ? ((a) > (c) ? (a) : (c)) : ((b) > (c) ? (b) : (c)))

/** 将v限制在a, b之间 (a <= b) */
#define clamp(v, a, b) ((v) > (b) ? (b) : ((v) < (a) ? (a) : (v)))

typedef unsigned long   Dword;
typedef unsigned short  Word;
typedef unsigned char   Byte;
typedef Dword           Color;
typedef float           Real;

// 彩色转黑白各通道的因子
const Real LUMI_RATIO_R = 0.299;
const Real LUMI_RATIO_G = 0.587;
const Real LUMI_RATIO_B = 0.114;

/**
 * 常用color
 */
enum E_COLOR {
    COLOR_BLACK         = 0x000000,     // 黑
    COLOR_WHITE         = 0xFFFFFF,     // 白
    COLOR_GRAY          = 0x808080,     // 灰
    COLOR_RED           = 0xFF0000,     // 红
    COLOR_LGREEN        = 0x00FF00,     // 浅绿
    COLOR_BLUE          = 0x0000FF,     // 蓝
    COLOR_YELLOW        = 0xFFFF00,     // 黄
    COLOR_FUSCIA        = 0xFF00FF,     // 洋红
    COLOR_AQUA          = 0x00FFFF,     // 青
    COLOR_ORANGE        = 0xFFA500,     // 桔
    COLOR_PINK          = 0xEE82EE,     // 粉
    COLOR_PERPLE        = 0x800080,     // 紫
    COLOR_GREEN         = 0x008000,     // 绿
    COLOR_DBLUE         = 0x000080,     // 深蓝
};

/**
 * 每个像素中 R, G, B 在字节流中的顺序
 */
enum E_RGB_ORDER {
    CO_0xRRGGBB = 0,    // R 在较高位
    CO_0xBBGGRR = 1,    // B 在较高位
};

/**
 * 将Color解析出r、g、b三个通道的值
 * @param rgb   [in]
 * @param r     [out]
 * @param g     [out]
 * @param b     [out]
 * @return
 */
inline void parseRGB(Color rgb, Byte &r, Byte &g, Byte &b) {
    r = getRValue(rgb); g = getGValue(rgb); b = getBValue(rgb);
}

/**
 * 亮度，即彩色转黑白
 * @param  color
 * @return
 */
inline Byte getLuminance(Color color) {
    return (Byte)(getRValue(color) * LUMI_RATIO_R
                + getGValue(color) * LUMI_RATIO_G
                + getBValue(color) * LUMI_RATIO_B + 0.5);
}

/**
 * 加权颜色加法
 * @param  ori      被加数
 * @param  aug      加数
 * @param  weight   权值
 * @return
 */
inline Color colorAdd(Color ori, Color aug, Real weight) {
    Byte ra, ga, ba, ro, go, bo;
    parseRGB(ori, ro, go, bo);
    parseRGB(aug, ra, ga, ba);
    ro = weight * ra + (1 - weight) * ro;
    go = weight * ga + (1 - weight) * go;
    bo = weight * ba + (1 - weight) * bo;
    return RGB(ro, go, bo);
}

/**
 * 颜色乘法
 * @param ori   被乘数
 * @param aug   乘数
 * @return
 */
inline Color colorMultiply(Color ori, Color aug) {
    Byte ra, ga, ba, ro, go, bo;
    parseRGB(ori, ro, go, bo);
    parseRGB(aug, ra, ga, ba);
    int ir = ro + ra - ro * ra / (Real)255;
    int ig = go + ga - go * ga / (Real)255;
    int ib = bo + ba - bo * ba / (Real)255;
    clamp(ir, 0, 255);
    clamp(ig, 0, 255);
    clamp(ib, 0, 255);
    return RGB(ir, ig, ib);
}

/**
 * 每个通道分别拉伸、放缩
 * @param  ori
 * @param  daugR    R通道的放缩比例
 * @param  daugG    G通道的放缩比例
 * @param  daugB    B通道的放缩比例
 * @return
 */
inline Color colorMultiply(Color ori, Real daugR, Real daugG, Real daugB) {
    Byte ro, go, bo;
    parseRGB(ori, ro, go, bo);
    ro = clamp(int(ro * daugR), 0, 255);
    go = clamp(int(go * daugG), 0, 255);
    bo = clamp(int(bo * daugB), 0, 255);
    return RGB(ro, go, bo);
}

/**
 * 颜色放缩
 * @param ori
 * @param daug      放缩比例
 * @return
 */
inline Color colorMultiply(Color ori, Real daug) {
    return colorMultiply(ori, daug, daug, daug);
}

/**
 * 将rgb转换成hsv
 * @param rgb   [in]
 * @param h     [out]
 * @param s     [out]
 * @param v     [out]
 * @return
 */ 
inline void rgb2hsv(const Color rgb, Real &h, Real &s, Real &v) {
    Byte    r = getRValue(rgb),
            g = getGValue(rgb),
            b = getBValue(rgb),
         minc = min3( r, g, b),
         maxc = max3( r, g, b);
    v = ((Real)maxc) / 255;
    float delta = maxc - minc;
    
    if(delta == 0) {
        h = 0; s = 0;  return;
    }
    
    s = delta / maxc;
    
    if( r == maxc ) 
        h = ( g - b ) / delta;        // between yellow & magenta 
    else if( g == maxc ) 
        h = 2 + ( b - r ) / delta;    // between cyan & yellow 
    else 
        h = 4 + ( r - g ) / delta;    // between magenta & cyan 
    h *= 60;
    if( h < 0 ) 
        h += 360; 
}

/**
 * 将hsv转成rgb
 * @param h     [in]
 * @param s     [in]
 * @param v     [in]
 * @return 转换结果
 */
inline Color hsv2rgb(Real h, Real s, Real v) {
    v *= 255;
    if(s == 0) {
        return RGB(v, v, v);
    }

    Color rgb;
    h /= 60;    
    int hi = (int(h)) % 6;
    float f  = h -    hi;
    Byte p = Byte(v * (1 - s));
    Byte q = Byte(v * (1 - f * s));
    Byte t = Byte(v * (1 - (1 - f) * s));
    
    switch(hi) {
    case 0:
        rgb = RGB(v, t, p); 
        break;
    case 1:
        rgb = RGB(q, v, p); 
        break;
    case 2:
        rgb = RGB(p, v, t); 
        break;
    case 3:
        rgb = RGB(p, q, v); 
        break;
    case 4:
        rgb = RGB(t, p, v); 
        break;
    case 5:
        rgb = RGB(v, p, q); 
        break;
    default:
        break;
    }
    return rgb;
}

/**
 * 将颜色转换到hsv，在h/s/v通道上分别调整，再转换回rgb
 * @param c     原颜色
 * @param ah    h通道的调整幅度
 * @param as    s通道的调整幅度
 * @param ab    v通道的调整幅度
 * @return
 */
inline Color hsbAdjust(Color c, Real ah, Real as, Real av) {
    Real h, s, v;
    rgb2hsv(c, h, s, v);
    h += ah; 
    if(h >= 360) {
        h -= 360;
    } else if(h < 0) {
        h += 360;
    }
    s += as;
    if(s < 0) {
        s = 0;
    } else if(s > 1) {
        s = 1;
    }
    v += av;
    if(v < 0) {
        v = 0;
    } else if(v > 1) {
        v = 1;
    }
    return hsv2rgb(h, s, v);
}

/**
 * 获取饱和度：三个通道中，(max-min)/max
 * @param r     颜色的r通道
 * @param g     颜色的g通道
 * @param b     颜色的b通道
 * @return
 */
inline Real getSaturation(Byte r, Byte g, Byte b) {
    Byte minc = min3( r, g, b ),
         maxc = max3( r, g, b );
    return (maxc - minc) / (Real)maxc;
}

/**
 * 设置饱和度：将颜色中hsv中h、v不变，改变s
 * @param c     原颜色
 * @param s     新的s通道值
 * @return
 */
inline Color setSaturation(Color c, Real s) {
    Real oh, os, ov;
    rgb2hsv(c, oh, os, ov);
    return hsv2rgb(oh, s, ov);
}

/**
 * 通过权重设置饱和度：将颜色中hsv中h、v不变，s通道乘以权重w
 * @param c     原颜色
 * @param w     s通道的权重
 * @return
 */
inline Color setSaturationByWeight(Color c, Real w) {
    Real oh, os, ov;
    rgb2hsv(c, oh, os, ov);
    return hsv2rgb(oh, w * os, ov);
}

/**
 * 对向量进行旋转，并放缩到 0 ~ 255 的范围内
 * @param x, y      向量
 * @param angle     角度
 * @return
 */
inline void rotateAndScale(Real &x, Real &y, Real angle) {
    Real x_ = x, y_ = y;
    Real cosa = cos(angle * PI / 180), sina = sin(angle * PI / 180);
    x = x_ * cosa + y_ * sina;
    y = y_ * cosa - x_ * sina;
    // 放缩到0 ~ 255范围
    x /= cosa;
    y /= cosa;
}

/**
 * 线性放缩，相当于一个三段线性函数，0~xr中的某些区间拉伸、某些区间压缩
 * @param x     [in&out]    作为x输入，作为y输出
 * @param x1,y1 [in]        第一个分段点 
 * @param x2,y2 [in]        第二个分段点 
 * @param xr,yr [in]        最后点坐标，区间限定在(0,0) -> (xr, yr) 
 * @return
 */
inline void linearScale(Real &x, Real x1, Real y1, Real x2, Real y2, Real xr, Real yr) {
    if(x < x1)
        x = x * y1 / x1;
    else if(x > x2)
        x = (x * (y1 - yr) + (x1 * xr - yr * y1)) / (x1 - xr);
    else
        x = (x * (y1 - y2) + (x1 * y2 - x2 * y1)) / (x1 - x2);
}

/**
 * 取整(最近的一个整数)
 * @param x
 * @return
 */
inline int round(Real x) {
    return ((x - (int)x >= 0.5) ? int(x) + 1 : int(x));
}

/**
 * 将颜色范围压缩
 * @param c
 * @return
 */
inline Byte lightenDark(Byte c) {
    if(c < 100) {
        return clamp((int)(1.5f * c + 30.0f), 0, 255);
    } else {
        return clamp((int)(0.5f * c + 130.0f), 0, 255);
    }
}

/**
 * 将颜色能量较低者线型增加
 * @param c
 * @return
 */
inline Byte lightShadow(Byte c) {
    if(c < 70) {
        return 0.5f * (70 - c) + c;
    } else {
        return c;
    }
}

/**
 * 将颜色进行圆形变换
 * @param x
 * @return
 */ 
inline Byte circleCurve(Byte x) {
    if(x <= 127) {
        return Byte(127 - sqrt(127 * 127 - x * x));
    } else {
        return Byte(127 + sqrt(127 * 127 - (255 - x) * (255 - x)));
    }
}

/**
 * 将颜色进行正弦变换
 * @param x
 * @return
 */
inline Byte sinCurve(Byte x) {
    return (Byte)clamp((sin(PI * (Real)x / 255.0f - PI / 2) + 1.0f) * 255.0f / 2.0f, 0, 255);
}

/**
 * 降低颜色的对比度
 * @param c
 * @param ratio [0, 1]，1 表示不降低
 * @return
 */
inline Byte decContrast(Byte c, Real ratio) {
    if(c < 70) {
        c = c + (70 - c) * ratio;
    } else if(c > 180) {
        c = c - (c - 180) * ratio;
    }
    return c;
}

