#pragma once
#include <QImage>
#include <iostream>
#include <cstdlib>

/**
* Функция для приведения значений к допустимому диапазону значений между max и min
*/
template <class T>
T clamp(T value, T max, T min);

class Filter
{
protected:
	//функция расчёта нового цвета пикслея(передаём константную ссылку на картинку и координаты пикселя)
	virtual QColor calcNewPixelColor(const QImage& img, int x, int y) const = 0;//чисто виртуальная
public:
	virtual ~Filter() = default;//создаём деструктор и отсавляем его реализацию компилятору(=default)
	virtual QImage process(const QImage& img) const;//обработка всего изображения
};

//фильтр Инверсии
class InvertFilter :public Filter
{
	//перегружаем функцию расчёта нового цвета пикселя 
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
};


class Kernel
{
protected:
	//данные ядра
	std::unique_ptr<float[]> data;//массив флотов
	//радиус ядра
	std::size_t radius;

	//размер памяти ядра
	std::size_t getLen() const
	{
		return getSize() * getSize();
	}

public:
	//конструктор пустого ядра
	Kernel(std::size_t radius) :radius(radius)
	{
		//выделение памяти под массив размера getLen()
		data = std::make_unique<float[]>(getLen());
	}

	//конструктор копирования
	Kernel(const Kernel& other) : Kernel(other.radius)
	{
		std::copy(other.data.get(), other.data.get() + getLen(), data.get());
	}

	//аксессоры
	std::size_t getRadius() const//радиус
	{
		return radius;
	}
	std::size_t getSize() const//размер
	{
		return radius * 2 + 1;
	}
	float operator[](std::size_t id) const//оператор индексации
	{
		return data[id];
	}
	float& operator[](std::size_t id)
	{
		return data[id];
	}
};

//класс ,реализующий выисление операции свёртки с ядром
class MatrixFilter : public Filter
{
protected:

	Kernel mKernel;
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;

public:
	MatrixFilter(const Kernel& kernel) : mKernel(kernel) //в конструктор необходимо передать ядро
	{

	}
	virtual ~MatrixFilter() = default;
};

/*     Фильтр размытия    */
//ядро
class BlurKernel : public Kernel
{
public:
	using Kernel::Kernel;//???????????
	//конструктор предка
	BlurKernel(std::size_t radius = 2) : Kernel(radius)//: Kernel(radius) это переача аргументов в конструктор предка
	{
		for (std::size_t i = 0; i < getLen(); i++)//расчёт матрицы весовых коэффициентов
		{
			data[i] = 1.0f / getLen();//коэффициенты зависят от размера ядра
		}
	}
};
//класс
class BlurFilter : public MatrixFilter
{
public:
	BlurFilter(std::size_t radius = 1) : MatrixFilter(BlurKernel(radius)) {}
};

class GaussianKernel : public Kernel
{
public:
	//Наследование конструкторов
	using Kernel::Kernel;
	GaussianKernel(std::size_t radius = 2, float sigma = 3.f) :Kernel(radius)
	{
		//коэффициент нормировки ядра
		float norm = 0;
		int signed_radius = static_cast<int>(radius);
		//расчитываем ядро линейного фильтра
		for (int x = -signed_radius; x <= signed_radius; x++)
		{
			for (int y = -signed_radius; y <= signed_radius; y++)
			{
				std::size_t idx = (x + radius) * getSize() + (y + radius);
				data[idx] = std::exp(-(x * x + y * y) / (sigma * sigma));
				norm += data[idx];
				std::cout <<data[idx]<<" ";
			}
			std::cout << "\n";
		}
		//нормируем ядро
		std::cout << "Norm:\n";
		for (std::size_t i = 0; i <getLen(); i++)
		{
			data[i] /= norm;
			std::cout << data[i] << " ";
		}
	}
};

class GaussianFilter :public MatrixFilter
{
public:
	GaussianFilter(std::size_t radius = 1) : MatrixFilter(GaussianKernel(radius)) {}
};



/*Задания для самостоятельного выполнения*/

/*Точечные фильтры*/

/*GrayScale*/

