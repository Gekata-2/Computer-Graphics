#include "Filters.h"
#include<math.h>

template <class T>
T clamp(T value, T max, T min)
{
	//если значение больше максимального - возвращаем максимально,если меньше минимального - минимальное
	if (value > max)
	{
		return max;
	}
	if (value < min)
	{
		return min;
	}
	//если в допустимом диапазоне,то возвращаем без изменений
	return value;
}

//обработка всего изображения (для всех фильтров одинаковая)
QImage Filter::process(const QImage& img) const
{
	QImage result(img);//создаём переменную-картинку-результат
	//проходим каждый пикслей в цикле 
	for (int x = 0; x < img.width(); x++)
	{
		for (int y = 0; y < img.height(); y++)
		{
			//создаём "переменную-результат"-"color" работы функции обработки цвета текущего пикселя
			//(в новой картинке)
			QColor color = calcNewPixelColor(img, x, y);
			//в результирующей картинке устанавливаем этот пиксель(x,y) в новый цвет color
			result.setPixelColor(x, y, color);
		}
	}
	//возвращаем полученное изображение
	return result;
}

//расчёт нового цвета пикселя для фильтра "Инверсия" 
QColor InvertFilter::calcNewPixelColor(const QImage& img, int x, int y) const
{
	//берём значения цвета текущего пикселя
	QColor color = img.pixelColor(x, y);
	//устанавливаем новые значения r g b (меняем цвет на противоположный) 
	color.setRgb(255 - color.red(), 255 - color.green(), 255 - color.blue());
	return color;//возвращаем полученный новый цвет пикселя
}


//Матричный фильтр::расчёт нового цвета пикселя
QColor MatrixFilter::calcNewPixelColor(const QImage& img, int x, int y) const // x,y -координаты текущего пикселя
{
	//результаты расчёта цвета 
	float returnR = 0;
	float returnG = 0;
	float returnB = 0;
	int size = mKernel.getSize();//получаем размер
	int radius = mKernel.getRadius();//радиус

	//Переменные i и j принимают значения от -radius
	//до radius и означают положение точки в матрице ядра, если начало отсчета
	//поместить в центр матрицы,т.е. если  i =0 и j=0 это центр,то i=-redius,j=-redius это левый верхний угол
	for (int i = -radius; i <= radius; i++)//проходим по ядру и высчитываем новые r g b 
	{
		for (int j = -radius; j <= radius; j++)
		{
			//В переменной idx хранятся координаты пикселя –
			//соседа пикселя x, y, для которого происходит вычисления цвета
			//при этом осуществляется пересчёт в систему ,где отчёт ведётся от левого верхнего угла
			int idx = (i + radius) * size + j + radius;//например центр(i=0,j=0) перейдёт в 5 ,если радиус равен 1 
			
			//получаем значения r g b текущего обрабатываемого пикселя, при этом проверяем чтобы координаты положения пикселя были в пределах картинки
			//для ширины и высоты отдельно
			QColor color = img.pixelColor(clamp<float>(x + j, img.width() - 1, 0), clamp<float>(y + i, img.height() - 1, 0));

			//высчитываем новые значения цветов
			//для каждого цвета r g b 
			//умножаем текущие значения на цветовой коэффициент
			returnR += color.red() * mKernel[idx];
			returnG += color.green() * mKernel[idx];
			returnB += color.blue() * mKernel[idx];
		}
	}
	//возвращаем полученный цвет
	return QColor(clamp<float>(returnR, 255.f, 0.f), clamp<float>(returnG, 255.f, 0.f), clamp<float>(returnB, 255.f, 0.f));
}

QColor GrayScaleFilter::calcNewPixelColor(const QImage& img, int x, int y) const
{
	//берём значения цвета текущего пикселя
	QColor color = img.pixelColor(x, y);

	//расчитываем яркость(интенсивность)
	float intensity = 0.299 * color.red() + 0.587 * color.green() + 0.144 * color.blue();
	//устанавливаем во все каналы полученное значение
	color.setRgb(clamp<float>(intensity, 255.f, 0.f), clamp<float>(intensity, 255.f, 0.f), clamp<float>(intensity, 255.f, 0.f));
	return color;
}

QColor SepiaFilter::calcNewPixelColor(const QImage& img, int x, int y) const
{
	//берём значения цвета текущего пикселя
	QColor color = img.pixelColor(x, y);

	//расчитываем яркость(интенсивность)
	float intensity = 0.299 * color.red() + 0.587 * color.green() + 0.144 * color.blue();
	
	float k = 58;
	//устанавливаем во все каналы полученное значение
	color.setRgb(clamp<float>(intensity + 2 * k, 255.f, 0.f), clamp<float>(intensity + 0.5 * k, 255.f, 0.f), clamp<float>(intensity - 1 * k, 255.f, 0.f));

	return color;
}

