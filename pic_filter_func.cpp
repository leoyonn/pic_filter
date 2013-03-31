/*****************************************************************************
 * @(#)pic_filter_func.cpp, 2011-5-13.                                       *
 *                                                                           *
 * @author leo                                                               *
 * @desc PicFilter�ķ�������(�˾��㷨ʵ��)                                    *
 *****************************************************************************/

#include "pic_filter.h"
#include <stdio.h>
#include <string.h>

PicFilter::PicFilter(){}

PicFilter::~PicFilter(){}

//////////////////////////////////////////////////////////////////////////

/**
 * ��ȡһ�����ص���ɫֵ
 * @param   x       ������ͼ���ϵĺ�����
 * @param   y       ������ͼ���ϵ�������
 * @param   data    ͼ�񻺴�����
 * @return
 */ 
inline Color PicFilter::getPixel(int x, int y, Byte *data) {
    if(data == null)
        data = imgData.oriData;
    int idx = (y * imgData.width + x) * imgData.bytesPerPixel;
    if(imgData.corder == CO_0xRRGGBB)
        return RGB(data[idx + 2], data[idx + 1], data[idx]);
    else
        return RGB(data[idx], data[idx + 1], data[idx + 2]);
}

/**
 * ����һ�����ص���ɫֵ
 * @param   color   Ҫ���õ���ɫ
 * @param   x       ������ͼ���ϵĺ�����
 * @param   y       ������ͼ���ϵ�������
 * @param   data    ͼ�񻺴�����
 * @return
 */ 
inline void PicFilter::setPixel(Color color, int x, int y, Byte *data) {
    if(data == null)
        data = imgData.resData;
    int idx = (y * imgData.width + x) * imgData.bytesPerPixel;
    if(imgData.corder == CO_0xRRGGBB) {
        data[idx + 2]    = getRValue(color);
        data[idx + 1]    = getGValue(color);
        data[idx]        = getBValue(color);
    } else {
        data[idx]        = getRValue(color);
        data[idx + 1]    = getGValue(color);
        data[idx + 2]    = getBValue(color);
    }
}

/**
 * ����tsize��С��һ�鷽��ͼ�������е�ƽ����ɫ
 * @param   x       ������ͼ���ϵĺ�����
 * @param   y       ������ͼ���ϵ�������
 * @param   tsize   ��������Ĵ�С
 * @return  ƽ����ɫ
 */
Color PicFilter::computeTileColor(int x, int y, int tsize) {
    Real h = 0, s = 0, v = 0;
    Real ra = 0, ga = 0, ba = 0;
    int cnt = 0;
    for(int j = 0; j < tsize && y + j < imgData.height; j ++) {
        for(int i = 0; i < tsize && x + i < imgData.width; i ++) {
            Color c = getPixel(x + i, y + j);
            Byte r, g, b;
            parseRGB(c, r, g, b);
            ra += r; ga += g; ba += b;
            cnt ++;
        }
    }
    ra /= cnt; ga /= cnt; ba /= cnt;
    return RGB(ra, ga, ba);
}

/**
 * ��computeTileColor�������Color����Ϊ����tile����ɫ
 * @param   color       Ҫ���õ������ϵ���ɫ
 * @param   upperColor  Ҫ������һ�����ɫ���Ⱦ����������Ը߶�
 * @param   x           ������ͼ���ϵĺ�����
 * @param   y           ������ͼ���ϵ�������
 * @param   tsize       ��������Ĵ�С
 */
void PicFilter::setTileColor(Color color, Color upperColor, int x, int y, int tsize) {
    Real htmp = 0, stmp = 0, vu = 0, v;
    rgb2hsv(upperColor, htmp, stmp, vu);
    rgb2hsv(color, htmp, stmp, v);
    int tid = TILING;
    if(v - vu > 0.1)
        tid = TILING2;
    else if(v - vu < -0.1)
        tid = TILING3;
    int tw = texData[tid].width, th = texData[tid].height;
    Byte bmid = texData[tid].data[th * th / 2 + tw / 2];
    int j, i;
    for(j = 0; j < tsize && y + j < imgData.height; j ++) {
        for(i = 0; i < tsize && x + i < imgData.width; i ++) {
            Byte btile = texData[tid].data[j * th / tsize * th + i * tw / tsize];
            Color rcolor = color;
            if(btile != bmid) {
                Real w = fabs(btile - bmid) / 255;
                rcolor = RGB(clamp((int)(w * btile + getRValue(color) * (1 - w)), 0, 255),
                             clamp((int)(w * btile + getGValue(color) * (1 - w)), 0, 255),
                             clamp((int)(w * btile + getBValue(color) * (1 - w)), 0, 255));
            }
            setPixel(rcolor, x + i, y + j);
        }
    }
}

