#include "converdemo.h"
#include <windows.h>
#include <gdiplus.h>
#include <GdiPlus.h>
using namespace Gdiplus;

#include<functional>

#ifdef Q_OS_WIN

KPT_BEGIN_NAMESPACE	
//-------------------------------------------------------------------------
// ���ת��
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
// ���ε�ת��
void convertQRectF2GpRectF(const QRectF &qRect, OUT RectF* pRect)
{
	Q_ASSERT(NULL == pRect);
	//��ʼ��
	qreal x(0), y(0), width(0), height(0);
	qRect.getCoords(&x, &y, &width, &height);
	pRect->X = x;
	pRect->Y = y;
	pRect->Width = width;
	pRect->Height = height;
}
RectF convertQRectF2GpRectF(const QRectF &qRect)
{
	//��ʼ��
	qreal x(0), y(0), width(0), height(0);
	qRect.getCoords(&x, &y, &width, &height);
	return RectF(x, y, width, height);
}
//-------------------------------------------------------------------------
//��ɫ��ת��
void convertQColor2GpColor(const QColor &qColor, OUT Color* pColor)
{
	Q_ASSERT(pColor);
	//��ʼ��
	qreal red(0), green(0), blue(0), alpha(0);
	qColor.getRgbF(&red, &green, &blue, &alpha);

	//��λ�����ARGB��ֵ
	ARGB argb = (int)(255 * alpha);
// ������Ĵ���õ��Ľ������
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
	//��ʼ��
	qreal red(0), green(0), blue(0), alpha(0);
	qColor.getRgbF(&red, &green, &blue, &alpha);
	return Color(255 * alpha, 255 * red, 255 * green, 255 * blue);
 }
//-------------------------------------------------------------------------
// QTransform ת���� Matrix,ԭ����� QMatrix ת�� Matrix�ģ�����QT�ĵ�˵QMatrix�ǹ�ʱ����
inline void converQTransform2GpMatrix(const QTransform &qTransform, OUT Matrix* pMatrix)
{
	Q_ASSERT(pMatrix);

	pMatrix->SetElements(qTransform.m11(), qTransform.m12(), qTransform.m21(), qTransform.m22(), qTransform.dx(), qTransform.dy());
}
 
 Matrix* converQTransform2GpMatrix(const QTransform &qTransform)
 {
	Matrix* matrix = new Matrix;

	Q_ASSERT(matrix);
	matrix->Translate(qTransform.dx(), qTransform.dy());
	matrix->Shear(qTransform.m21(), qTransform.m12());
	matrix->Scale(qTransform.m11(), qTransform.m22());
	return matrix;
 } 
//------------------------------------------------------------------------
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

//�뺯��newPathGradientBrush����ɫ���Ʋ�ͬ�Ĳ���
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

	//�õ������������
	PointF pointSart(convertQPointF2GpPointF(qlineGradinet->start())),
		pointStop(convertQPointF2GpPointF(qlineGradinet->finalStop()));
	//��ʼ�����ֿ���ɫ
	Color colorStart, colorStop;
	LinearGradientBrush* retLinearGrBrush = new LinearGradientBrush(
								pointSart, pointStop, colorStart, colorStop);
	retLinearGrBrush->SetWrapMode(WrapModeTileFlipXY);
	setGradientBrushInterpolationColors(qlineGradinet->stops(), retLinearGrBrush, fnLinearGradientOperation);

	Matrix matrix;
	converQTransform2GpMatrix(qBrush.transform(), &matrix);
	retLinearGrBrush->MultiplyTransform(&matrix, MatrixOrderAppend);
	return retLinearGrBrush;
}

//�뺯��newLineGradientBrush����ɫ���Ʋ�ͬ�Ĳ���
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
	GraphicsPath* gpath = createGpPath(qpath);		//����һ��new��GraphicsPath����
	PathGradientBrush* retPathGrBrush = new PathGradientBrush(gpath);
	retPathGrBrush->SetWrapMode(WrapModeTileFlipXY);
	setGradientBrushInterpolationColors(qpathGradinet->stops(), retPathGrBrush, fnPathGradientOperation);
//	����Ĵ����Ѿ����ΪsetGradientBrushInterpolationColors ��fnPathGradientOperation��������
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
	Matrix matrix;
	converQTransform2GpMatrix(qBrush.transform(), &matrix);
	retPathGrBrush->MultiplyTransform(&matrix, MatrixOrderAppend);
	delete gpath;
	return retPathGrBrush;
}

inline TextureBrush* newTexturePatternBrush(const QBrush &qBrush)
{
	
	   return NULL;
}

/*
 @remark convertQBrush2GpBrush����û�ж� ConicalGradientPattern ���ͺ� RadialGradientPattern ���ͽ���ת��
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