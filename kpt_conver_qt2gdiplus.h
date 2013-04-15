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
/*
//根据主干相同函数改写过来的：Coding\office\include\drawing\common\blipproperty.h
void GenImgAttributes(IN const KDrawingEffects &aeffects, OUT ImageAttributes *pImAtt)
{
	if (!aeffects.hasEffects())
		return;

	QImageEffects effects = aeffects;
	if (effects.hasDuotone() && effects.isGray())
	{
		QRgb clr1, clr2;
		effects.getDuotone(clr1, clr2);
		//Duotone和Gray是不会同时设置的，见GenImgEffects
		Q_ASSERT((clr1 == kpt::Color::Black && clr2 == kpt::Color::White));
		if ((clr1 == kpt::Color::Black && clr2 == kpt::Color::White))
		{
			effects.unsetDuotone();
			effects.setGray(true);
		}
	}

	if (effects.hasSubstituteColor())
	{
		Gdiplus::Color color(effects.substituteColor());
		Gdiplus::REAL fA = Gdiplus::REAL((double)color.GetA() / 255);
		Gdiplus::REAL fR = Gdiplus::REAL((double)color.GetR() / 255);
		Gdiplus::REAL fG = Gdiplus::REAL((double)color.GetG() / 255);
		Gdiplus::REAL fB = Gdiplus::REAL((double)color.GetB() / 255);
		Gdiplus::ColorMatrix colorMatrix =
		{
			0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
			0.0f,  0.0f, 0.0f, 0.0f, 0.0f,
			0.0f,  0.0f, 0.0f, 0.0f, 0.0f,
			0.0f,  0.0f, 0.0f, fA, 0.0f,
			fR, fG, fB, 0.0f, 1.0f};
			pImAtt->SetColorMatrix(&colorMatrix,
				ColorMatrixFlagsDefault,
				ColorAdjustTypeDefault);
	}
	else
	{
		// 如果recolor属性enable
		if (effects.hasRecolor())
		{
			Gdiplus::Color color(effects.recolor());
			Gdiplus::REAL fA = Gdiplus::REAL((double)color.GetA() / 255);
			Gdiplus::REAL fR = Gdiplus::REAL((double)color.GetR() / 255);
			Gdiplus::REAL fG = Gdiplus::REAL((double)color.GetG() / 255);
			Gdiplus::REAL fB = Gdiplus::REAL((double)color.GetB() / 255);

			// ntfs weight,工业标准算法
			float gray_r = 0.299f;
			float gray_g = 0.587f;
			float gray_b = 0.114f;

			float coefficient_r = 1.0f - fR;
			float coefficient_g = 1.0f - fG;
			float coefficient_b = 1.0f - fB;

			// recolor 颜色matrix
			Gdiplus::ColorMatrix colorMatrix =
			{
				gray_r*coefficient_r, gray_r*coefficient_g, gray_r*coefficient_b, 0.0f, 0.0f,
				gray_g*coefficient_r, gray_g*coefficient_g, gray_g*coefficient_b, 0.0f, 0.0f,
				gray_b*coefficient_r, gray_b*coefficient_g, gray_b*coefficient_b, 0.0f, 0.0f,
				0.0f,  0.0f, 0.0f, fA,  0.0f,
				fR,    fG,    fB,   0.0f, 1.0f
			};

			pImAtt->SetColorMatrix(&colorMatrix, 
				ColorMatrixFlagsDefault,
				ColorAdjustTypeDefault);
		}

		// 如果recolor后，图片还有设置了亮度，对比度，那么，继续对着色后的图片应用亮度，和对比度得颜色matrix
		// 如果recolor后，图片设置gray, 那么继续应用gray matrix

		if (effects.hasAlpha())
		{
			Gdiplus::ColorMatrix alphaMatrix =
				MakeColorMatrix_Alpha((BYTE)effects.alpha());
			pImAtt->SetColorMatrix(&alphaMatrix);
		}
		else if (effects.hasDuotone())
		{
			QRgb clr1, clr2;
			effects.getDuotone(clr1, clr2);
			Gdiplus::ColorMatrix duotoneMatrix =
				MakeColorMatrix_Duotone(clr1, clr2);
			pImAtt->SetColorMatrix(&duotoneMatrix);
		}
		else if (effects.brightness() != 0 || effects.contrast() != 1)
		{
			Gdiplus::REAL propBright =effects.brightness() / 2.0;
			Gdiplus::REAL propContrast = effects.contrast();

			Gdiplus::REAL fOffset = 0.0f;
			if (propContrast < 1.0f && propBright > 0.0f)
			{//跟冲蚀效果相关
				fOffset = 0.15f * (propContrast - 1.0f);
			}
			else
			{// '-'号后是微软使用的调整因子----propBright * (propContrast - 1.0f)
				fOffset = 0.5f * (propContrast - 1.0f) - propBright * (propContrast - 1.0f);
			}
			Gdiplus::REAL fContrast = propContrast;
			Gdiplus::REAL fBrightness = 2.0f * propBright;
			Gdiplus::REAL fOffsetVal = fBrightness - fOffset;

			if (effects.isGray() || effects.hasBilevel())
			{
				Gdiplus::ColorMatrix colorMatrix =
				{
					0.299f*fContrast, 0.299f*fContrast, 0.299f*fContrast, 0.0f, 0.0f,
					0.587f*fContrast, 0.587f*fContrast, 0.587f*fContrast, 0.0f, 0.0f,
					0.114f*fContrast, 0.114f*fContrast, 0.114f*fContrast, 0.0f, 0.0f,
					0.0f,   0.0f,   0.0f,   1.0f,   0.0f,
					fOffsetVal,  fOffsetVal,   fOffsetVal,   0.0f, 1.0f
				};
				pImAtt->SetColorMatrix(&colorMatrix,
					ColorMatrixFlagsDefault,
					ColorAdjustTypeDefault);
			}
			else 
			{
				Gdiplus::ColorMatrix colorMatrix =
				{
					fContrast, 0.0f, 0.0f, 0.0f, 0.0f,
					0.0f, fContrast, 0.0f, 0.0f, 0.0f,
					0.0f, 0.0f, fContrast, 0.0f, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
					fOffsetVal, fOffsetVal, fOffsetVal, 0.0f, 1.0f
				};

				pImAtt->SetColorMatrix(&colorMatrix,
					ColorMatrixFlagsDefault,
					ColorAdjustTypeDefault);
			}
		}
		else if (effects.isGray() || effects.hasBilevel())
		{
			Gdiplus::ColorMatrix colorMatrix =
			{
				0.299f, 0.299f, 0.299f, 0.0f, 0.0f,
				0.587f, 0.587f, 0.587f, 0.0f, 0.0f,
				0.114f, 0.114f, 0.114f, 0.0f, 0.0f,
				0.0f,   0.0f,   0.0f,   1.0f,   0.0f,
				0.0,  0.0,   0.0,   0.0f, 1.0f
			};
			pImAtt->SetColorMatrix(&colorMatrix,
				ColorMatrixFlagsDefault,
				ColorAdjustTypeDefault);

		}
	}
	if (effects.hasColorKey()) // 如果为~0UL，表示没有透明色（color key）
	{
		QRgb colorKey = effects.colorKey();
		quint8 tolerance = effects.colorKeyTolerance();
		//这里的值要跟Coding\office\include\drawing\common\blipproperty.h同步
		Q_ASSERT(tolerance == 1 || tolerance == 12);
		if (tolerance > 1) {
			int r = max(0, qRed(colorKey) - tolerance),
				g = max(0, qGreen(colorKey) - tolerance),
				b = max(0, qBlue(colorKey) - tolerance);
			Gdiplus::Color low(r, g, b);
			r = min(r + tolerance * 2, 255), g = min(g + tolerance * 2, 255), b = min(b + tolerance * 2, 255);
			Gdiplus::Color high(r, g, b);
			pImAtt->SetColorKey(low, high, ColorAdjustTypeDefault);
		} else {
			Gdiplus::Color key(colorKey);
			pImAtt->SetColorKey(key, key, ColorAdjustTypeDefault);
		}
	}
	if (effects.hasBilevel())
		pImAtt->SetThreshold(effects.bilevel(), Gdiplus::ColorAdjustTypeDefault);
}
*/
// int   GetEncoderClsid(const   WCHAR*   format,   CLSID*   pClsid) 
// { 
// 	UINT     num   =   0;                     //   number   of   image   encoders 
// 	UINT     size   =   0;                   //   size   of   the   image   encoder   array   in   bytes 
// 
// 	ImageCodecInfo*   pImageCodecInfo   =   NULL; 
// 
// 	GetImageEncodersSize(&num,   &size); 
// 	if(size   ==   0) 
// 		return   -1;     //   Failure 
// 
// 	pImageCodecInfo   =   (ImageCodecInfo*)(malloc(size)); 
// 	if(pImageCodecInfo   ==   NULL) 
// 		return   -1;     //   Failure 
// 
// 	GetImageEncoders(num,   size,   pImageCodecInfo); 
// 
// 	for(UINT   j   =   0;   j   <   num;   ++j) 
// 	{ 
// 		if(   wcscmp(pImageCodecInfo[j].MimeType,   format)   ==   0   ) 
// 		{ 
// 			*pClsid   =   pImageCodecInfo[j].Clsid; 
// 			free(pImageCodecInfo); 
// 			return   j;     //   Success 
// 		}         
// 	} 
// 
// 	free(pImageCodecInfo); 
// 	return   -1;     //   Failure 
// } 
inline Image* converQImage2GpImage(const QImage &qImage)
{
	Bitmap* pBitmap = createBmpFromQImage(qImage);
	if (NULL == pBitmap)
	{
		pBitmap = createBmpFromQImage(qImage.convertToFormat(QImage::Format_ARGB32));
	}

	Q_ASSERT(pBitmap);

// 	CLSID encoderClsid;
// 	GetEncoderClsid(L"image/jpeg",&encoderClsid);
// 	pBitmap->Save(L"d:\\hello11.jpg",&encoderClsid);
	return pBitmap;
}
//-------------------------------------------------------------------------
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

	Matrix matrix;
	converQTransform2GpMatrix(qBrush.transform(), &matrix);
	retLinearGrBrush->MultiplyTransform(&matrix, MatrixOrderAppend);
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