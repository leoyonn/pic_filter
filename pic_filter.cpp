/*****************************************************************************
 * @(#)pic_filter.cpp, 2011-5-13.                                            *
 *                                                                           *
 * @author leo                                                               *
 * @desc PicFilter�ķ�������(�˾��㷨ʵ��)                                   *
 *****************************************************************************/

#include "pic_filter.h"
#include <iostream.h>
#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * �˾��������
 * @param type �˾����ͣ�{@see FILTER_TYPE}
 * @return
 */
void PicFilter::doFilter(FILTER_TYPE type) {
    switch(type)
    {
    case LOMO_EX:       // [Lomo] (�°�)
        doLomoEx();
        break;
    case HDR:           // [HDR]
        doHDR();
        break;
    case SOFTLIGHT:     // [�λ�] (ԭ�������)
        doSoftLight(3, 1, 1);
        break;
    case GRAY_EX:       // [�ڰ�] (�°�)
        doBlackAndWhite();
        break;
    case THE80S:        // [80��]
        doThe80s();
        break;
    case TILTSHIFT:     // [����] (ԭ�����۽�)
        doTiltShift(0.7, 0.9);
        break;
    case GOLDEN:        // [�ƽ�����]  
        doGolden();
        break;
    case EARLYBIRD:     // [����] (�°�) 
        doEarlyBirds();
        break;
    case FADE:          // [��ɫ] 
        doFade();
        break;
    case PINK:          // [����] 
        doPink();
        break;
    case SUMMER:        // [����] 
        doSummer();
        break;
    case SANDSTORM:     // [ɳ����] 
        doSandStorm();
        break;
    case NIGHT:         // [��ҹ] 
        doNight();
        break;
    case ANTIQUE:       // [��ͭ] (ԭ��������Ƭ) 
        doAntique();
        break;
    case OVEREXPOSE:    // [���ع�] 
        doOverExposure();
        break;
    case NASHVILLE:     // [Nashville] 
        doNashvilleCurve();
        break;
    case GRAY:          // [�ڰ�](�ɰ�)
        doGray();
        break;
    default:                        // ����Ч�������������(��Щ��Ҫ����)
        break;
    }
}

/**
 * ���ò������ڵ����κ�doPSǰ��Ҫ����������
 * @param   in              ָ���������ػ�����
 * @param   out             ָ��������ػ�����
 * @param   width           ���
 * @param   height          �߶�
 * @param   bytesPerPixel   ÿ�����ص��ֽ������ݽ�֧��3
 * @param   corder          ͨ��˳�� @see E_RGB_ORDER
 * @return
 */
void PicFilter::setData(Byte *in, Byte *out, int width, int height, int bytesPerPixel, E_RGB_ORDER corder) {
    imgData.oriData = in;
    imgData.resData = out;
    imgData.width = width;
    imgData.height = height;
    imgData.bytesPerPixel = bytesPerPixel;
    imgData.nBytes = width * height * bytesPerPixel * sizeof(Byte);
    imgData.corder = corder;
}

/**
 * LomoЧ��������
 * @param 
 * @return
 */
void PicFilter::doLomoEx() {
    Real weight = .8f, radius = .8f;
    TRAVERSE_IMG_BEGIN{
        Byte r, g, b;
        parseRGB(getPixel(x, y, imgData.oriData), r, g, b);
        r = clamp((int)((circleCurve(r) + (Real)sinCurve(r) * 4) / 5), 0, 255);
        g = clamp((int)((circleCurve(g) + (Real)sinCurve(g) * 9) / 10), 0, 255);
        b = clamp((int)((circleCurve(b) + (Real)sinCurve(b) * 4) / 5), 0, 255);
        setPixel(RGB(r, g, b), x, y, imgData.resData);
    } TRAVERSE_IMG_END
    darkenCorners2(imgData.resData, weight, radius);
    multiply(imgData.resData, 1.0, 1.1, 1.2);
}