QColor IntensityUP::calcNewPixelColor(const QImage& img, int x, int y) const
{
	//берём значения цвета текущего пикселя
	QColor color = img.pixelColor(x, y);

	float constanta = 58;

	//устанавливаем во все каналы полученное значение
	color.setRgb(clamp<float>(color.red()+ constanta, 255.f, 0.f), clamp<float>(color.green()+ constanta, 255.f, 0.f), clamp<float>(color.blue() + constanta, 255.f, 0.f));

	return color;
}



QColor SobelFilter::calcNewPixelColor(const QImage& img, int x, int y) const // x,y -координаты текущего пикселя
{
	//результаты расчёта цвета 
	float returnR = 0;
	float returnG = 0;
	float returnB = 0;
	float GyR=0, GyG=0, GyB=0;
	float GxR=0, GxG=0, GxB=0;
	int size = mKernel.getSize();//получаем размер
	int radius = mKernel.getRadius();//радиус
	

	
	for (int i = -radius; i <= radius; i++)//проходим по ядру и высчитываем новые r g b 
	{
		for (int j = -radius; j <= radius; j++)
		{
			//В переменной idx хранятся координаты пикселя –
			//соседа пикселя x, y, для которого происходит вычисления цвета
			//при этом осуществляется пересчёт в систему ,где отчёт ведётся от левого верхнего угла
			int idxY = (i + radius) * size + j + radius;//например центр(i=0,j=0) перейдёт в 5 ,если радиус равен 1 
			int idxX = (j + radius) * size + i + radius;
			//получаем значения r g b текущего обрабатываемого пикселя, при этом проверяем чтобы координаты положения пикселя были в пределах картинки
			//для ширины и высоты отдельно
		
			//берём цвет пикселя 
			QColor color = img.pixelColor(clamp<float>(x + j, img.width() - 1, 0), clamp<float>(y + i, img.height() - 1, 0));
					
			//считаем переходы по вертикали 
			GyR += color.red() * mKernel[idxY];
			GyG += color.green() * mKernel[idxY];
			GyB += color.blue() * mKernel[idxY];
			//считаем переходы по горизонтали
			GxR += color.red() * mKernel[idxX];
			GxG += color.green() * mKernel[idxX];
			GxB += color.blue() * mKernel[idxX];
			
		}
	}
	returnR = std::sqrt(GyR * GyR + GxR * GxR);
	returnG = std::sqrt(GyG * GyG + GxG * GxG);
	returnB = std::sqrt(GyB * GyB + GxB * GxB);

	//возвращаем полученный цвет
	return QColor(clamp<float>(returnR, 255.f, 0.f), clamp<float>(returnG, 255.f, 0.f), clamp<float>(returnB, 255.f, 0.f));
}


void GreyWorldFilter::mediumIntensities(const QImage& img)
{
	int size = img.width()*img.height();//количество пикселей в картинке
	
	float accum_R = 0;
	float accum_G = 0;
	float accum_B = 0;
	for (int x = 0; x < img.width(); x++)
	{
		for (int y = 0; y < img.height(); y++)
		{
			QColor color=img.pixelColor(clamp<float>(x, img.width() - 1, 0), clamp<float>(y, img.height() - 1, 0));
			accum_R += color.red();
			accum_G += color.green();
			accum_B += color.blue();
		}
	}
	med_R = accum_R / size;
	med_G = accum_G / size;
	med_B = accum_B / size;
	avg = (med_R + med_G + med_B) / 3;
	//return QColor(clamp<float>(med_R, 255.f, 0.f), clamp<float>(med_G, 255.f, 0.f), clamp<float>(med_B, 255.f, 0.f));
}

QColor GreyWorldFilter::calcNewPixelColor(const QImage& img, int x, int y) const
{
	//берём значения цвета текущего пикселя
	QColor color = img.pixelColor(x, y);
	
	//устанавливаем во все каналы полученное значение
	color.setRgb(clamp<float>(color.red()*avg/med_R, 255.f, 0.f), clamp<float>(color.green() * avg / med_G, 255.f, 0.f), clamp<float>(color.blue() * avg / med_B, 255.f, 0.f));

	return color;
}




