#include "converdemo.h"
#include <windows.h>
#include <gdiplus.h>
#include <GdiPlus.h>
using namespace Gdiplus;

#ifdef Q_OS_WIN

KPT_BEGIN_NAMESPACE


DashStyle convertDashStyle(Qt::PenStyle style)
{
	switch (style)
	{
	case Qt::NoPen:
		Q_ASSERT(false);
	case Qt::SolidLine:
		return DashStyleSolid;
	case Qt::DashLine:
		return DashStyleDash;
	case Qt::DotLine:
		return DashStyleDot;
	case Qt::DashDotLine:
		return DashStyleDashDot;
	case Qt::DashDotDotLine:
		return DashStyleDashDotDot;
	case Qt::CustomDashLine:
		return DashStyleCustom;
	default:
		Q_ASSERT(false);
		break;
	}
	return DashStyleSolid;
}

LineCap convertLineCap(Qt::PenCapStyle cap)
{
	switch (cap)
	{
	case Qt::FlatCap:
		return LineCapFlat;
	case Qt::SquareCap:
		return LineCapSquare;
	case Qt::RoundCap:
		return LineCapRound;
	case Qt::TriangleCap:
		return LineCapTriangle;
	default:
		break;
	}
	return LineCapSquare;
}

LineCap convertLineAnchor(Qt::PenAnchorStyle anchorStyle)
{
	switch (anchorStyle)
	{
	case Qt::SquareAnchor:
		return LineCapSquareAnchor;
	case Qt::RoundAnchor:
		return LineCapRoundAnchor;
	case Qt::DiamondAnchor:
		return LineCapDiamondAnchor;
	case Qt::ArrowAnchor:
		return LineCapArrowAnchor;
	case Qt::CustomAnchor:
		return LineCapCustom;
	default:
		break;
	}

	return LineCapSquareAnchor;
}

DashCap convertDashCap(Qt::PenCapStyle dashCap)
{
	switch (dashCap)
	{
	case Qt::FlatCap:
		return DashCapFlat;
	case Qt::RoundCap:
		return DashCapRound;
	case Qt::TriangleCap:
		return DashCapTriangle;
	case Qt::SquareCap:
	default:
		break;
	}
	return DashCapFlat;
}

PenAlignment convertAlign(Qt::PenAlignment align)
{
	switch (align)
	{
	case Qt::PenAlignmentCenter:
		return PenAlignmentCenter;
	case Qt::PenAlignmentInset:
		return PenAlignmentInset;
	case Qt::PenAlignmentOutset:
	default:
		Q_ASSERT(false);
	}
	return PenAlignmentCenter;
}

LineJoin convertLineJoint(Qt::PenJoinStyle jonit)
{
	switch (jonit)
	{
	case Qt::MiterJoin:
		return LineJoinMiter;
	case Qt::BevelJoin:
		return LineJoinBevel;
	case Qt::RoundJoin:
		return LineJoinRound;
	default:
		Q_ASSERT(false);
	}
	return LineJoinMiter;
}

void convertQPen2GpPen(const QPen &qpen, OUT Pen *pPen)
{
	Q_ASSERT(NULL != pPen);
	pPen->SetDashStyle(convertDashStyle(qpen.style()));
	pPen->SetWidth((REAL)qpen.widthF());
	pPen->SetStartCap(convertLineCap(qpen.startCapStyle()));
	pPen->SetEndCap(convertLineCap(qpen.endCapStyle()));
	if (qpen.style() > Qt::SolidLine)
	{
		const QVector<qreal> dashs = qpen.dashPattern();
		const int nCount = dashs.size();
		REAL *pDashs = new REAL[nCount];
		for (int i = 0; i < nCount; i++)
			pDashs[i] = (REAL)dashs.at(i);
		pPen->SetDashPattern(pDashs, nCount);
		pPen->SetDashCap(convertDashCap(qpen.dashCapStyle()));
		pPen->SetDashOffset(qpen.dashOffset());

		delete []pDashs;
	}

	//TODO: CustomAnchor
	Q_ASSERT(qpen.startAnchorStyle() != Qt::CustomAnchor && qpen.endAnchorStyle() != Qt::CustomAnchor);

	pPen->SetAlignment(convertAlign(qpen.alignment()));
	pPen->SetLineJoin(convertLineJoint(qpen.joinStyle()));
	if (qpen.joinStyle() == Qt::MiterJoin)
		pPen->SetMiterLimit((REAL)qpen.miterLimit());
	const QVector<qreal> comps = qpen.compoundArray();
	if (!comps.empty())
	{
		const int nCount = comps.size();
		REAL *pComps = new REAL[nCount];
		for (int i = 0; i < nCount; i++)
			pComps[i] = (REAL)comps.at(i);

		pPen->SetCompoundArray(pComps, nCount);
		delete []pComps;
	}
}