/**
 * HDRЧ��
 * @param 
 * @return
 */
void PicFilter::doHDR() {
    Real weight = .3; Real radius = .5;
    crossProcessCurve(imgData.oriData, imgData.resData);
    darkenCorners1(imgData.resData, weight, radius);
}

/**
 * �ڰ�Ч��������
 * @param 
 * @return
 */
void PicFilter::doBlackAndWhite() {
    Real weight = .9, radius = .8;
    setSaturation(imgData.oriData, imgData.resData, 0.0f);
    highContrastCurve(imgData.resData);
    darkenCorners1(imgData.resData, weight, radius);
}

/**
 * 80��Ч��
 * @param 
 * @return
 */
void PicFilter::doThe80s() {
    crossProcessCurve(imgData.oriData, imgData.resData);
    doNashvilleCurve();
    darkenCorners1(imgData.resData, 0.6f, 0.5f);
}

/**
 * �ƽ�ʱ��Ч��
 * @param 
 * @return
 */
void PicFilter::doGolden() {
    TRAVERSE_IMG_BEGIN{
        Byte r, g, b;
        parseRGB(getPixel(x, y), r, g, b);
        Real dr = lightenDark(r), dg = lightenDark(g), db = lightenDark(b);
        r = clamp((int)(((Real)circleCurve(dr) + sinCurve(dr) * 8) / 9), 0, 255);
        g = clamp((int)(((Real)circleCurve(dg) + sinCurve(dg) * 8) / 9), 0, 255);
        b = clamp((int)(0.8f * db + 15.0f), 0, 255);
        setPixel(RGB(r, g, b), x, y);
    } TRAVERSE_IMG_END
}

/**
 * ����Ч��
 * @param 
 * @return
 */
void PicFilter::doEarlyBirds() {
    crossProcessCurve(imgData.oriData, imgData.resData);
    Byte shift = 0; // ����shift
    TRAVERSE_IMG_BEGIN {
        Byte r, g, b;
        parseRGB(getPixel(x, y, imgData.resData), r, g, b);
        r = clamp((int)decContrast(r, 0.5f) + shift, 0, 255);
        g = clamp((int)decContrast(g, 0.5f) + shift, 0, 255);
        b = clamp((int)decContrast(b, 0.5f) + shift, 0, 255);
        setPixel(RGB(r, g, b), x, y, imgData.resData);
    } TRAVERSE_IMG_END
    setSaturation(imgData.resData, imgData.resData, 0.5f);
    darkenCorners1(imgData.resData, 0.9f, 0.8f);
}

/**
 * ��ɫЧ��
 * @param 
 * @return
 */
void PicFilter::doFade() {
    TRAVERSE_IMG_BEGIN {
        setPixel(hsbAdjust(getPixel(x, y, imgData.oriData),
            -0.3f * 360 / (2 * PI), -0.6f, 0.1f), x, y, imgData.resData);        
    } TRAVERSE_IMG_END
    darkenCorners1(imgData.resData, 0.6f, 0.7f);
}

/**
 * ����Ч��
 * @param 
 * @return
 */
void PicFilter::doPink()
{
    TRAVERSE_IMG_BEGIN {
        Byte r, g, b;
        parseRGB(getPixel(x, y, imgData.oriData), r, g, b);        
        r = clamp((int)decContrast(r, 0.5f) * 0.7f + 76.0f, 0, 255);
        g = clamp((int)decContrast(g, 0.5f), 0, 255);
        b = clamp((int)decContrast(b, 0.5f) * 0.8f + 36.0f, 0, 255);
        setPixel(RGB(r, g, b), x, y, imgData.resData);
    } TRAVERSE_IMG_END
    darkenCorners1(imgData.resData, 0.6f, 0.7f);
}

/**
 * ����Ч��
 * @param 
 * @return
 */
