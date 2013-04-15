#ifndef KPT_GDIPLUSFROMQT_H__
#define KPT_GDIPLUSFROMQT_H__

#include "kpt_conver_qt2gdiplus.h"

void setPenDashPattern(const QPen& qpen, Pen& gpen)
{
  QVector<qreal> vecDashPattern;
	vecDashPattern = qpen.dashPattern();
	int vecSize = vecDashPattern.size();
	if(vecSize)
	{
		REAL* dashArray = new REAL[vecSize];
		memset(dashArray, 0, vecSize*sizeof(REAL));
		for(int i = 0; i != vecSize; ++i)
		{
			REAL delta = 0;
			if (qpen.capStyle() == Qt::SquareCap)
				delta = 1.0;
			if(i%2)
				dashArray[i] = vecDashPattern[i] - delta;
			else
				dashArray[i] = vecDashPattern[i] + delta;
		}
		gpen.SetDashPattern(dashArray, vecSize);
		delete []dashArray;
	}
}

void setPenCompoundArray(const QPen& qpen, Pen& gpen)
{
	QVector<qreal> vecComArray;
	vecComArray = qpen.compoundArray();
	int vecSize = vecComArray.size();
	if(vecSize)
	{
		REAL* comArray = new REAL[vecSize];
		memset(comArray, 0, vecSize*sizeof(REAL));
		for(int i = 0; i != vecSize; ++i)
		{
			comArray[i] = vecComArray[i];
		}
		gpen.SetCompoundArray(comArray, vecSize);
		delete []comArray;
	}
}
void convertQPenToPen(const QPen& qpen, Pen& gpen)
{
	gpen.SetBrush(kpt::convertQBrush2GpBrush(qpen.brush()));
	gpen.SetWidth(qpen.widthF());
	gpen.SetDashStyle(kpt::convertDashStyle(qpen.style()));
	gpen.SetDashOffset(qpen.dashOffset());
	gpen.SetLineJoin(kpt::convertLineJoint(qpen.joinStyle()));
	gpen.SetMiterLimit(qpen.miterLimit());
	gpen.SetLineCap(kpt::convertLineCap(qpen.startCapStyle()),
					kpt::convertLineCap(qpen.endCapStyle()),
					kpt::convertDashCap(qpen.dashCapStyle()));
 	setPenDashPattern(qpen, gpen);
	setPenCompoundArray(qpen, gpen);
	gpen.SetAlignment(kpt::convertAlign(qpen.alignment()));
}

void setFontHeight(LONG& lfHeight, const QFont& qfont, HDC hdc)
{
	if(qfont.pixelSize() == -1)
	{
		lfHeight = -qfont.pointSizeF() * 
				   ::GetDeviceCaps(hdc, LOGPIXELSY) / 72;
	}
	else
	{
		lfHeight = -qfont.pixelSize();
	}
}

void setFontWeight(LONG& lfWeight, LONG qweight)
{
	switch (qweight)
	{
	case QFont::Light:
		lfWeight = FW_LIGHT;
		break;
	case QFont::DemiBold:
		lfWeight = FW_DEMIBOLD;
		break;
	case QFont::Bold:
		lfWeight = FW_BOLD;
		break;
	case QFont::Normal:
	default:
		lfWeight = FW_NORMAL;
	}
}

Font* convertQFontToFont(const QFont& qfont, HDC hdc)
{
	return &Font(hdc,qfont.handle());
}

LOGFONTA convertQFontToFontA(const QFont& qfont, HDC hdc)
{
	LOGFONTA font;
	setFontHeight(font.lfHeight, qfont, hdc);
	setFontWeight(font.lfWeight, qfont.weight());
	font.lfItalic = qfont.italic();
	font.lfUnderline = qfont.underline();
	font.lfEscapement = 0;
	font.lfCharSet = DEFAULT_CHARSET;
	int nSize = qfont.family().size();
	if(nSize > LF_FACESIZE)
		nSize = LF_FACESIZE;
	LPSTR pchName = font.lfFaceName;
	memset(pchName, 0, LF_FACESIZE * sizeof(char));
	if(qfont.verticalMetrics())
	{
		strcpy(pchName, "@");
		++ pchName;
	}
	memcpy(pchName, qfont.family().data(), nSize * sizeof(QChar));
	return font;
}

LOGFONTW convertQFontToFontW(const QFont& qfont, HDC hdc)
{
	LOGFONTW font;
	setFontHeight(font.lfHeight, qfont, hdc);
	setFontWeight(font.lfWeight, qfont.weight());
	font.lfItalic = qfont.italic();
	font.lfUnderline = qfont.underline();
	font.lfEscapement = 0;
	font.lfCharSet = DEFAULT_CHARSET;
	int nSize = qfont.family().size();
	if(nSize > LF_FACESIZE)
		nSize = LF_FACESIZE;
	LPWSTR pchName = font.lfFaceName;
	memset(pchName, 0, LF_FACESIZE * sizeof(wchar_t));
	if(qfont.verticalMetrics())
	{
		wcscpy(pchName, L"@");
		++ pchName;
	}
	memcpy(pchName, qfont.family().data(), nSize * sizeof(QChar));
	return font;
}
#endif
