#include <QtCore/QCoreApplication>
#include <QImage>
#include <iostream>
#include <vector>
#include "Filters.h"
using namespace std;
int main(int argc, char* argv[])
{
    string str;
    QImage img;

    for (int i = 0; i < argc; i++)
    {
        if (!strcmp(argv[i], "-p") && (i + 1 < argc))
        {
            str = argv[i + 1];
        }

    }
    img.load(QString(str.c_str()));
    img.save("Images/Source2.png");

    InvertFilter invert;

    invert.process(img).save(QString("Images\\Invert2.png"));
    Kernel my_lovely_kernel(1);
    
    my_lovely_kernel[0] = 3;
    my_lovely_kernel[1] = 2;
    my_lovely_kernel[2] = 1;
    my_lovely_kernel[3] = 0;
    my_lovely_kernel[4] = 0;
    my_lovely_kernel[5] = -8;
    my_lovely_kernel[6] = 7;
    my_lovely_kernel[7] = -2;
    my_lovely_kernel[8] = 5;
   
  MatrixFilter mat_filter(my_lovely_kernel);
  mat_filter.process(img).save(QString("Images\\MatrixFilter.png"));
    
    return 0;
}