/**
 * ��computeTileColor�������Color����Ϊ����tile����ɫ
 * @param   color       Ҫ���õ������ϵ���ɫ
 * @param   upperColor  Ҫ������һ�����ɫ���Ⱦ����������Ը߶�
                        TODO�����飬��hsv�е�vͨ����ΪȨ��
 * @param   x           ������ͼ���ϵĺ�����
 * @param   y           ������ͼ���ϵ�������
 * @param   tsize       ��������Ĵ�С
 * @param   shift       Ϊ͹��3DЧ��������λ�Ƴ߶�
 */
void PicFilter::setTileColor3D(Color color, Color upperColor, int x, int y, int tsize, int shift) {
    Real htmp = 0, stmp = 0, vu = 0, v;
    rgb2hsv(upperColor, htmp, stmp, vu);
    rgb2hsv(color, htmp, stmp, v);
    Color ctile;
    if(v - vu > 0)    // ����һ��tile������Ӱף���ʾΪ͹��
        ctile = RGB(0xFF * .2 + getRValue(color) * .8,
                    0xFF * .2 + getGValue(color) * .8,
                    0xFF * .2 + getBValue(color) * .8);
    else            // ������ʾΪ����
        ctile = RGB(0x00 * .2 + getRValue(color) * .8,
                    0x00 * .2 + getGValue(color) * .8,
                    0x00 * .2 + getBValue(color) * .8);
    shift = shift * (30 * fabs(v - vu));

    int j, i;
    for(j = 0; j < tsize && y + j < imgData.height; j ++) {
        for(i = 0; i < tsize && x + i < imgData.width; i ++) {
             // ��shift������ctile��ɫ
            if(j <= shift)
                setPixel(ctile, x + i, y + j);
            else
                setPixel(color, x + i, y + j);
        }
    }
    for(j = shift; j < tsize && y + j < imgData.height; j ++) {
        // �����ұ߽��û�ɫ��ɫ
        setPixel(0x888888, x + 0, y + j);
        if(x + tsize - 1 < imgData.width)
            setPixel(0x888888, x + tsize - 1, y + j);
    }
}


/**
 * ���ļ�file�м��������ļ���ʽ����һ�У�width height��֮�� data
 * @param   type    ��������
 * @param   file    �����ļ�·��
 * @deprecated
 */
void PicFilter::loadTexture(E_GRAIN_TYPE type, const char *file) {
    FILE * fp = fopen(file, "r");
    if(!fp)return;
    texData->type = type;
    fscanf(fp, "%d %d", &texData[type].width, &texData[type].height);
    int n = texData[type].width * texData[type].height;
    texData[type].data = new Byte [n];
    for(int i = 0; i < n; i ++)
        fscanf(fp, "%d", &texData[type].data[i]);
    fclose(fp);
}

/**
 * ��������
 * @param type
 * @param data
 * @param width
 * @param height
 * @return
 */
void PicFilter::setTexture(E_GRAIN_TYPE type, Byte *data, int width, int height) {
    texData[type].type = type;
    texData[type].data = data;
    texData[type].width = width;
    texData[type].height = height;
}

/**
 * ��������Ϊ�ļ�
 * @param type
 * @param file
 * @return
 */
void PicFilter::saveTexture(E_GRAIN_TYPE type, const char *file) {
    FILE * fp = fopen(file, "w");
    if(!fp)return;
    fprintf(fp, "%d %d\n", texData[type].width, texData[type].height);
    int n = texData[type].width * texData[type].height;
    for(int i = 0; i < n; i ++)
        fprintf(fp, "%d ", texData[type].data[i]);
    fclose(fp);
}

/**
 * �ͷ�����
 * @param type
 * @return
 */
void PicFilter::releaseTexture(E_GRAIN_TYPE type) {
    if(texData[type].data) {
        delete []texData[type].data;
        texData[type].data = null;
    }
}

/**
 * ��ֵ�˲�
 * @param   tsize   ģ���С
 * @param   x       ������ͼ���ϵĺ�����
 * @param   y       ������ͼ���ϵ�������
 * @return
 */