QImage GreyWorldFilter::process(const QImage& img) 
{
	mediumIntensities(img);
	QImage result(img);//создаём переменную-картинку-результат
	//проходим каждый пикслей в цикле 
	for (int x = 0; x < img.width(); x++)
	{
		for (int y = 0; y < img.height(); y++)
		{
			//создаём "переменную-результат"-"color" работы функции обработки цвета текущего пикселя
			//(в новой картинке)
			QColor color = calcNewPixelColor(img, x, y);
			//в результирующей картинке устанавливаем этот пиксель(x,y) в новый цвет color
			result.setPixelColor(x, y, color);
		}
	}
	//возвращаем полученное изображение
	return result;
}

QImage HistugrammFilter::process(const QImage& img)
{
	intensities_range_calc(img);
	QImage result(img);//создаём переменную-картинку-результат
	//проходим каждый пикслей в цикле 
	for (int x = 0; x < img.width(); x++)
	{
		for (int y = 0; y < img.height(); y++)
		{
			//создаём "переменную-результат"-"color" работы функции обработки цвета текущего пикселя
			//(в новой картинке)
			QColor color = calcNewPixelColor(img, x, y);
			//в результирующей картинке устанавливаем этот пиксель(x,y) в новый цвет color
			result.setPixelColor(x, y, color);
		}
	}
	//возвращаем полученное изображение
	return result;
}

QColor HistugrammFilter::calcNewPixelColor(const QImage& img, int x, int y) const
{
	//берём значения цвета текущего пикселя
	QColor color = img.pixelColor(x, y);

	//устанавливаем во все каналы полученное значение
	float intensity = 0, intensity_tmp = 0;
	intensity_tmp = 0.299 * color.red() + 0.587 * color.green() + 0.144 * color.blue();
	intensity = (intensity_tmp - intensity_min) * (255 - 0) / (intensity_max - intensity_min);

	color.setRgb(clamp<float>(intensity, 255.f, 0.f), clamp<float>(intensity, 255.f, 0.f), clamp<float>(intensity, 255.f, 0.f));
	return color;
}


void HistugrammFilter::intensities_range_calc(const QImage& img)
{
	float min_int = 0, max_int = 0;
	float tmp_intens;
	for (int x = 0; x < img.width(); x++)
	{
		for (int y = 0; y < img.height(); y++)
		{
			QColor color = img.pixelColor(clamp<float>(x, img.width() - 1, 0), clamp<float>(y, img.height() - 1, 0));
			tmp_intens = 0.299 * color.red() + 0.587 * color.green() + 0.144 * color.blue();
			if (tmp_intens > max_int)
			{
				max_int = tmp_intens;
			}
			if (tmp_intens < min_int)
			{
				min_int = tmp_intens;
			}
		}
	}
	intensity_max = max_int;
	intensity_min = min_int;

}




QImage LocationFilter::process(const QImage& img) const 
{
	QImage result(img);//создаём переменную-картинку-результат
	//проходим каждый пикслей в цикле 
	for (int x = 0; x < img.width(); x++)
	{
		for (int y = 0; y < img.height(); y++)
		{
			
			QColor color = img.pixelColor(x, y);//считываем цвет текущего пиксле (x,y) исходного изображения
			KL result_kl = calcNewPixelLocation(x, y);//высчитываем его положение в новой картинке (x,y) - > (k,l)

			QColor black_color;
				black_color.setRgb(0, 0, 0);
				//больше границ
				if 	(result_kl.k >= img.width())
				{
					result_kl.k = result_kl.k % img.width();
					color = black_color;
				}
				if (result_kl.k < 0)
				{
					result_kl.k = img.width() + result_kl.k;//result_kl.k<0
					color = black_color;
				}
				//меньше границ
				if (result_kl.l >= img.height())
				{
					result_kl.l = result_kl.l % img.height();
					color = black_color;
				}	
				if (result_kl.l < 0)
				{
					result_kl.l = img.height() + result_kl.l;//result_kl.l<0
						color = black_color;
				}
			//записываем в полученное местоположение цвет текущего пикселя
			result.setPixelColor(result_kl.k, result_kl.l, color);
		}
	}
	//возвращаем полученное изображение
	return result;
}



///* 
//	𝑥(𝑘, 𝑙) = 𝑘 + 50;
//    𝑦(𝑘, 𝑙) = 𝑙;
//*/
LocationFilter::KL ShiftFilter::calcNewPixelLocation(int x, int y) const
{
	KL result;
	result.k = x - 50;
	result.l = y + 50;
	return result;
}



