#include "converdemo.h"
#include <windows.h>
#include <gdiplus.h>
#include <GdiPlus.h>
using namespace Gdiplus;

#include<functional>

#ifdef Q_OS_WIN

KPT_BEGIN_NAMESPACE	
//-------------------------------------------------------------------------
// 点的转换
void convertQPointF2GpPointF(const QPointF &qPoint, OUT PointF* pPoint)
{
	Q_ASSERT(NULL == pPoint); 
	pPoint->X = qPoint.x();
	pPoint->Y = qPoint.y();
}
PointF 	convertQPointF2GpPointF(const QPointF &qPoint)
{
	return PointF(qPoint.x(), qPoint.y());
}
//-------------------------------------------------------------------------
// 矩形的转换
void convertQRectF2GpRectF(const QRectF &qRect, OUT RectF* pRect)
{
	Q_ASSERT(NULL == pRect);
	//初始化
	qreal x(0), y(0), width(0), height(0);
	qRect.getCoords(&x, &y, &width, &height);
	pRect->X = x;
	pRect->Y = y;
	pRect->Width = width;
	pRect->Height = height;
}
RectF convertQRectF2GpRectF(const QRectF &qRect)
{
	//初始化
	qreal x(0), y(0), width(0), height(0);
	qRect.getCoords(&x, &y, &width, &height);
	return RectF(x, y, width, height);
}
//-------------------------------------------------------------------------
//颜色的转换
void convertQColor2GpColor(const QColor &qColor, OUT Color* pColor)
{
	Q_ASSERT(pColor);
	//初始化
	qreal red(0), green(0), blue(0), alpha(0);
	qColor.getRgbF(&red, &green, &blue, &alpha);

	//移位来组合ARGB的值
	ARGB argb = (int)(255 * alpha);
// 用下面的代码得到的结果不对
// 	argb = argb << 8 + (int)(255 * red);
// 	argb = argb << 8 + (int)(255 * green);
// 	argb = argb << 8 + (int)(255 * blue);
	
	argb = argb<<8;
	argb += (int)(255 * red);
	argb = argb<<8;
	argb += (int)(255 * green);
	argb = argb<<8;
	argb += (int)(255 * blue);
	pColor->SetValue(argb);
}
Color convertQColor2GpColor(const QColor &qColor)
 {
	//初始化
	qreal red(0), green(0), blue(0), alpha(0);
	qColor.getRgbF(&red, &green, &blue, &alpha);
	return Color(255 * alpha, 255 * red, 255 * green, 255 * blue);
 }
//-------------------------------------------------------------------------
// QTransform 转换成 Matrix,原本想把 QMatrix 转成 Matrix的，不过QT文档说QMatrix是过时的类
inline void converQTransform2GpMatrix(const QTransform &qTransform, OUT Matrix* pMatrix)
{
	Q_ASSERT(pMatrix);

	pMatrix->SetElements(qTransform.m11(), qTransform.m12(), qTransform.m21(), qTransform.m22(), qTransform.dx(), qTransform.dy());
}
 
inline Matrix* converQTransform2GpMatrix(const QTransform &qTransform)
 {
	Matrix* pmatrix = new Matrix;
	converQTransform2GpMatrix(qTransform, pmatrix);
	return pmatrix;
 }
//-------------------------------------------------------------------------
//copy 自 kpt_variant_image.cpp文件
Bitmap* createBmpFromQImage(const QImage &img)
{
	Q_ASSERT(!img.isNull());

	PixelFormat format = PixelFormatUndefined;
	switch (img.format())
	{
	case QImage::Format_Indexed8:
		format = PixelFormat8bppIndexed;
		break;
	case QImage::Format_RGB32:
		format = PixelFormat32bppRGB;
		break;
	case QImage::Format_ARGB32:
		format = PixelFormat32bppARGB;
		break;
	case QImage::Format_ARGB32_Premultiplied:
		format = PixelFormat32bppPARGB;
		break;
	default:
		return NULL;
	}
	Q_ASSERT(PixelFormatUndefined != format);

	Bitmap *pBmp = new Bitmap(img.width(), img.height(), img.bytesPerLine(), format, (BYTE*)img.constBits());
	Q_ASSERT(pBmp->GetLastStatus() == Ok);

	return pBmp;
}

