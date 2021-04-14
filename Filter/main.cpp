#include <QtCore/QCoreApplication>
#include <QImage>
#include <iostream>
#include <cstdlib>

#include "Filters.h"
using namespace std;
int main(int argc, char* argv[])
{
    srand(1);
    string str;
    QImage img;//класс  QImage позволяет удобный инерфейс для работы с пикселями и предоставляет
    //методы обработки цвета пикселей  и т.д.
    
    for (int i = 0; i < argc; i++)//находим адрес картинки
    {
        if (!strcmp(argv[i], "-p") && (i + 1 < argc))
        {
            str = argv[i + 1];
        }

    }
    img.load(QString(str.c_str()));//преобразовываем полученный адрес из формата string  в c-строку 
    //и преобразуем её в тип  QString
    //Класс QString предоставляет символьную строку Unicode.
   // img.save("Images\\Source1.png");//сохраняем картинку в папку Images

   

  //InvertFilter invert;
  //    invert.process(img).save(QString("Images\\Invert1.png"));
  // BlurFilter blur;
  //    blur.process(img).save(QString("Images\\Blur2.png"));
 /* GaussianFilter gauss;
      gauss.process(img).save(QString("Images\\Gauss1.png"));*/
 /* GrayScaleFilter gray_scale;
      gray_scale.process(img).save(QString("Images\\GreyScale1.png"));*/
 /* SepiaFilter sepia;
      sepia.process(img).save(QString("Images\\Sepia1.png"));*/
  //IntensityUP intensity_up;
  //    intensity_up.process(img).save(QString("Images\\IntensityUP1.png"));
  SobelFilter sobel;
  GrayScaleFilter gray_scale;
  QImage sbl;
  sbl =sobel.process(img);
  gray_scale.process(sbl).save(QString("Images\\Sobel1_grey.png"));
  //SharpnessUPFilter sharpness;;
  //    sharpness.process(img).save(QString("Images\\Sharpness1.png"));
   /* GreyWorldFilter grey_world;
    grey_world.process(img).save(QString("Images\\GreyWorld1.png"));*/
    //HistugrammFilter hist;
    //hist.process(img).save(QString("Images\\Hist5.png"));
  /*  ShiftFilter shift;
    shift.process(img).save(QString("Images\\Shift1.png"));*/
    //GlassFilter glass;
    //glass.process(img).save(QString("Images\\Glass1.png"));

    //MotionBlurFilter motion_blur(4);
    //motion_blur.process(img).save(QString("Images\\MotionBlur1.png"));
    /*UltraSharpnessFilter ultra_sharpness;
    ultra_sharpness.process(img).save(QString("Images\\`UltraSharpness1.png"));*/
   /* BlackWhiteFilter bw;
   bw.process(img).save(QString("Images\\BlackWhiteFilter.png"));*/
  

  /*  DilationFilter dilation(3);
    dilation.process(img).save(QString("Images\\Dilation6.png"));*/
    //ErosionFilter erosion(2);
    //erosion.process(img).save(QString("Images\\Erosion6.png"));
   /* MedianFilter median(5);
    median.process(img).save(QString("Images\\Median1.png")); */

  /*  OpeningFilter opening;
    opening.process(img).save(QString("Images\\Opening6_inv_noi.png")); */

    /*ClosingFilter closing;
    closing.process(img).save(QString("Images\\Opening6_noi.png"));*/

    //TopHatFilter top_hat;
    //top_hat.process(img).save(QString("Images\\TopHat7.png"));
    return 0;
}