void PicFilter::doSummer() {
    crossProcessCurve(imgData.oriData, imgData.resData);
    setSaturation(imgData.resData, imgData.resData, 0.5f);
    darkenCorners2(imgData.resData, 0.7f, 0.7f);
    sepiaCast(imgData.resData);
}

/**
 * ɳ����Ч��
 * @param 
 * @return
 */
void PicFilter::doSandStorm() {
    TRAVERSE_IMG_BEGIN {
        Byte r, g, b;
        parseRGB(getPixel(x, y, imgData.oriData), r, g, b);
        r = clamp((int)r * 1 + 30, 0, 255);
        g = clamp((int)g * 0.85 + 20, 0, 255);
        b = clamp((int)b *0.75 + 10, 0, 255);
        
        r = clamp((int)decContrast(r, 0.5f) + 55, 0, 255);
        g = clamp((int)decContrast(g, 0.5f) + 30, 0, 255);
        b = clamp((int)decContrast(b, 0.5f) - 10, 0, 255);
        setPixel(RGB(r, g, b), x, y, imgData.resData);
    } TRAVERSE_IMG_END
    darkenCorners1(imgData.resData, 0.7f, 0.5f);
}

/**
 * ��ҹЧ��
 * @param 
 * @return
 */
void PicFilter::doNight() {
    setSaturation(imgData.oriData, imgData.resData, 0.15);
    crossProcessCurve(imgData.resData, imgData.resData);
    darkenCorners1(imgData.resData, 0.6f, 1.9f);
}

/**
 * �Ŷ�Ч��
 * @param 
 * @return
 */
void PicFilter::doAntique() {
    do1Color(0xaa5500, false);
}

/**
 * ���ع�Ч��
 * @param 
 * @return
 */
void PicFilter::doOverExposure() {
    Real ratio = 2;
    TRAVERSE_IMG_BEGIN {
        Byte r, g, b;
        parseRGB(getPixel(x, y, imgData.oriData), r, g, b);
        r = clamp((int)r * ratio, 0, 255);
        g = clamp((int)g * ratio, 0, 255);
        b = clamp((int)b * ratio, 0, 255);
        setPixel(RGB(r, g, b), x, y, imgData.resData);
    } TRAVERSE_IMG_END
}

/**
 * NashvilleЧ��
 * @param 
 * @return
 */
void PicFilter::doNashvilleCurve() {
    TRAVERSE_IMG_BEGIN {
        Byte r, g, b;
        parseRGB(getPixel(x, y, imgData.oriData), r, g, b);
        r = lightenDark(r);
        g = lightenDark(g);
        b = lightenDark(b);
        
        r = (circleCurve(r) + (int)sinCurve(r) * 4) / 5;
        if (g < 130)
            g = clamp((int)((Real)g * 0.9f + 10.0f), 0, 255);
        b = clamp((int)((Real)b * 0.8f + 10.0f), 0, 255);
        setPixel(RGB(r, g, b), x, y, imgData.resData);
    } TRAVERSE_IMG_END
}

/**
 * ����Ч��
 * @param 
 * @return
 */
void PicFilter::doLily() {
    lightShadowWithYellowCast(imgData.oriData, imgData.resData);
}


/**
 * �ڰ�Ч��(�ɰ�)
 * @param histEqual �Ƿ����ֱ��ͼ���⻯
 * @return
 */
void PicFilter::doGray(bool histEqual) {
    TRAVERSE_IMG_BEGIN
    {
        Color color = getPixel(x, y);
        Byte lumi = getLuminance(color);
        Color cgray    = RGB(lumi, lumi, lumi);
        setPixel(cgray, x, y);
    } TRAVERSE_IMG_END

    if(histEqual) {
        histogramEqualization(imgData.resData, imgData.width, imgData.height,imgData.bytesPerPixel);
    }
}


/**
 * ��ֵ��Ч��
 * @param   threshold ���ݴ���ֵ���ж�ֵ��
 *          TODO: �������������Զ�����ֵѡ��ֱ��ͼ
 * @return
 */