class KConverQImage2GpImage
{
private:
	QImage m_image;
	Bitmap* m_bitmap;
public:
	KConverQImage2GpImage():m_bitmap(NULL)
	{
	}
	~KConverQImage2GpImage()
	{
		m_image = QImage();
		if (m_bitmap)
		{
			delete m_bitmap;
		}
	}
	//因为转换后的Bitmap要用到QImage对象的数据，又想把类的方法像函数一样使用，故用函数对象
	//返回的对象不需要手动释放，类会自己释放
	Image* operator()(const QImage &qImage)
	{
		m_image = qImage;
		m_bitmap = createBmpFromQImage(qImage);
		if (NULL == m_bitmap)
		{
			m_bitmap = createBmpFromQImage(qImage.convertToFormat(QImage::Format_ARGB32));
		}

		Q_ASSERT(m_bitmap);

		return m_bitmap;
	}
};
//-------------------------------------------------------------------------
template<class T>
inline void addMatrix(const QBrush &qBrush, T* pbrush)
{
	Matrix matrix;
	converQTransform2GpMatrix(qBrush.transform(), &matrix);
	pbrush->MultiplyTransform(&matrix, MatrixOrderAppend);
}

inline HatchStyle getHatchStyle(Qt::BrushStyle style)
{
	switch(style)
	{
	case Qt::Dense1Pattern:
		return HatchStyle50Percent;
	case Qt::Dense2Pattern:
		return HatchStyle40Percent;
	case Qt::Dense3Pattern:
		return  HatchStyle30Percent;
	case Qt::Dense4Pattern:	
		return  HatchStyle25Percent;
	case Qt::Dense5Pattern:
		return  HatchStyle20Percent;
	case Qt::Dense6Pattern:
		return  HatchStyle10Percent;
	case Qt::Dense7Pattern:
		return  HatchStyle05Percent;
	case Qt::HorPattern:
		return HatchStyleHorizontal;
	case Qt::VerPattern:
		return HatchStyleVertical;
	case Qt::CrossPattern:
		return HatchStyleCross;
	case Qt::BDiagPattern:
		return HatchStyleBackwardDiagonal;
	case Qt::FDiagPattern:
		return HatchStyleForwardDiagonal;
	case Qt::DiagCrossPattern:
		return HatchStyleDiagonalCross;
	default:
		Q_ASSERT(false);
	}
	return HatchStyleHorizontal;
}

typedef std::function<void((const QGradientStops &, Color* , REAL* , const int &))> fnPtr;
template<class T>
inline void setGradientBrushInterpolationColors(
			QGradientStops &gradientStops, T* pGradientBrush, fnPtr fnConverOperator)
{
	int itemsCount = gradientStops.count();
	Q_ASSERT(itemsCount);
	Color* colors = new Color[itemsCount];
	REAL* reals = new REAL[itemsCount];

	Q_ASSERT(colors);
	Q_ASSERT(reals);
	Q_ASSERT(fnConverOperator);

	fnConverOperator(gradientStops, colors, reals, itemsCount);
	pGradientBrush->SetInterpolationColors(colors, reals, itemsCount);

	delete [] colors;
	delete [] reals;
}

//与函数newPathGradientBrush的颜色复制不同的部分
inline void fnLinearGradientOperation(
	const QGradientStops &gradientStops, Color* colors, REAL* reals, const int &itemsCount)
{
	Q_ASSERT(colors);
	Q_ASSERT(reals);
	Q_ASSERT(itemsCount);

	int index = 0;
	for (auto iter = gradientStops.begin(); iter != gradientStops.end(); ++iter)
	{
		convertQColor2GpColor(iter->second, &colors[index]);
		reals[index] = iter->first;
		++index;
	}
}
inline LinearGradientBrush* newLineGradientBrush(const QBrush &qBrush)
{
	QGradient* qgradient = const_cast<QGradient*>(qBrush.gradient());
	Q_ASSERT(qgradient);
	QLinearGradient* qlineGradinet = static_cast<QLinearGradient*>(qgradient);
	Q_ASSERT(qlineGradinet);

	//得到两个点的坐标
	PointF pointSart(convertQPointF2GpPointF(qlineGradinet->start())),
		pointStop(convertQPointF2GpPointF(qlineGradinet->finalStop()));
	//初始化两种空颜色
	Color colorStart, colorStop;
	LinearGradientBrush* retLinearGrBrush = new LinearGradientBrush(
								pointSart, pointStop, colorStart, colorStop);
	retLinearGrBrush->SetWrapMode(WrapModeTileFlipXY);
	setGradientBrushInterpolationColors(qlineGradinet->stops(), retLinearGrBrush, fnLinearGradientOperation);

// 	Matrix matrix;
// 	converQTransform2GpMatrix(qBrush.transform(), &matrix);
// 	retLinearGrBrush->MultiplyTransform(&matrix, MatrixOrderAppend);
	addMatrix(qBrush, retLinearGrBrush);
	return retLinearGrBrush;
}

