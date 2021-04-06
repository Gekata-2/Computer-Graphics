#pragma once
#include <QImage>

template <class T>
T clamp(T value, T max, T min);

class Filter
{
protected:
	virtual QColor calcNewPixelColor(const QImage& img, int x, int y) const = 0;
public:
	virtual ~Filter() = default;
	virtual QImage process(const QImage& img) const;
};

class InvertFilter :public Filter
{
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
	std::size_t getRadius() const
	{
		return radius;
	}
	std::size_t getSize() const
	{
		return radius * 2 + 1;
	}
	float operator[](std::size_t id) const
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
	MatrixFilter(const Kernel& kernel) : mKernel(kernel)
	{

	}
	virtual ~MatrixFilter() = default;
};