class GrayScaleFilter :public Filter
{
	//перегружаем функцию расчёта нового цвета пикселя 
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
};

/*Сепия*/

class SepiaFilter :public Filter
{
	//перегружаем функцию расчёта нового цвета пикселя 
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
};

/*Увеличение яркости*/
class IntensityUP : public Filter
{
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
};

/*Фильтр Собеля по нахождению градиента яркости*/
class YSobelKernel : public Kernel
{
public:
	//Наследование конструкторов
	using Kernel::Kernel;
	YSobelKernel(std::size_t radius = 1) :Kernel(radius)
	{
		data[0] = -1; data[1] = -2; data[2] = -1;
		data[3] =  0; data[4] =  0; data[5] =  0;
		data[6] =  1;  data[7] = 2; data[8] =  1;		
	}
};

class SobelFilter :public MatrixFilter
{
protected:

public:
	SobelFilter(std::size_t radius = 1) : MatrixFilter(YSobelKernel(radius)) {}

	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
};



/*Повышение контрасности*/
//ядро
class SharpnessUPKernel : public Kernel
{
public:
	//Наследование конструкторов
	using Kernel::Kernel;
	SharpnessUPKernel(std::size_t radius = 1) :Kernel(radius)
	{
		data[0] = 0; data[1] = -1; data[2] = 0;
		data[3] = -1; data[4] = 5; data[5] = -1;
		data[6] = 0;  data[7] = -1; data[8] = 0;
	}
};
//фильтр
class SharpnessUPFilter :public MatrixFilter
{
public:
	SharpnessUPFilter(std::size_t radius = 1) : MatrixFilter(SharpnessUPKernel(radius)) {}
};

/*Серый мир*/
class GreyWorldFilter :public Filter
{
	float med_R, med_G, med_B, avg;
public:
	GreyWorldFilter()
	{
		med_R = 1, med_G = 1, med_B = 1, avg = 1;
	}
	//перегружаем функцию расчёта нового цвета пикселя 
	void mediumIntensities(const QImage& img);
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
	QImage process(const QImage& img);
};



/*Линейная растяжка*/
class HistugrammFilter : public Filter
{
	float intensity_max, intensity_min;
public:
	HistugrammFilter()
	{
		intensity_max = 1, intensity_min=1;		
	}
	void intensities_range_calc(const QImage& img);
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
	QImage process(const QImage& img);
};

/*Фильтры индексации пикселей*/
//
//class ShiftFilter : public Filter
//{
//protected:
//		struct 
//		{
//			int k;
//			int l;
//		} typedef KL;
//public:
//	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
//	QImage process(const QImage& img) const override;
//	KL calcFromWhereTakeColor(int x,int y) const;
//};
class LocationFilter 
{
protected:
	struct 
	{
		int k;
		int l;
	} typedef KL;
public:
	virtual KL calcNewPixelLocation(int x, int y) const =0 ;
	virtual QImage process(const QImage& img) const;
	virtual ~LocationFilter() = default;
};

/*Перенос/сдвиг*/
class ShiftFilter : public LocationFilter
{
public:
	KL calcNewPixelLocation(int x, int y) const override;
	
};

/* Фильтр "эффект стекла" */
class GlassFilter : public LocationFilter
{
public:
	QImage process(const QImage& img) const;
	KL calcNewPixelLocation(int x, int y) const;
};


/*Филтр Motion Blur*/
//ядро
class MotionBlurKernel : public Kernel
{
public:
	using Kernel::Kernel;
	MotionBlurKernel(std::size_t radius = 1) :Kernel(radius)
	{
		int signed_radius = static_cast<int>(radius);
		//расчитываем ядро линейного фильтра
		for (int x = -signed_radius; x <= signed_radius; x++)
		{
			for (int y = -signed_radius; y <= signed_radius; y++)
			{
				std::size_t idx = (x + radius) * getSize() + (y + radius);
				
				if (x == y)
				{
					data[idx] = (float)1 / getSize();
				}
				else
				{
					data[idx] = 0;
				}				
				std::cout << data[idx]<<" ";
			}
			std::cout << "\n";
		}
	}

};

