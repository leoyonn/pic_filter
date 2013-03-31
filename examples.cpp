/*****************************************************************************
 * @(#)examples.cpp, 2011-5-13.                                              *
 *                                                                           *
 * @author leo                                                               *
 * @desc �ṩ��PicFilter��ʹ��ʾ��                                           *
 *****************************************************************************/

#include "pic_filter.h"
#include "xinclude/ximage.h"
#include "color_utils.h"
#include <iostream>
using namespace std;

#ifdef   _DEBUG   
    #pragma   comment(lib, "cximagecrtd.lib")
#else   
    #pragma   comment(lib, "cximagecrt.lib")   
#endif  

PicFilter    ps;

/**
 * ����ͼ���ڴ�
 * @params 
 * @return
 */
Byte *loadImageToMem(const char* file, int &width, int &height, int imgType = CXIMAGE_FORMAT_BMP, int channel = 3) {
    // cximage: grb
    Byte *data = null;
    CxImage xImg;
    xImg.Load(file, imgType);
    height = xImg.GetHeight();
    width = xImg.GetWidth();
    data = new Byte [channel * width * height];
    for(int j = 0; j < height; j ++) {
        for(int i = 0; i < width; i ++) {
            if (channel == 1)
                data[j * width + i] = xImg.GetPixelGray(i, j);
            else if(channel == 3) {
                RGBQUAD c = xImg.GetPixelColor(i, j, false);
                data[3 * (j * width + i)] = c.rgbBlue;
                data[3 * (j * width + i) + 1] = c.rgbGreen;
                data[3 * (j * width + i) + 2] = c.rgbRed;
            }
        }
    }
    return data;
}

/**
 * ����һ��Ч��
 * @params 
 * @return
 */
void test(FILTER_TYPE type, const char *infile, const char *outfile) {
    Byte * oriData = null, *resData = null;
    long size = 0;
    int width = 0, height = 0;
    
    oriData = loadImageToMem(infile, width, height);
    resData = new Byte[width * height * 3];
    memset(resData, 0, width * height * 3);

    ps.setData(oriData, resData, width, height, 3);
    ps.doPS(type);

    CxImage xImg;
    xImg.CreateFromArray(resData, width, height, 24, 3 * width, false);
    xImg.SetJpegQuality(90);
    xImg.Save(outfile, CXIMAGE_FORMAT_JPG);

    delete []oriData;    oriData = null;
    delete []resData;    oriData = null;
}

const char* appendName(string fileName, const char* toAppend){
    static string fileAfter = "";
    fileAfter = fileName.substr(0, fileName.length() - 4);
    fileAfter.append(toAppend);
    fileAfter.append(".jpg");
    return fileAfter.data();
}

void testall(string fileName)
{
    // Lomo
    test(LOMO_EX, fileName.data(), appendName(fileName, ".f01.Lomo"));
    // HDR
    test(HDR, fileName.data(), appendName(fileName, ".f02.HDR"));
    // ���
    test(SOFTLIGHT, fileName.data(), appendName(fileName, ".f03.�λ�"));
    // �ڰ�
    test(GRAY_EX, fileName.data(), appendName(fileName, ".f04.�ڰ�"));
    // 80��
    test(THE80S, fileName.data(), appendName(fileName, ".f05.80��"));
    // �۽�
    test(TILTSHIFT, fileName.data(), appendName(fileName, ".f06.����"));
    // �ƽ�����
    test(GOLDEN, fileName.data(), appendName(fileName, ".f07.�ƽ�����"));
    // ����
    test(EARLYBIRD,  fileName.data(), appendName(fileName, ".f08.����"));
    // ��ɫ
    test(FADE, fileName.data(), appendName(fileName, ".f09.��ɫ"));
    // ����
    test(PINK, fileName.data(), appendName(fileName, ".f10.����"));
    // ����
    test(SUMMER, fileName.data(), appendName(fileName, ".f11.����"));
    // ɳ����
    test(SANDSTORM, fileName.data(), appendName(fileName, ".f12.ɳ����"));
    // ��ҹ
    test(NIGHT, fileName.data(), appendName(fileName, ".f13.��ҹ"));
    // �Ŷ�
    test(ANTIQUE, fileName.data(), appendName(fileName, ".f14.��ͭ"));
    // �����ع�
    test(OVEREXPOSE, fileName.data(), appendName(fileName, ".f15.���ع�"));
    // Nashville
    test(NASHVILLE, fileName.data(), appendName(fileName, ".f16.Nashville"));
    //more...
}

void main()
{
    testall("E:/_cps/1.jpg");
    testall("E:/_cps/2.jpg");
    testall("E:/_cps/3.jpg");
    testall("E:/_cps/4.jpg");
}