void PicFilter::doBinary(Real threshold) {
    Byte mid = Byte(threshold * 255);
    TRAVERSE_IMG_BEGIN {
        Color color = getPixel(x, y);
        Byte lumi = getLuminance(color);
        if(lumi > mid)
            setPixel(0xFFFFFF, x, y);
        else
            setPixel(0x0, x, y);
        Color cgray = RGB(lumi, lumi, lumi);
    } TRAVERSE_IMG_END
}

/**
 * ��ɫЧ��
 * @param   color       ָ����ɫ
 * @param   histEqual   �Ƿ�ʹ��ֱ��ͼ���⻯
 * @return
 */
void PicFilter::do1Color(Color color, bool histEqual) {
    if(histEqual)
        histogramEqualization(imgData.oriData, imgData.width, imgData.height,imgData.bytesPerPixel);
    Real hc = 0, sc = 0, vc = 1,
         ho = 0, so = 0, vo = 1;
    rgb2hsv(color, hc, sc, vc);
    TRAVERSE_IMG_BEGIN {
        Color oricolor = getPixel(x, y);
        // �����Ż�һ�£�����Ҫh��s�����Բ���rgb2hsv
        // rgb2hsv(oricolor, ho, so, vo);
        vo = getLuminance(oricolor) / (Real)255;
        Color c1 = hsv2rgb(hc, (vo > .6 ? (1 - vo) : .4), vo);
        setPixel(c1, x, y);
    }
    TRAVERSE_IMG_END
}

/**
 * ����Ч��
 * TODO: ������Ч��
 * @param   gtype   ��������
 * @param   weight  ʩ�����������
 * @param   scale   ���������С
 * @param   deltaH  ����ļ��
 * @return
 * @deprecated
 */
void PicFilter::doGrain(E_GRAIN_TYPE gtype, Real weight, Real scale, int deltaH) {
    if(!texData[gtype].data || deltaH > imgData.height){ 
        return;
    }
    TRAVERSE_IMG_BEGIN {
        Color oricolor = getPixel(x, y);
        int tx = x % (int)(texData[gtype].width * scale) / scale,
            ty = y % (int)(texData[gtype].height * scale) / scale;
        Byte texcolor = texData[gtype].data[ty * texData[gtype].width + tx];
        Real wt = texcolor / (Real)128;
        Byte r, g, b;
        parseRGB(oricolor, r, g, b);
        
        r  = clamp((int)(wt * weight * r + (1 - weight) * r), 0, 255);
        g  = clamp((int)(wt * weight * g + (1 - weight) * g), 0, 255);
        b  = clamp((int)(wt * weight * b + (1 - weight) * b), 0, 255);
        setPixel(RGB(r, g, b), x, y);
    } TRAVERSE_IMG_END
}

/**
 * ɨ����Ч��
 * @param   color       scanline����ɫ
 * @param   lineWidth   scanline�Ŀ�ȣ�����Ϊ��λ
 * @param   weight      scanline����ɫȨ�أ�1Ϊȫɫ��0Ϊ������0.5��͸��
 * @param   density     scanline���ܶȣ�density = lineWidth / deltaLineWidth���������߼�ľ������߿��
 * @return
 */
void PicFilter::doScanLine(Color color, Real lineWidth, Real weight, Real density) {
    int deltaH = int(lineWidth / density);
    if(deltaH > imgData.height) {
        return;
    }
    TRAVERSE_IMG_BEGIN {
        Color oricolor = getPixel(x, y);
        if(y % deltaH >= 0 && y % deltaH < lineWidth) {
            Color rcolor = RGB(
                getRValue(color) * weight + getRValue(oricolor) * (1 - weight),
                getGValue(color) * weight + getGValue(oricolor) * (1 - weight),
                getBValue(color) * weight + getBValue(oricolor) * (1 - weight));
            setPixel(rcolor, x, y);
        } else {
            setPixel(oricolor, x, y);
        }
    } TRAVERSE_IMG_END
}

