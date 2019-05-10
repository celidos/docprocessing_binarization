#include <CImg.h>
#include <iostream>
#include <stdio.h>
#include <cmath>
#include <string>

using namespace std;
using namespace cimg_library;

void mypause ( void ) 
{ 
  printf ( "Press [Enter] to continue . . ." );
  fflush ( stdout );
  getchar();
} 

void cumsum(CImg<unsigned char> img, CImg<long long int> &cumsum, CImg<long long int> &cumsum2)
{
    cimg_forXY(img, x, y) {
        cumsum(x, y, 0, 0) = img(x, y, 0, 0);
        cumsum2(x, y, 0, 0) = img(x, y, 0, 0) * img(x, y, 0, 0);
        if (x > 0) {
            cumsum(x, y, 0, 0) += cumsum(x - 1, y, 0, 0);
            cumsum2(x, y, 0, 0) += cumsum2(x - 1, y, 0, 0);
        }
        if (y > 0) {
            cumsum(x, y, 0, 0) += cumsum(x, y - 1, 0, 0);
            cumsum2(x, y, 0, 0) += cumsum2(x, y - 1, 0, 0);
        }
        if (x > 0 && y > 0) {
            cumsum(x, y, 0, 0) -= cumsum(x - 1, y - 1, 0, 0);
            cumsum2(x, y, 0, 0) -= cumsum2(x - 1, y - 1, 0, 0);
        }
    }
}

float getMean(CImg<unsigned char> img)
{
    long long int sum = 0;
    cimg_forXY(img, x, y) {
        sum += img(x, y, 0, 0);
    }
    return sum * 1.0f / (img.height() * 1ll * img.width());
}

float getStdev(CImg<unsigned char> img)
{
    float mean = getMean(img);
    float sum = 0.0;
    cimg_forXY(img, x, y) {
        sum += (img(x, y, 0, 0) - mean) * (img(x, y, 0, 0) - mean);
    }
    return sqrt(sum * 1.0f / (img.height() * 1ll * img.width()));
}

long long int getat(CImg<long long int> *data, int x, int y)
{
    if (x >= 0 && y >= 0)
        return (*data)(x, y, 0, 0);
    return 0;
}

int clip(int v, int min, int max)
{
    if (v < min)
        return min;
    if (v > max)
        return max;
    return v;
}

float getWindowMean(CImg<long long int> *data, int x, int y, int window, long long int *_N = nullptr)
{
    int h = data->height(),
        w = data->width();
    
    int x1 = clip(x + window,     -1, w - 1);
    int y1 = clip(y + window,     -1, h - 1);
    int x2 = clip(x - window - 1, -1, w - 1);
    int y2 = clip(y - window - 1, -1, h - 1);

    long long int N = (x2 - x1) * (y2 - y1);
    if (_N)
        *_N = N;
    if (N > 0)
        return (getat(data, x1, y1) + getat(data, x2, y2) - getat(data, x2, y1) - getat(data, x1, y2)) / (N * 1.0f);
    else
        return 0.0;
}

float getWindowStdev(CImg<long long int> *data, CImg<long long int> *meandata, int x, int y, int window)
{
    long long int N;
    float windowStd = getWindowMean(data, x, y, window, &N);
    float windowMean = getWindowMean(meandata, x, y, window);
    if (N > 1)
    {
        return sqrt((windowStd * N - windowMean * windowMean * N) / (1.0f * N - 1.0));
    }
    else
        return 0;
}

CImg<long long int> binarizeNiblack(CImg<long long int> img, float t=1, float w=1, float k=-1.0)
{
    int height = img.height(),
        width = img.width();

    CImg<long long int> integralimg(width, height, 1, 1, 0);
    CImg<long long int> integral2img(width, height, 1, 1, 0);

    cumsum(img, integralimg, integral2img);
    
    long long int img_area = height * width;
    cout << "Area = " << img_area << " px" << endl;
    
    float img_mean = getMean(img);
    float img_stdev = getStdev(img);
    
    cout <<"T_stdev =" << img_stdev << endl;
    
    CImg<unsigned char> ans(width, height, 1, 1, 0);
    CImg<float> Ts(width, height, 1, 1, 0);
    
    int step = 0;
    cimg_forXY(img, i, j) {
        
        ++step;
        if (step % 200000 == 0)
            cout << step << " / " << img_area << endl;
            
        float prev_mean = -1.;
        int window = 30;
        for (; window < std::max(height, width); ++window)
        {
            float wmean = getWindowMean(&integralimg, i, j, window);
            if (wmean > prev_mean)
                prev_mean = wmean;
            else
                break;
        }
        
        float wmean = getWindowMean(&integralimg, i, j, window);
        float stdev = getWindowStdev(&integral2img, &integralimg, i, j, window);

        float T = wmean + k * stdev;
        // float T = wmean + 0.5 * (stdev / 128.0 - 1);
        
        Ts(i, j, 0, 0) = T;
        if (img(i, j, 0, 0) > wmean + k * stdev)
            ans(i, j, 0, 0) = 255;
        else
            ans(i, j, 0, 0) = 0;
    }
    return ans;
}

int main(int argc,char* argv[])
{
    if (argc < 2)
    {
        cout << "Usage: main [filename]" << endl;
        return 0;
    }
    
    string filename = string(argv[1]);
    
    CImg<unsigned char> image(argv[1]),
        gray(image.width(), image.height(), 1, 1, 0),
        grayWeight(image.width(), image.height(), 1, 1, 0),
        imgR(image.width(), image.height(), 1, 3, 0),
        imgG(image.width(), image.height(), 1, 3, 0),
        imgB(image.width(), image.height(), 1, 3, 0);
 
    cimg_forXY(image,x,y) {

        imgR(x,y,0,0) = image(x,y,0,0),    // Red component of image sent to imgR
        imgG(x,y,0,1) = image(x,y,0,1),    // Green component of image sent to imgG
        imgB(x,y,0,2) = image(x,y,0,2);    // Blue component of image sent to imgB
     
        int R = (int)image(x,y,0,0);
        int G = (int)image(x,y,0,1);
        int B = (int)image(x,y,0,2);
        
        int grayValueWeight = (int)(0.299*R + 0.587*G + 0.114*B);

        grayWeight(x,y,0,0) = grayValueWeight;
    }
    
    CImg<unsigned char> ans = binarizeNiblack(grayWeight);
    
    //CImgDisplay main_disp(image,"Original"),
    //    draw_dispGrWeight(grayWeight,"Gray (Weighted)"),
    //    draw_ans(ans,"Transformed");
      
    size_t lastindex = filename.find_last_of("."); 
    string rawname = filename.substr(0, lastindex);  
    lastindex = rawname.find_last_of("/\\"); 
    if (lastindex != string::npos)
        rawname = rawname.substr(lastindex + 1);    
    ans.save(("./processed/" + rawname + ".bmp").c_str());
      
    // mypause();

    return 0;
}
