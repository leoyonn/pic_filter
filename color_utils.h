/*****************************************************************************
 * @(#)color_utils.cpp, 2011-5-13.                                           *
 *                                                                           *
 * @author leo                                                               *
 * @desc �ṩһЩ�򵥵����ز�����������                                      *
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

// ע�⣺��wingdi�еĶ��岻ͬ������һ��Dword���ǣ�0xRRGGBB�ĸ�ʽ
#define RGB(r, g, b)    ((Color)(((Byte)(b) | ((Word)((Byte)(g)) << 8)) | (((Color)(Byte)(r)) << 16)))
#define RGBF(r, g, b)    (RGB((r) * 255, (g) * 255, (b) * 255))

#define getRValue(rgb)    ((Byte)((rgb) >> 16))
#define getGValue(rgb)    ((Byte)(((Word)(rgb)) >> 8))
#define getBValue(rgb)    ((Byte)(rgb))

/** ��ȡa, b, c����Сֵ */
#define min3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

/** ��ȡa, b, c�����ֵ */
#define max3(a, b, c) ((a) > (b) ? ((a) > (c) ? (a) : (c)) : ((b) > (c) ? (b) : (c)))

/** ��v������a, b֮�� (a <= b) */
#define clamp(v, a, b) ((v) > (b) ? (b) : ((v) < (a) ? (a) : (v)))

typedef unsigned long   Dword;
typedef unsigned short  Word;
typedef unsigned char   Byte;
typedef Dword           Color;
typedef float           Real;

// ��ɫת�ڰ׸�ͨ��������
const Real LUMI_RATIO_R = 0.299;
const Real LUMI_RATIO_G = 0.587;
const Real LUMI_RATIO_B = 0.114;

/**
 * ����color
 */
enum E_COLOR {
    COLOR_BLACK         = 0x000000,     // ��
    COLOR_WHITE         = 0xFFFFFF,     // ��
    COLOR_GRAY          = 0x808080,     // ��
    COLOR_RED           = 0xFF0000,     // ��
    COLOR_LGREEN        = 0x00FF00,     // ǳ��
    COLOR_BLUE          = 0x0000FF,     // ��
    COLOR_YELLOW        = 0xFFFF00,     // ��
    COLOR_FUSCIA        = 0xFF00FF,     // ���
    COLOR_AQUA          = 0x00FFFF,     // ��
    COLOR_ORANGE        = 0xFFA500,     // ��
    COLOR_PINK          = 0xEE82EE,     // ��
    COLOR_PERPLE        = 0x800080,     // ��
    COLOR_GREEN         = 0x008000,     // ��
    COLOR_DBLUE         = 0x000080,     // ����
};

/**
 * ÿ�������� R, G, B ���ֽ����е�˳��
 */
enum E_RGB_ORDER {
    CO_0xRRGGBB = 0,    // R �ڽϸ�λ
    CO_0xBBGGRR = 1,    // B �ڽϸ�λ
};

/**
 * ��Color������r��g��b����ͨ����ֵ
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
 * ���ȣ�����ɫת�ڰ�
 * @param  color
 * @return
 */
inline Byte getLuminance(Color color) {
    return (Byte)(getRValue(color) * LUMI_RATIO_R
                + getGValue(color) * LUMI_RATIO_G
                + getBValue(color) * LUMI_RATIO_B + 0.5);
}

/**
 * ��Ȩ��ɫ�ӷ�
 * @param  ori      ������
 * @param  aug      ����
 * @param  weight   Ȩֵ
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
 * ��ɫ�˷�
 * @param ori   ������
 * @param aug   ����
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
 * ÿ��ͨ���ֱ����졢����
 * @param  ori
 * @param  daugR    Rͨ���ķ�������
 * @param  daugG    Gͨ���ķ�������
 * @param  daugB    Bͨ���ķ�������
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
 * ��ɫ����
 * @param ori
 * @param daug      ��������
 * @return
 */
inline Color colorMultiply(Color ori, Real daug) {
    return colorMultiply(ori, daug, daug, daug);
}

/**
 * ��rgbת����hsv
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
 * ��hsvת��rgb
 * @param h     [in]
 * @param s     [in]
 * @param v     [in]
 * @return ת�����
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
 * ����ɫת����hsv����h/s/vͨ���Ϸֱ��������ת����rgb
 * @param c     ԭ��ɫ
 * @param ah    hͨ���ĵ�������
 * @param as    sͨ���ĵ�������
 * @param ab    vͨ���ĵ�������
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
 * ��ȡ���Ͷȣ�����ͨ���У�(max-min)/max
 * @param r     ��ɫ��rͨ��
 * @param g     ��ɫ��gͨ��
 * @param b     ��ɫ��bͨ��
 * @return
 */
inline Real getSaturation(Byte r, Byte g, Byte b) {
    Byte minc = min3( r, g, b ),
         maxc = max3( r, g, b );
    return (maxc - minc) / (Real)maxc;
}

/**
 * ���ñ��Ͷȣ�����ɫ��hsv��h��v���䣬�ı�s
 * @param c     ԭ��ɫ
 * @param s     �µ�sͨ��ֵ
 * @return
 */
inline Color setSaturation(Color c, Real s) {
    Real oh, os, ov;
    rgb2hsv(c, oh, os, ov);
    return hsv2rgb(oh, s, ov);
}

/**
 * ͨ��Ȩ�����ñ��Ͷȣ�����ɫ��hsv��h��v���䣬sͨ������Ȩ��w
 * @param c     ԭ��ɫ
 * @param w     sͨ����Ȩ��
 * @return
 */
inline Color setSaturationByWeight(Color c, Real w) {
    Real oh, os, ov;
    rgb2hsv(c, oh, os, ov);
    return hsv2rgb(oh, w * os, ov);
}

/**
 * ������������ת���������� 0 ~ 255 �ķ�Χ��
 * @param x, y      ����
 * @param angle     �Ƕ�
 * @return
 */
inline void rotateAndScale(Real &x, Real &y, Real angle) {
    Real x_ = x, y_ = y;
    Real cosa = cos(angle * PI / 180), sina = sin(angle * PI / 180);
    x = x_ * cosa + y_ * sina;
    y = y_ * cosa - x_ * sina;
    // ������0 ~ 255��Χ
    x /= cosa;
    y /= cosa;
}

/**
 * ���Է������൱��һ���������Ժ�����0~xr�е�ĳЩ�������졢ĳЩ����ѹ��
 * @param x     [in&out]    ��Ϊx���룬��Ϊy���
 * @param x1,y1 [in]        ��һ���ֶε� 
 * @param x2,y2 [in]        �ڶ����ֶε� 
 * @param xr,yr [in]        �������꣬�����޶���(0,0) -> (xr, yr) 
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
 * ȡ��(�����һ������)
 * @param x
 * @return
 */
inline int round(Real x) {
    return ((x - (int)x >= 0.5) ? int(x) + 1 : int(x));
}

/**
 * ����ɫ��Χѹ��
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
 * ����ɫ�����ϵ�����������
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
 * ����ɫ����Բ�α任
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
 * ����ɫ�������ұ任
 * @param x
 * @return
 */
inline Byte sinCurve(Byte x) {
    return (Byte)clamp((sin(PI * (Real)x / 255.0f - PI / 2) + 1.0f) * 255.0f / 2.0f, 0, 255);
}

/**
 * ������ɫ�ĶԱȶ�
 * @param c
 * @param ratio [0, 1]��1 ��ʾ������
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