void PicFilter::applyTemplateAve(int tsize, int x, int y)
{
    int m, n;
    int sumr = 0, sumg = 0, sumb = 0;
    int cnt = 0;
    int x_l = x - (tsize >> 1), y_l = y - (tsize >> 1);
    for(m = 0; m < tsize; m ++) {
        for(n = 0; n < tsize; n ++) {
            // �߽����
            if(x_l + m < 0 || y_l + n < 0
                ||  x_l + m >= imgData.width || y_l + n >= imgData.height) {
                continue;
            }
            Color c = getPixel(x_l + m, y_l + n);
            sumr += getRValue(c);
            sumg += getGValue(c);
            sumb += getBValue(c);
            cnt ++;
        }
    }
    
    Real coef = 1 / (Real)cnt;
    sumr = abs((int)(sumr * coef));
    sumg = abs((int)(sumg * coef));
    sumb = abs((int)(sumb * coef));
    clamp((int)sumr, 0, 255);
    clamp((int)sumg, 0, 255);
    clamp((int)sumb, 0, 255);
    
    setPixel(RGB(sumr, sumg, sumb), x, y);
}

/**
 * ��ģ��Ӧ�õ����ص�
 * @param   gtemp   ģ������
 * @param   tsize   ģ���С
 * @param   x       ������ͼ���ϵĺ�����
 * @param   y       ������ͼ���ϵ�������
 * @return
 */
void PicFilter::applyTemplate(Real *gtemp, int tsize, int x, int y) {
    int m, n;
    Real sumr = 0, sumg = 0, sumb = 0;
    int cnt = 0;
    int x_l = x - (tsize >> 1), y_l = y - (tsize >> 1);
    for(m = 0; m < tsize; m ++) {
        for(n = 0; n < tsize; n ++) {
            // �߽����
            if( x_l + m < 0 || y_l + n < 0
                || x_l + m >= imgData.width || y_l + n >= imgData.height) { 
                continue;
            }
            Color c = getPixel(x_l + m, y_l + n);
            Real mult = gtemp[m * tsize + n];
            sumr += getRValue(c) * mult;
            sumg += getGValue(c) * mult;
            sumb += getBValue(c) * mult;
            cnt ++;
        }
    }
    Real coef = tsize * tsize / cnt;
    int isumr = fabs((int)(sumr * coef));
    int isumg = fabs((int)(sumg * coef));
    int isumb = fabs((int)(sumb * coef));
    clamp(isumr, 0, 255);
    clamp(isumg, 0, 255);
    clamp(isumb, 0, 255);

    setPixel(RGB(isumr, isumg, isumb), x, y);
}


/**
 * ֱ��ͼ���⻯
 * TODO: ֻ����bytesPerPixel = 3
 * @param data
 * @param width
 * @param height
 * @param bytesPerPixel 
 * @return
 */ 
void PicFilter::histogramEqualization(Byte *data, int width, int height, int bytesPerPixel) {
    int i, j;
    Byte histMap[3][256] = {0,};
    long histCount[3][256] = {0,};

    // ͳ��ֱ��ͼbin�ĸ߶�
    for (j = 0; j < height; j ++) {
        for (i = 0; i < width; i ++) {
            Color c = getPixel(i, j, data);
            histCount[0][getRValue(c)] ++;
            histCount[1][getGValue(c)] ++;
            histCount[2][getBValue(c)] ++;
        }
    }
    
    Real sizeCoef = 255 / (Real)(height * width);
    // ����ӳ���256->256
    for (i = 0; i < 256; i++) {
        long tmpr = 0, tmpg = 0, tmpb = 0;        
        for (j = 0; j <= i ; j++) {
            tmpr += histCount[0][j];
            tmpg += histCount[1][j];
            tmpb += histCount[2][j];
        }
        
        // �����Ӧ���»Ҷ�ֵ
        histMap[0][i] = (Byte)(tmpr * sizeCoef);
        histMap[1][i] = (Byte)(tmpg * sizeCoef);
        histMap[2][i] = (Byte)(tmpb * sizeCoef);
    }

    for (j = 0; j < height; j ++) {
        for (i = 0; i < width; i ++) {
            Color o = getPixel(i, j, data);
            setPixel(RGB(   histMap[0][getRValue(o)], 
                            histMap[1][getGValue(o)],
                            histMap[2][getBValue(o)]), i, j, data);
        }
    }
}

/**
 * ͨ��HSV��Vͨ�����в�ɫͼ��ֱ��ͼ���⻯
 * TODO: ֻ����bytesPerPixel = 3
 * @param data
 * @param width
 * @param height
 * @param bytesPerPixel 
 * @return
 * @suppresswarning("unchecked")
 */ 
