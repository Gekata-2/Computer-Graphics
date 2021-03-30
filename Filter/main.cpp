#include <QtCore/QCoreApplication>
#include <QImage>
#include <iostream>
#include <vector>
#include "Filters.h"
using namespace std;
int main(int argc, char *argv[])
{
   string str;
    QImage img;

        for (int i = 0; i < argc; i++)
        {            
            if (!strcmp(argv[i], "-p") && (i + 1 < argc))
            {
                str=argv[i + 1];
            }
      
        }
        img.load(QString(str.c_str()));
        img.save("Images/Source2.png");
    
        InvertFilter invert;
    
        invert.process(img).save(QString("Images\\Invert2.png"));
   
    return 0;
}