/**
 * ������Ч��
 * @param   tsize   ɫ���С
 * @param   shift   ��ά�����˵�ɫ��߶�
 * @return
 */
void PicFilter::doTiling(int tsize, int shift) {
    TRAVERSE_IMG_BEGIN {
        // ���ȣ�����һ��tile��ȡ����ɫ���ԱȽ�����ĸ߶�
        Color cupper;
        if(y == 0)
            cupper = 0;
        else    // [x+1, y-2] �������ϸ�����δ����ɸ߹����Ӱ��ɫ�����򣨱߽�Ϊ1��
            cupper = getPixel(x + 1, y - 2, imgData.resData);
        Color c = computeTileColor(x, y, tsize);
        setTileColor(c, cupper, x, y, tsize);
    } TRAVERSE_IMG_END 
}

/**
 * LOMOЧ��(��)
 * @param   color           ������ɫ
 * @param   ratio           ���Ǵ�С
 * @param   weight          ��������
 * @param   noiseWeight     ��������
 * @return
 */
void PicFilter::doLomo(Color color, Real ratio, Real weight, Real noiseWeight) {
    int cx = imgData.width / 2,
        cy = imgData.height / 2;
    Real a2 = cx * cx * 2, b2 = cy * cy * 2;
    Real r1 = 1 - ratio, r2 = 1;
    Real realNW = noiseWeight * noiseWeight;
    int genNoise = 1 / realNW;
    srand(time(0));
    TRAVERSE_IMG_BEGIN {
        Color oricolor = getPixel(x, y);
        Real dist = sqrt((x - cx) * (x - cx) / a2 + (y - cy) * (y - cy) / b2);
        oricolor = colorAdd(oricolor, color, dist * dist * weight);
        if(!(rand() % genNoise))
            oricolor = colorMultiply(oricolor, 1 - realNW);
        if(dist < r1)
            setPixel(oricolor, x, y);
        else {
            Real ww = (dist - r1) / (r2 - r1) * weight;
            if(ww > weight) ww = weight;
            Color rcolor = RGB(
                0 * ww + getRValue(oricolor) * (1 - ww),
                0 * ww + getGValue(oricolor) * (1 - ww),
                0 * ww + getBValue(oricolor) * (1 - ww));
            setPixel(rcolor, x, y);
        }
    } TRAVERSE_IMG_END
}

/**
 * ����Ч��
 * @param   ratio   ģ������
 * @param   weight  ģ������
 * @return
 */
void PicFilter::doTiltShift(Real ratio, Real weight) {
    // ��ֵ�˲�����3 - 15��ģ���С
    const int tmin = 3, tmax = 9;
    int cx = imgData.width / 2,
        cy = imgData.height / 2;
    Real a2 = cx * cx * 2, b2 = cy * cy * 2;
    Real r1 = 1 - ratio, r2 = 1;
    TRAVERSE_IMG_BEGIN {
        Color oricolor = getPixel(x, y);
        Real dist = sqrt((x - cx) * (x - cx) / a2 + (y - cy) * (y - cy) / b2);
        if(dist < r1)
            setPixel(oricolor, x, y);
        else {
            int tsize = int((dist - r1) / (r2 - r1) * tmax + 1);
            if(!(tsize & 1)) tsize ++;
            if( tsize > tmax) tsize = tmax;
            // TODO: ���ﲻ�����Ż�����Ϊtsize�Ǳ仯��
            applyTemplateAve(tsize, x, y);
        }
    } TRAVERSE_IMG_END
}

void PicFilter::doLegend(Real weight){}

void PicFilter::doOldFilm(Color color, Real weight, Real density){}

/**
 * ��ƬЧ��
 * @param 
 * @return
 */