//фильтр
class MotionBlurFilter : public MatrixFilter
{
public:
	MotionBlurFilter(std::size_t radius = 1) : MatrixFilter(MotionBlurKernel(radius)) {}

};

/*Повышение резкости*/
class UltraSharpnessKernel : public Kernel
{
public:
	using Kernel::Kernel;
	UltraSharpnessKernel(std::size_t radius = 1) :Kernel(radius)
	{
		data[0] = -1;  data[1] = -1;  data[2] = -1;
		data[3] = -1;  data[4] = 9;   data[5] = -1;
		data[6] = -1;  data[7] = -1;  data[8] = -1;
	}
};
class UltraSharpnessFilter : public MatrixFilter
{
public:
	UltraSharpnessFilter(std::size_t radius = 1) : MatrixFilter(UltraSharpnessKernel(radius)) {}
};



/*Математическая морфология*/
//Чёрно-белый
class BlackWhiteFilter : public Filter
{
public : 
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
};


/*Dilation*/
class DilationKernel : public Kernel
{
public:
	using Kernel::Kernel;
	//конструктор предка
	DilationKernel(std::size_t radius = 1) : Kernel(radius)//: Kernel(radius) это переача аргументов в конструктор предка
	{
		for (size_t i = 0; i < getLen(); i++)
		{
			data[i] = 1;
		}
	/*  data[0] = 1; data[1] = 1;   data[2] = 1;
		data[3] = 1;  data[4] = 1;   data[5] = 1;
		data[6] = 1;  data[7] = 1;   data[8] = 1;*/
		
	}
};

class DilationFilter : public MatrixFilter
{
public:
	DilationFilter(std::size_t radius = 1) : MatrixFilter(DilationKernel(radius)) {}
	QImage process(const QImage& img) const override; 
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
};

/*Erosion*/
class ErosionKernel : public Kernel
{
public:
	using Kernel::Kernel;
	//конструктор предка
	ErosionKernel(std::size_t radius = 1) : Kernel(radius)//: Kernel(radius) это переача аргументов в конструктор предка
	{
		for (size_t i = 0; i < getLen(); i++)
		{
			data[i] = 1;
		}
		/*  data[0] = 1; data[1] = 1;   data[2] = 1;
			data[3] = 1;  data[4] = 1;   data[5] = 1;
			data[6] = 1;  data[7] = 1;   data[8] = 1;*/

	}
};

class ErosionFilter : public MatrixFilter
{
public:
	ErosionFilter(std::size_t radius = 1) : MatrixFilter(ErosionKernel(radius)) {}
	QImage process(const QImage& img) const override;
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
};


/*Median Filter*/
class MedianKernel : public Kernel
{
public:
	using Kernel::Kernel;
	MedianKernel(std::size_t radius = 1) : Kernel(radius)
	{
		int signed_radius = static_cast<int>(radius);
		for (int i = -signed_radius; i <= signed_radius; i++)
		{
			for (int j = -signed_radius; j <= signed_radius; j++)
			{
				std::size_t idx = (i + radius) * getSize() + (j + radius);
				data[idx] = 1;
			}
		}
	}
};

class MedianFilter : public MatrixFilter
{
public:
	MedianFilter(std::size_t radius = 2) : MatrixFilter(MedianKernel(radius)) {}
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
};



/*Opening*/
/*erosion -> dilation*/
class OpeningFilter 
{

public:
	QImage process(const QImage& img) const 
	{
		DilationFilter dilation(2);
	ErosionFilter erosion(2);
		QImage result;
		QImage temp;
		temp = erosion.process(img);
		result = dilation.process(temp);
		return result;
	}
};


/*Opening*/
/*dilation -> erosion*/
class ClosingFilter
{

public:
	QImage process(const QImage& img) const
	{
		DilationFilter dilation(2);
		ErosionFilter erosion(2);
		QImage result;
		QImage temp;
		temp = dilation.process(img);
		result = erosion.process(temp);
		return result;
	}
};


/*Top hat*/
class TopHatFilter
{
public:
	QImage process(const QImage& img) const;

};