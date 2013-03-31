/*****************************************************************************
 * @(#)pic_filter_func.cpp, 2011-5-13.                                       *
 *                                                                           *
 * @author leo                                                               *
 * @desc PicFilter的方法定义(滤镜算法实现)                                    *
 *****************************************************************************/

#include "pic_filter.h"
#include <stdio.h>
#include <string.h>

PicFilter::PicFilter(){}

PicFilter::~PicFilter(){}

//////////////////////////////////////////////////////////////////////////

/**
 * 获取一个像素的颜色值
 * @param   x       像素在图像上的横坐标
 * @param   y       像素在图像上的纵坐标
 * @param   data    图像缓存数组
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
 * 设置一个像素的颜色值
 * @param   color   要设置的颜色
 * @param   x       像素在图像上的横坐标
 * @param   y       像素在图像上的纵坐标
 * @param   data    图像缓存数组
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
 * 计算tsize大小的一块方形图像区域中的平均颜色
 * @param   x       像素在图像上的横坐标
 * @param   y       像素在图像上的纵坐标
 * @param   tsize   方形区域的大小
 * @return  平均颜色
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
 * 用computeTileColor计算出的Color设置为整个tile的颜色
 * @param   color       要设置到像素上的颜色
 * @param   upperColor  要根据上一块的颜色亮度决定本块的相对高度
 * @param   x           像素在图像上的横坐标
 * @param   y           像素在图像上的纵坐标
 * @param   tsize       方形区域的大小
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
 * 用computeTileColor计算出的Color设置为整个tile的颜色
 * @param   color       要设置到像素上的颜色
 * @param   upperColor  要根据上一块的颜色亮度决定本块的相对高度
                        TODO：试验，用hsv中的v通道作为权重
 * @param   x           像素在图像上的横坐标
 * @param   y           像素在图像上的纵坐标
 * @param   tsize       方形区域的大小
 * @param   shift       为凸显3D效果的向上位移尺度
 */
void PicFilter::setTileColor3D(Color color, Color upperColor, int x, int y, int tsize, int shift) {
    Real htmp = 0, stmp = 0, vu = 0, v;
    rgb2hsv(upperColor, htmp, stmp, vu);
    rgb2hsv(color, htmp, stmp, v);
    Color ctile;
    if(v - vu > 0)    // 比上一个tile亮，则加白，显示为凸起
        ctile = RGB(0xFF * .2 + getRValue(color) * .8,
                    0xFF * .2 + getGValue(color) * .8,
                    0xFF * .2 + getBValue(color) * .8);
    else            // 否则显示为凹下
        ctile = RGB(0x00 * .2 + getRValue(color) * .8,
                    0x00 * .2 + getGValue(color) * .8,
                    0x00 * .2 + getBValue(color) * .8);
    shift = shift * (30 * fabs(v - vu));

    int j, i;
    for(j = 0; j < tsize && y + j < imgData.height; j ++) {
        for(i = 0; i < tsize && x + i < imgData.width; i ++) {
             // 对shift区域用ctile着色
            if(j <= shift)
                setPixel(ctile, x + i, y + j);
            else
                setPixel(color, x + i, y + j);
        }
    }
    for(j = shift; j < tsize && y + j < imgData.height; j ++) {
        // 对左右边界用灰色着色
        setPixel(0x888888, x + 0, y + j);
        if(x + tsize - 1 < imgData.width)
            setPixel(0x888888, x + tsize - 1, y + j);
    }
}


/**
 * 从文件file中加载纹理。文件格式：第一行：width height；之后 data
 * @param   type    纹理类型
 * @param   file    纹理文件路径
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
 * 设置纹理
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
 * 将纹理保存为文件
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
 * 释放纹理
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
 * 均值滤波
 * @param   tsize   模板大小
 * @param   x       像素在图像上的横坐标
 * @param   y       像素在图像上的纵坐标
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
            // 边界情况
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
 * 将模板应用到像素点
 * @param   gtemp   模板数据
 * @param   tsize   模板大小
 * @param   x       像素在图像上的横坐标
 * @param   y       像素在图像上的纵坐标
 * @return
 */