QImage GlassFilter::process(const QImage& img) const
{
	QImage result(img);//создаём переменную-картинку-результат
	//проходим каждый пикслей в цикле 
	for (int x = 0; x < img.width(); x++)
	{
		for (int y = 0; y < img.height(); y++)
		{

			//QColor color = img.pixelColor(x, y);//считываем цвет текущего пиксле (x,y) исходного изображения
			KL result_kl = calcNewPixelLocation(x, y);//высчитываем откуда брать значение для текущего пикселя (x,y)
			
		
			//Если вышли за границы картинки ,то оставляем пиксель без изменений
			if (result_kl.k >= img.width())
			{
				result_kl.k = x;				
			}
			if (result_kl.k <= 0)
			{
				result_kl.k =x;//result_kl.k<0
			
			}
			//меньше границ
			if (result_kl.l >= img.height())
			{
				result_kl.l =y;
			
			}
			if (result_kl.l <= 0)
			{
				result_kl.l =y;//result_kl.l<0
			}
			//после того как высчитали координаты пикселя от которого будем брать цвет 
			QColor	color= img.pixelColor(result_kl.k, result_kl.l);
			//записываем полученный цвет в наш текущий пиксель
			result.setPixelColor(x, y, color);
		}
	}
	//возвращаем полученное изображение
	return result;
}



/*
		𝑥(𝑘, 𝑙) = 𝑘 + (𝑟𝑎𝑛𝑑(1) – 0.5) ∗ 10;
		𝑦(𝑘, 𝑙) = 𝑙 + (𝑟𝑎𝑛𝑑(1) – 0.5) ∗ 10;
*/
LocationFilter::KL GlassFilter::calcNewPixelLocation(int x, int y) const
{
	KL result;
	result.k = x- (float(rand()%101)/100-0.5)*10;
	result.l = y -(float(rand()%101)/100-0.5)*10;
	return result;
}


QColor BlackWhiteFilter::calcNewPixelColor(const QImage& img, int x, int y) const
{
	float intensity_max, intensity_min;
	intensity_max = 0.299 * 255 + 0.587 * 255 + 0.144 * 255;
	//берём значения цвета текущего пикселя
	QColor color = img.pixelColor(x, y);
	QColor black_color;
	black_color.setRgb(0, 0, 0);
	QColor white_color;
	white_color.setRgb(255, 255, 255);
	//расчитываем яркость(интенсивность)
	float intensity = 0.299 * color.red() + 0.587 * color.green() + 0.144 * color.blue();
	
	if (intensity> intensity_max/(2.5f))
	{
		color = white_color;
	}
	else
	{
		color = black_color;
	}
	
	//устанавливаем во все каналы полученное значение
	color.setRgb(clamp<float>(color.red(), 255.f, 0.f), clamp<float>(color.green(), 255.f, 0.f), clamp<float>(color.blue(), 255.f, 0.f));
	return color;
}


QImage DilationFilter::process(const QImage& img) const
{
	QImage result(img);//создаём переменную-картинку-результат
	
	int mask_width=1, mask_height = 1;
	int mask_radius = 0;
	mask_radius = mKernel.getRadius();
	
	//img.convertToFormat(QImage::Format_Mono);
	for (int x = mask_radius; x < img.width()- mask_radius ; x++)
	{
		for (int y = mask_radius; y < img.height()- mask_radius; y++)
		{
			//создаём "переменную-результат"-"color" работы функции обработки цвета текущего пикселя
			//(в новой картинке)
			QColor color = calcNewPixelColor(img, x, y);
			//в результирующей картинке устанавливаем этот пиксель(x,y) в новый цвет color
			result.setPixelColor(x, y, color);
		}
	}
	return result;
}


QColor DilationFilter::calcNewPixelColor(const QImage& img, int x, int y) const
{
	QColor result;

	int max=0;


	int size = mKernel.getSize();//получаем размер
	int radius = mKernel.getRadius();//радиус


	for (int i = -radius; i < radius; i++)
	{
		for (int j = -radius; j < radius; j++)
		{
			QColor tmp_color = img.pixelColor(x+i, y+j);
			float tmp_intensity = 0;

			int idx = (i + radius) * size + j + radius;
			if ((mKernel[idx]) && (tmp_color.red() > max)&& (tmp_color.green() > max)&& (tmp_color.blue() > max))
			{				
				max = 255;			
			}
		}
	}
	result.setRgb(clamp<float>(max, 255.f, 0.f), clamp<float>(max, 255.f, 0.f), clamp<float>(max, 255.f, 0.f));
	return result;
}


/**/