void PicFilter::doReverse() {
    TRAVERSE_IMG_BEGIN {
        Color oricolor = getPixel(x, y);
        Color cr = RGB( 255 - getRValue(oricolor),
                        255 - getGValue(oricolor),
                        255 - getBValue(oricolor));
        setPixel(cr, x, y);
    } TRAVERSE_IMG_END
}

void PicFilter::doGallery(Color color, Real weight){}

void PicFilter::doNoise(Real weight, Real density){}

/**
 * �ͻ�Ч��
 * @param   radius      �ͻ����뾶
 * @param   histEqual   �Ƿ�ʹ��ֱ��ͼ����
 * @return
 */
void PicFilter::doPainting(int radius, bool histEqual) {
    if(histEqual)
        histogramEqualization(imgData.oriData, imgData.width, imgData.height,imgData.bytesPerPixel);
    srand(time(0));
    TRAVERSE_IMG_BEGIN {
        // find random color in :radius:
        int pidx = -INFINITE, pidy = -INFINITE;
        do {
            pidx = x + rand() % (radius * 2 + 1) - radius,
                pidy = y + rand() % (radius * 2 + 1) - radius;
        } while(pidx < 0 || pidx >= imgData.width || pidy < 0 || pidy >= imgData.height);
        Color crand = getPixel(pidx, pidy);
        setPixel(crand, x, y);
    } TRAVERSE_IMG_END
}

/**
 * ��תƬЧ��
 * @param   weight  ����
 * @return
 */
void PicFilter::doReversalFilm(Real weight)
{
    if(weight != this->weight)
        computeReversalMap(weight);
    
    TRAVERSE_IMG_BEGIN {
        Color c = getPixel(x, y);    
        Byte r = getRValue(c), g = getGValue(c), b = getBValue(c);
        r = emap[r]; b = emap[b]; b = emap[b];
/*
        // TODO: ��vͨ��Ҳ�任Ч����̫���ԣ����ǵ��������أ��Ȳ�Ҫ
        Real h, s, v;
        rgb2hsv(RGB(r, g, b), h, s, v);
        v = emap[Byte(v * 255)] / (Real)255;
        c = hsv2rgb(h, s, v);
        setPixel(c, x, y);
*/
        setPixel(RGB(r, g, b), x, y);
    } TRAVERSE_IMG_END
}

/**
 * ���Ч��
 * @param 
 * @return
 */
void PicFilter::doSoftLight(Real radius, Real weight, Real contrast)
{
    if(weight != this->weight)
        computeReversalMap(weight);
    TRAVERSE_IMG_BEGIN {
        Color co = getPixel(x, y);
        applyTemplateAve(radius * 2 + 1, x, y);
        Color cm = getPixel(x, y, imgData.resData);
        Byte ro = getRValue(co), go = getGValue(co), bo = getBValue(co),
             rm = getRValue(cm), gm = getGValue(cm), bm = getBValue(cm);

        // ���Է���
        //    linearScale(rm, 0.2, 0.1, 0.8, 0.9, 1, 1);
        //    linearScale(gm, 0.2, 0.1, 0.8, 0.9, 1, 1);
        //    linearScale(bm, 0.2, 0.1, 0.8, 0.9, 1, 1);
        
        // ���߷���
        rm = emap[rm]; gm = emap[gm]; bm = emap[bm];
        
        // �˷����
        int r = ro + rm - ro * rm / 255;
        int g = go + gm - go * gm / 255;
        int b = bo + bm - bo * bm / 255;
        r = clamp((r), 0, 255);
        g = clamp((g), 0, 255);
        b = clamp((b), 0, 255);
        setPixel(RGB(r, g, b), x, y);
    } TRAVERSE_IMG_END
}

/**
 * ģ��Ч��
 * @param   ƽ��ģ���С
 * @return
 */
void PicFilter::doSmoothAve(int tsize) {
    TRAVERSE_IMG_BEGIN {
        applyTemplateAve(tsize, x, y);
    } TRAVERSE_IMG_END
}