void PicFilter::applyTemplate(Real *gtemp, int tsize, int x, int y) {
    int m, n;
    Real sumr = 0, sumg = 0, sumb = 0;
    int cnt = 0;
    int x_l = x - (tsize >> 1), y_l = y - (tsize >> 1);
    for(m = 0; m < tsize; m ++) {
        for(n = 0; n < tsize; n ++) {
            // 边界情况
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
 * 直方图均衡化
 * TODO: 只接受bytesPerPixel = 3
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

    // 统计直方图bin的高度
    for (j = 0; j < height; j ++) {
        for (i = 0; i < width; i ++) {
            Color c = getPixel(i, j, data);
            histCount[0][getRValue(c)] ++;
            histCount[1][getGValue(c)] ++;
            histCount[2][getBValue(c)] ++;
        }
    }
    
    Real sizeCoef = 255 / (Real)(height * width);
    // 计算映射表：256->256
    for (i = 0; i < 256; i++) {
        long tmpr = 0, tmpg = 0, tmpb = 0;        
        for (j = 0; j <= i ; j++) {
            tmpr += histCount[0][j];
            tmpg += histCount[1][j];
            tmpb += histCount[2][j];
        }
        
        // 计算对应的新灰度值
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
 * 通过HSV的V通道进行彩色图像直方图均衡化
 * TODO: 只接受bytesPerPixel = 3
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

    
    // 统计直方图bin的高度
    for (j = 0; j < height; j ++) {
        for (i = 0; i < width; i ++) {
            Real h, s, v;
            rgb2hsv(getPixel(i, j, data), h, s, v);
            histCount[Byte(v * 255)] ++;
        }
    }
    
    // 计算映射表：256->256
    for (i = 0; i < 256; i++) {
        long tmp = 0;        
        for (j = 0; j <= i ; j++)
            tmp += histCount[j];
        
        // 计算对应的新v
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
 * 创建反转放缩映射map
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
    // 将旋转后落空的地方（由于取整）填充
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
 * 曲线放缩
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
 * 加大对比度曲线放缩
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
 * 加暗角，weight为暗角浓度；radius为暗角范围
 * @param 
 * @return
 */
void PicFilter::darkenCorners1(Byte *data, Real weight, Real radius) {
    int halfW = imgData.width >> 1, halfH = imgData.height >> 1;
    Real imgR = sqrt(halfW * halfW + halfH * halfH);
    Real darkR = imgR * radius;
    TRAVERSE_IMG_BEGIN {
        Real dx = halfW - x, dy = halfH - y;
        // 离中心点的距离
        Real distToCenter = sqrt(dx * dx + dy * dy);
        Real ddist = imgR * (1.0f - radius);
        // 在暗角中的长度，如果为负值则没到暗角的范围，不计算
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
 * 加暗角，weight为暗角浓度；radius为暗角范围
 * @param   data    [in&out]
 * @param   weight  暗角浓度
 * @param   radius  暗角范围
 * @return
 */ 
void PicFilter::darkenCorners2(Byte *data, Real weight, Real radius) {
    int halfW = imgData.width >> 1, halfH = imgData.height >> 1;
    Real imgR = sqrt(halfW * halfW + halfH * halfH);
    Real darkR = imgR * radius;
    TRAVERSE_IMG_BEGIN {
        Real dx = halfW - x, dy = halfH - y;
        // 离中心点的距离
        Real distToCenter = sqrt(dx * dx + dy * dy); // f1
        Real ddist = imgR * (1.0f - radius); // f4
        // 在暗角中的长度，如果为负值则没到暗角的范围，不计算
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
 * 设置饱和度
 * @param   input
 * @param   output
 * @param   weight (saturaiton)为0 ~ 1，0为灰度图
 * @return
 */
void PicFilter::setSaturation(Byte *input, Byte *output, float weight) {
    TRAVERSE_IMG_BEGIN {
        setPixel(::setSaturationByWeight(getPixel(x, y, input), weight), x, y, output);
    }TRAVERSE_IMG_END
}

/**
 * 对颜色每个通道加权
 * @param data      [in&out]
 * @param daugR     r通道的权重
 * @param daugG     g通道的权重
 * @param daugB     b通道的权重
 * @return
 */
void PicFilter::multiply(Byte *data, Real daugR, Real daugG, Real daugB) {
    TRAVERSE_IMG_BEGIN {
        setPixel(colorMultiply(getPixel(x, y, data), daugR, daugG, daugB), x, y, data);
    } TRAVERSE_IMG_END
}

/**
 * 发黄效果
 * @param   input
 * @param   output
 * @return
 */
void PicFilter::lightShadowWithYellowCast(Byte *input, Byte *output) {
    Byte shift = 0; // 不使用shift
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
 * 对不同通道进行不同的加权(强调红，其次绿，其次蓝)
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