void PicFilter::histogramEqualizationRGB(Byte *data, int width, int height, int bytesPerPixel)
{
    int i, j;
    Byte histMap[256] = {0,};
    long histCount[256] = {0,};

    
    // ͳ��ֱ��ͼbin�ĸ߶�
    for (j = 0; j < height; j ++) {
        for (i = 0; i < width; i ++) {
            Real h, s, v;
            rgb2hsv(getPixel(i, j, data), h, s, v);
            histCount[Byte(v * 255)] ++;
        }
    }
    
    // ����ӳ���256->256
    for (i = 0; i < 256; i++) {
        long tmp = 0;        
        for (j = 0; j <= i ; j++)
            tmp += histCount[j];
        
        // �����Ӧ����v
        histMap[i] = (Byte)(tmp * 255 / (Real)(height * width));
    }
    
    for (j = 0; j < height; j ++) {
        for (i = 0; i < width; i ++) {
            Real h, s, v;
            rgb2hsv(getPixel(i, j, data), h, s, v);
            Real mv = histMap[Byte(v * 255)] / (Real)255;
            setPixel(hsv2rgb(h, s, mv), i, j, data);
        }
    }
}

/**
 * ������ת����ӳ��map
 * @param weight
 * @return
 */
void PicFilter::computeReversalMap(Real weight) {
    this->weight = weight;
    bool bset[256] = {false,};
    memset(emap, 0, 256);
    const int MAG = 16;
    int x;
    for(x = 0;    x < 256; x ++) {
        Real rx = x;
        Real ry = sin((x - 128) / Real(128) * PI) * MAG * weight;
        rotateAndScale(rx, ry, -45);
        if(ry < 0) ry = 0;
        if(ry > 255) ry = 255;
        Byte bx = (Byte)round(rx), by = (Byte)round(ry);
        if(emap[bx] == 0)
        {
            emap[bx] = by;
            bset[bx] = true;
        }
    }
    // ����ת����յĵط�������ȡ�������
    for(x = 0; x < 256; x ++) {
        if(!bset[x]) {
            Byte b1, b2;
            int l = 1;
            while(x - l >= 0 && !bset[x - l]) l ++;
            if(x - l >= 0)
                b1 = emap[x - l];
            else
                b1 = 0;
            l = 1;
            while(x + l < 256 && !bset[x + l]) l ++;
            if(x + l < 256)
                b2 = emap[x + l];
            else
                b2 = 255;
            emap[x] = Byte((Real(b1) + b2) / 2);
            bset[x] = true;
        }
    }
}

/**
 * ���߷���
 * @param input
 * @param output
 * @return
 */
void PicFilter::crossProcessCurve(Byte *input, Byte * output) {
    TRAVERSE_IMG_BEGIN{
        Byte r, g, b;
        parseRGB(getPixel(x, y, input), r, g, b);
        r = clamp((int)((circleCurve(r) + (int)sinCurve(r) * 4) / (Real)5), 0, 255);
        g = clamp((int)((circleCurve(g) + (int)sinCurve(g) * 4) / (Real)5), 0, 255);
        b = clamp((int)((circleCurve(b) + (int)sinCurve(b) * 4) / (Real)5), 0, 255);
        setPixel(RGB(r, g, b), x, y, output);
    } TRAVERSE_IMG_END        
}

/**
 * �Ӵ�Աȶ����߷���
 * @param data
 * @return
 */
void PicFilter::highContrastCurve(Byte *data) {
    Byte r, g, b;
    TRAVERSE_IMG_BEGIN {
        parseRGB(getPixel(x, y, data), r, g, b);
        r = clamp((circleCurve(r) + sinCurve(r) * 8) / 9 + 40, 0, 255);
        g = clamp((circleCurve(g) + sinCurve(g) * 8) / 9 + 40, 0, 255);
        b = clamp((circleCurve(b) + sinCurve(b) * 8) / 9 + 40, 0, 255);
        setPixel(RGB(r, g, b), x, y, data);
    } TRAVERSE_IMG_END
}

/**
 * �Ӱ��ǣ�weightΪ����Ũ�ȣ�radiusΪ���Ƿ�Χ
 * @param 
 * @return
 */
void PicFilter::darkenCorners1(Byte *data, Real weight, Real radius) {
    int halfW = imgData.width >> 1, halfH = imgData.height >> 1;
    Real imgR = sqrt(halfW * halfW + halfH * halfH);
    Real darkR = imgR * radius;
    TRAVERSE_IMG_BEGIN {
        Real dx = halfW - x, dy = halfH - y;
        // �����ĵ�ľ���
        Real distToCenter = sqrt(dx * dx + dy * dy);
        Real ddist = imgR * (1.0f - radius);
        // �ڰ����еĳ��ȣ����Ϊ��ֵ��û�����ǵķ�Χ��������
        Real cornerLen = distToCenter - ddist;
        if(cornerLen <= 0){
            continue;
        }
        
        Byte r, g, b;
        parseRGB(getPixel(x, y, data), r, g, b);
        Real ratioDark = cornerLen / darkR;
        Real ratioWDark = 1 - ratioDark * ratioDark * weight;
        r = r * ratioWDark;
        g = g * ratioWDark;
        b = b * ratioWDark;
        setPixel(RGB(r, g, b), x, y, data);
    } TRAVERSE_IMG_END
}

