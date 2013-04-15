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
 
inline Matrix* converQTransform2GpMatrix(const QTransform &qTransform)
 {
	Matrix* pmatrix = new Matrix;
	converQTransform2GpMatrix(qTransform, pmatrix);
	return pmatrix;
 }
//-------------------------------------------------------------------------
//copy �� kpt_variant_image.cpp�ļ�
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
//����������ͬ������д�����ģ�Coding\office\include\drawing\common\blipproperty.h
void GenImgAttributes(IN const KDrawingEffects &aeffects, OUT ImageAttributes *pImAtt)
{
	if (!aeffects.hasEffects())
		return;

	QImageEffects effects = aeffects;
	if (effects.hasDuotone() && effects.isGray())
	{
		QRgb clr1, clr2;
		effects.getDuotone(clr1, clr2);
		//Duotone��Gray�ǲ���ͬʱ���õģ���GenImgEffects
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
		// ���recolor����enable
		if (effects.hasRecolor())
		{
			Gdiplus::Color color(effects.recolor());
			Gdiplus::REAL fA = Gdiplus::REAL((double)color.GetA() / 255);
			Gdiplus::REAL fR = Gdiplus::REAL((double)color.GetR() / 255);
			Gdiplus::REAL fG = Gdiplus::REAL((double)color.GetG() / 255);
			Gdiplus::REAL fB = Gdiplus::REAL((double)color.GetB() / 255);

			// ntfs weight,��ҵ��׼�㷨
			float gray_r = 0.299f;
			float gray_g = 0.587f;
			float gray_b = 0.114f;

			float coefficient_r = 1.0f - fR;
			float coefficient_g = 1.0f - fG;
			float coefficient_b = 1.0f - fB;

			// recolor ��ɫmatrix
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

		// ���recolor��ͼƬ�������������ȣ��Աȶȣ���ô����������ɫ���ͼƬӦ�����ȣ��ͶԱȶȵ���ɫmatrix
		// ���recolor��ͼƬ����gray, ��ô����Ӧ��gray matrix

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
			{//����ʴЧ�����
				fOffset = 0.15f * (propContrast - 1.0f);
			}
			else
			{// '-'�ź���΢��ʹ�õĵ�������----propBright * (propContrast - 1.0f)
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
	if (effects.hasColorKey()) // ���Ϊ~0UL����ʾû��͸��ɫ��color key��
	{
		QRgb colorKey = effects.colorKey();
		quint8 tolerance = effects.colorKeyTolerance();
		//�����ֵҪ��Coding\office\include\drawing\common\blipproperty.hͬ��
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