QImage ErosionFilter::process(const QImage& img) const
{
	QImage result(img);//создаём переменную-картинку-результат

	int mask_width = 1, mask_height = 1;
	int mask_radius = 0;
	mask_radius = mKernel.getRadius();

	//img.convertToFormat(QImage::Format_Mono);
	for (int x = mask_radius; x < img.width() - mask_radius; x++)
	{
		for (int y = mask_radius; y < img.height() - mask_radius; y++)
		{
			//создаём "переменную-результат"-"color" работы функции обработки цвета текущего пикселя
			//(в новой картинке)
			QColor color = calcNewPixelColor(img, x, y);
			//в результирующей картинке устанавливаем этот пиксель(x,y) в новый цвет color
			result.setPixelColor(x, y, color);
		}
	}
	return result;
}


QColor ErosionFilter::calcNewPixelColor(const QImage& img, int x, int y) const
{
	QColor result;

	int min = 255;


	int size = mKernel.getSize();//получаем размер
	int radius = mKernel.getRadius();//радиус


	for (int i = -radius; i < radius; i++)
	{
		for (int j = -radius; j < radius; j++)
		{
			QColor tmp_color = img.pixelColor(x + i, y + j);
			float tmp_intensity = 0;

			int idx = (i + radius) * size + j + radius;
			if ((mKernel[idx]) && (tmp_color.red() < min) && (tmp_color.green() < min) && (tmp_color.blue() < min))
			{
				min = 0;
			}
		}
	}
	result.setRgb(clamp<float>(min, 255.f, 0.f), clamp<float>(min, 255.f, 0.f), clamp<float>(min, 255.f, 0.f));
	return result;
}





/*Median FIlter*/
void sortcolor(float color[], int Ind)
{
	int tmp = 0;
	for (int i = 0; i < Ind; i++)
		for (int j = (Ind - 1); j >= (i + 1); j--)
			if (color[j] < color[j - 1])
			{
				tmp = color[j];
				color[j] = color[j - 1];
				color[j - 1] = tmp;
			}
};
float median(float color[], int Ind)
{
	int Indx = Ind / 2;
	return color[Indx];
};



QColor MedianFilter::calcNewPixelColor(const QImage& img, int x, int y) const
{
	float returnR = 0;
	float returnG = 0;
	float returnB = 0;
	int size = mKernel.getSize();
	int radius = mKernel.getRadius();
	float* colorR = new float[size * size];
	float* colorG = new float[size * size];
	float* colorB = new float[size * size];
	
	for (int i = -radius; i <= radius; i++)
	{
		for (int j = -radius; j <= radius; j++)
		{
			int idx = (i + radius) * size + j + radius;
			QColor color = img.pixelColor(clamp(x + j, img.width() - 1, 0), clamp(y + i, img.height() - 1, 0));
			colorR[idx] = float(color.red()) * mKernel[idx];
			colorG[idx] = float(color.green()) * mKernel[idx];
			colorB[idx] = float(color.blue()) * mKernel[idx];
		}
	}
	sortcolor(colorR, size * size);
	sortcolor(colorG, size * size);
	sortcolor(colorB, size * size);
	//returnR += color.red() * mKernel[idx];
	//returnG += color.green() * mKernel[idx];
	//returnB += color.blue() * mKernel[idx];
	return QColor(clamp(median(colorR, size * size), 255.f, 0.f), clamp(median(colorG, size * size), 255.f, 0.f),
		clamp(median(colorB, size * size), 255.f, 0.f));
};






QImage TopHatFilter::process(const QImage& img) const
{
	ErosionFilter erosion(3);
	QImage result(img);
	QImage temp;
	temp = erosion.process(img);

	for (int x = 0; x < img.width(); x++)
	{
		for (int y = 0; y < img.height(); y++)
		{
			QColor color, color_a, color_ero;
			int colorR = 0, colorB = 0, colorG = 0;

			color_a = img.pixelColor(x, y);//цвет пикселя исходной картинки
			color_ero = temp.pixelColor(x, y);//цвет пикселя картинки после эрозии 

			//находим разность этих цветов по каналам
			colorR = color_a.red() - color_ero.red();
			colorB = color_a.blue() - color_ero.blue();
			colorG = color_a.green() - color_ero.green();
			color.setRgb(clamp<float>(colorR, 255.f, 0.f), clamp<float>(colorB, 255.f, 0.f), clamp<float>(colorG, 255.f, 0.f));
			//в результирующей картинке устанавливаем этот пиксель(x,y) в новый цвет color
			result.setPixelColor(x, y, color);
		}
	}

	return result;
}