/**
 * �Ӱ��ǣ�weightΪ����Ũ�ȣ�radiusΪ���Ƿ�Χ
 * @param   data    [in&out]
 * @param   weight  ����Ũ��
 * @param   radius  ���Ƿ�Χ
 * @return
 */ 
void PicFilter::darkenCorners2(Byte *data, Real weight, Real radius) {
    int halfW = imgData.width >> 1, halfH = imgData.height >> 1;
    Real imgR = sqrt(halfW * halfW + halfH * halfH);
    Real darkR = imgR * radius;
    TRAVERSE_IMG_BEGIN {
        Real dx = halfW - x, dy = halfH - y;
        // �����ĵ�ľ���
        Real distToCenter = sqrt(dx * dx + dy * dy); // f1
        Real ddist = imgR * (1.0f - radius); // f4
        // �ڰ����еĳ��ȣ����Ϊ��ֵ��û�����ǵķ�Χ��������
        Real cornerLen = distToCenter - ddist; // f5

        Byte r, g, b;
        parseRGB(getPixel(x, y, data), r, g, b);
        Real ratioDark = cornerLen / darkR; // f7
        Real ratioWDark = 1 - ratioDark * weight;
        r = clamp((int)clamp((int)(1.3f * r), 0, 255) * ratioWDark, 0, 255);
        g = clamp((int)clamp((int)(1.3f * g), 0, 255) * ratioWDark, 0, 255);
        b = clamp((int)clamp((int)(1.3f * b), 0, 255) * ratioWDark, 0, 255);
        setPixel(RGB(r, g, b), x, y, data);
    } TRAVERSE_IMG_END
}

/**
 * ���ñ��Ͷ�
 * @param   input
 * @param   output
 * @param   weight (saturaiton)Ϊ0 ~ 1��0Ϊ�Ҷ�ͼ
 * @return
 */
void PicFilter::setSaturation(Byte *input, Byte *output, float weight) {
    TRAVERSE_IMG_BEGIN {
        setPixel(::setSaturationByWeight(getPixel(x, y, input), weight), x, y, output);
    }TRAVERSE_IMG_END
}

/**
 * ����ɫÿ��ͨ����Ȩ
 * @param data      [in&out]
 * @param daugR     rͨ����Ȩ��
 * @param daugG     gͨ����Ȩ��
 * @param daugB     bͨ����Ȩ��
 * @return
 */
void PicFilter::multiply(Byte *data, Real daugR, Real daugG, Real daugB) {
    TRAVERSE_IMG_BEGIN {
        setPixel(colorMultiply(getPixel(x, y, data), daugR, daugG, daugB), x, y, data);
    } TRAVERSE_IMG_END
}

/**
 * ����Ч��
 * @param   input
 * @param   output
 * @return
 */
void PicFilter::lightShadowWithYellowCast(Byte *input, Byte *output) {
    Byte shift = 0; // ��ʹ��shift
    TRAVERSE_IMG_BEGIN {
        Byte r, g, b;
        parseRGB(getPixel(x, y, input), r, g, b);
        r = clamp((int)decContrast(r, 0.5f) + shift, 0, 255);
        g = clamp((int)decContrast(g, 0.5f) + shift, 0, 255);
        b = clamp((int)decContrast(b, 0.5f) + shift, 0, 255);
        r = clamp((int)lightShadow(r) + 65, 0, 255);
        g = clamp((int)lightShadow(g) + 75, 0, 255);
        b = clamp((int)lightShadow(b) - 10, 0, 255);
        setPixel(RGB(r, g, b), x, y, output);
    } TRAVERSE_IMG_END
}

/**
 * �Բ�ͬͨ�����в�ͬ�ļ�Ȩ(ǿ���죬����̣������)
 * @param 
 * @return
 */
void PicFilter::sepiaCast(Byte * data) {
    TRAVERSE_IMG_BEGIN {
        Byte r, g, b;
        parseRGB(getPixel(x, y, data), r, g, b);
        r = clamp((int)r * 1 + 50, 0, 255);
        g = clamp((int)g * 0.85 + 30, 0, 255);
        b = clamp((int)b *0.75 + 20, 0, 255);
        setPixel(RGB(r, g, b), x, y, data);
    } TRAVERSE_IMG_END
}
