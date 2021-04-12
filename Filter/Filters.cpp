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
	int size = mKernel.getSize();//получаем размер
	int radius = mKernel.getRadius();//радиус
	float return_intensity=0;
	//std::cout << mKernel[0] << mKernel[1] << mKernel[2] <<
	//	mKernel[3] << mKernel[4] << mKernel[5]
	//	<< mKernel[6] << mKernel[7] << mKernel[8];
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
			int idxY = (i + radius) * size + j + radius;//например центр(i=0,j=0) перейдёт в 5 ,если радиус равен 1 
			int idxX = (j + radius) * size + i + radius;
			//получаем значения r g b текущего обрабатываемого пикселя, при этом проверяем чтобы координаты положения пикселя были в пределах картинки
			//для ширины и высоты отдельно
		
			QColor color = img.pixelColor(clamp<float>(x + j, img.width() - 1, 0), clamp<float>(y + i, img.height() - 1, 0));
			float intensity = 0.299 * color.red() + 0.587 * color.green() + 0.144 * color.blue();

			//float intensityX = clamp<float>(intensity * mKernel[idxX] , 255.f, 0.f);
			//float intensityY = clamp<float>(intensity * mKernel[idxY] , 255.f, 0.f);
			//float returnRX=0,returnGX=0,returnBX=0;
			//float returnRY=0, returnGY=0, returnBY=0;
			////высчитываем новые значения цветов
			////для каждого цвета r g b 
			////умножаем текущие значения на цветовой коэффициент
			//returnRX += clamp<float>(color.red() * mKernel[idxX], 255.f, 0.f);
			//returnRY += clamp<float>(color.red() * mKernel[idxY], 255.f, 0.f);
			//returnGX += clamp<float>(color.green() * mKernel[idxX], 255.f, 0.f);
			//returnGY += clamp<float>(color.green()* mKernel[idxY], 255.f, 0.f);
			//returnBX += clamp<float>(color.blue() * mKernel[idxX], 255.f, 0.f);
			//returnBY += clamp<float>(color.blue() * mKernel[idxY], 255.f, 0.f);
			//
			//returnR += std::sqrt(returnRX * returnRX + returnRY * returnRY);
			//returnG += std::sqrt(returnGX * returnGX + returnGY * returnGY);
			//returnB += std::sqrt(returnBX * returnBX + returnBY * returnBY);
			//return_intensity = std::sqrt(intensityX * intensityX + intensityY * intensityY);
			returnR += color.red() * mKernel[idxY];
			returnG += color.green() * mKernel[idxY];
			returnB += color.blue() * mKernel[idxY];

			/*returnG += color.green() * mKernel[idx];
			returnB += color.blue() * mKernel[idx];*/
		}
	}

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



/* 
	𝑥(𝑘, 𝑙) = 𝑘 + 50;
    𝑦(𝑘, 𝑙) = 𝑙;
*/
LocationFilter::KL ShiftFilter::calcNewPixelLocation(int x, int y) const
{
	KL result;
	result.k = x + 50;
	result.l = y-250;
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

			QColor color = img.pixelColor(x, y);//считываем цвет текущего пиксле (x,y) исходного изображения
			KL result_kl = calcNewPixelLocation(x, y);//высчитываем его положение в новой картинке (x,y) - > (k,l)
			//больше границ
			if (result_kl.k >= img.width())
			{
				result_kl.k = img.width()-1;				
			}
			if (result_kl.k <= 0)
			{
				result_kl.k = 0;//result_kl.k<0
			
			}
			//меньше границ
			if (result_kl.l >= img.height())
			{
				result_kl.l =img.height()-1;
			
			}
			if (result_kl.l <= 0)
			{
				result_kl.l = 0;//result_kl.l<0
			}
			//записываем в полученное местоположение цвет текущего пикселя
			result.setPixelColor(result_kl.k, result_kl.l, color);
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
	result.k = x+ (rand()%6-3)*10;
	result.l = y +(rand()%6-3)*10;
	return result;
}