//与函数newLineGradientBrush的颜色复制不同的部分
inline void fnPathGradientOperation(
	const QGradientStops &gradientStops, Color* colors, REAL* reals, const int &itemsCount)
{
	Q_ASSERT(colors);
	Q_ASSERT(reals);
	Q_ASSERT(itemsCount);

	int index = itemsCount;
	for (auto iter = gradientStops.begin(); iter != gradientStops.end(); ++iter)
	{
		--index;
		convertQColor2GpColor(iter->second, &colors[index]);
		reals[index] = 1 - iter->first;
	}
}

inline PathGradientBrush* newPathGradientBrush(const QBrush &qBrush)
{
	QGradient* qgradient = const_cast<QGradient*>(qBrush.gradient());
	Q_ASSERT(qgradient);
	QPathGradient* qpathGradinet = static_cast<QPathGradient*>(qgradient);
	Q_ASSERT(qpathGradinet);
	QPainterPath qpath = qpathGradinet->path();
	GraphicsPath* gpath = createGpPath(qpath);		//返回一个new的GraphicsPath对象
	PathGradientBrush* retPathGrBrush = new PathGradientBrush(gpath);
	retPathGrBrush->SetWrapMode(WrapModeTileFlipXY);
	setGradientBrushInterpolationColors(qpathGradinet->stops(), retPathGrBrush, fnPathGradientOperation);
//	下面的代码已经拆分为setGradientBrushInterpolationColors 和fnPathGradientOperation两个方法
// 	QGradientStops gradientStops(qpathGradinet->stops());
// 	int itemsCount = gradientStops.count();
// 	Q_ASSERT(itemsCount);
// 	Color* colors = new Color[itemsCount];
// 	REAL* reals = new REAL[itemsCount];
// 	Q_ASSERT(colors);
// 	Q_ASSERT(reals);
// 
// 	int index = itemsCount;
// 	for (auto iter = gradientStops.begin(); iter != gradientStops.end(); ++iter)
// 	{
// 		--index;
// 		convertQColor2GpColor(iter->second, &colors[index]);
// 		reals[index] = 1 - iter->first;
// 	}
// 
// 	retPathGrBrush->SetInterpolationColors(colors, reals, itemsCount);
// 
// 	delete [] colors;
// 	delete [] reals;

// 	Matrix matrix;
// 	converQTransform2GpMatrix(qBrush.transform(), &matrix);
// 	retPathGrBrush->MultiplyTransform(&matrix, MatrixOrderAppend);
	addMatrix(qBrush, retPathGrBrush);
	delete gpath;
	return retPathGrBrush;
}

inline TextureBrush* newTexturePatternBrush(const QBrush &qBrush)
{
	
	KConverQImage2GpImage converQImage2GpImage;
	Image* image = converQImage2GpImage(qBrush.textureImage());
	TextureBrush* retTextureBrush = new TextureBrush(image);
// 	Matrix matrix;
// 	converQTransform2GpMatrix(qBrush.transform(), &matrix);
// 	retTextureBrush->MultiplyTransform(&matrix, MatrixOrderAppend);
	addMatrix(qBrush, retTextureBrush);
	return retTextureBrush;
}

/*
 @remark convertQBrush2GpBrush方法没有对 ConicalGradientPattern 类型和 RadialGradientPattern 类型进行转换
*/
Brush* convertQBrush2GpBrush(const QBrush &qBrush)
{
	Qt::BrushStyle style = qBrush.style();

	if (style > 1 && style < 15)
	{
		HatchStyle hatchStyle = getHatchStyle(style);
		Color color(convertQColor2GpColor(qBrush.color()));
		return (new HatchBrush(hatchStyle, color, color));
	}
	else
	{
		switch(style)
		{
			case Qt::NoBrush:
				Q_ASSERT(false);
			case Qt::SolidPattern:  
				return (new SolidBrush(convertQColor2GpColor(qBrush.color())));
			case Qt::LinearGradientPattern:
				return newLineGradientBrush(qBrush);
			case Qt::ConicalGradientPattern:
			case Qt::RadialGradientPattern:
				Q_ASSERT(false);
				qWarning("have not conver this style");
				return NULL;
			case Qt::PathGradientPattern:
				return newPathGradientBrush(qBrush);
			case Qt::TexturePattern:
				return newTexturePatternBrush(qBrush);
			default:
				Q_ASSERT(false);
		} 
		return NULL;
	}
}

KPT_END_NAMESPACE

#endif // Q_OS_WIN