GraphicsPath* createGpPath(const QPainterPath &qpath)
{
	Q_ASSERT(qpath.elementCount() > 1);

	FillMode fillMode = (qpath.fillRule() == Qt::OddEvenFill) ? FillModeAlternate : FillModeWinding;
	int nCount = qpath.elementCount();
	bool bIsClosed = false;
	if ((QPointF)qpath.elementAt(0) == (QPointF)qpath.elementAt(nCount - 1) && qpath.elementAt(nCount - 1).isLineTo())
	{
		bIsClosed = true;
		nCount -= 1;
	}
	PointF *points = new PointF[nCount];
	BYTE *types = new BYTE[nCount];
	for (int i = 0; i < nCount; i++)
	{
		const QPainterPath::Element &element = qpath.elementAt(i);
		if (element.isMoveTo())
		{
			types[i] = Gdiplus::PathPointTypeStart;
			points[i].X = (REAL)element.x;
			points[i].Y = (REAL)element.y;
		}
		else if (element.isLineTo())
		{
			types[i] = Gdiplus::PathPointTypeLine;
			points[i].X = (REAL)element.x;
			points[i].Y = (REAL)element.y;
		}
		else if (element.isCurveTo())
		{
			Q_ASSERT(i + 2 < nCount);
			if (i + 2 >= nCount)
				break;
			const QPainterPath::Element &element2 = qpath.elementAt(i + 1);
			const QPainterPath::Element &element3 = qpath.elementAt(i + 2);
			Q_ASSERT(element2.type == QPainterPath::CurveToDataElement && element3.type == QPainterPath::CurveToDataElement);
			types[i] = Gdiplus::PathPointTypeBezier;
			points[i].X = (REAL)element.x;
			points[i].Y = (REAL)element.y;
			types[i + 1] = Gdiplus::PathPointTypeBezier;
			points[i + 1].X = (REAL)element2.x;
			points[i + 1].Y = (REAL)element2.y;
			types[i + 2] = Gdiplus::PathPointTypeBezier;
			points[i + 2].X = (REAL)element3.x;
			points[i + 2].Y = (REAL)element3.y;
			i += 2;
		}
	}
	if (bIsClosed)
		types[nCount - 1] |= Gdiplus::PathPointTypeCloseSubpath;

	GraphicsPath *gpPath = new GraphicsPath(points, types, nCount, fillMode);

	delete [] points;
	delete [] types;

	return gpPath;
}

QPainterPath convertGpPath2QPath(const GraphicsPath &gpPath)
{
	PathData pathData;
	gpPath.GetPathData(&pathData);
	const int count = pathData.Count;
	const PointF *points = pathData.Points;
	const BYTE *types = pathData.Types;
	QPainterPath path;
	if (count < 1)
		return path;

	path.moveTo(points[0].X, points[0].Y);
	for (int i = 1; i < count ; i++)
	{
		BYTE type = types[i] & Gdiplus::PathPointTypePathTypeMask;
		if (type == Gdiplus::PathPointTypeStart)
			path.moveTo(points[i].X, points[i].Y);
		else if (type == Gdiplus::PathPointTypeLine)
			path.lineTo(points[i].X, points[i].Y);
		else if (type == Gdiplus::PathPointTypeBezier)
		{
			Q_ASSERT(i + 2 < count);
			if (i + 2 >= count)
				break;
			path.cubicTo(points[i].X, points[i].Y, points[i + 1].X, points[i + 1].Y, points[i + 2].X, points[i + 2].Y);
			i += 2;
		}
		if ((types[i] & Gdiplus::PathPointTypeCloseSubpath) == Gdiplus::PathPointTypeCloseSubpath)
			path.closeSubpath();
	}

	return path;
}

/*
QPainterPath widenPath(const QPainterPath& qpath, const QPen& qpen)
{
	KGdiplusInitializer::init();

	QPainterPath retPath;
	Pen gpPen(Color::Black);
	convertQPen2GpPen(qpen, &gpPen);
	GraphicsPath *gpPath = createGpPath(qpath);
	if (NULL == gpPath)
		return retPath;

	Gdiplus::Status status = gpPath->Widen(&gpPen);
	if (Gdiplus::Ok == status)
		retPath = convertGpPath2QPath(*gpPath);

	delete gpPath;

	return retPath;
}
*/

KPT_END_NAMESPACE

#endif // Q_OS_WIN
