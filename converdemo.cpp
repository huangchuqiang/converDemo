#include "converdemo.h"
#include "kpt_widen_path.h"
#include "kpt_conver_qt2gdiplus.h"
#include <QPaintEngine>
#include <QRect>
 using namespace kpt;
converDemo::converDemo(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}

converDemo::~converDemo()
{
	GdiplusShutdown(gdiplusToken);
}


VOID Example_GetBlend(HDC hdc)
{
	Graphics graphics(hdc);

	// Create a path that consists of a single ellipse.
	GraphicsPath path;
	path.AddEllipse(0, 0, 200, 100);
	path.AddRectangle(Rect(100, 100, 250, 300));

	// Use the path to construct a brush.
	PathGradientBrush pthGrBrush(&path);

	// Set the color at the center of the path to blue.
	pthGrBrush.SetCenterColor(Color(255, 0, 255, 255));

	// Set the color along the entire boundary of the path to aqua.
	Color colors[] = {Color(255, 0, 0, 255)};
	INT count = 1;
	pthGrBrush.SetSurroundColors(colors, &count);

	// Set blend factors and positions for the path gradient brush.
	REAL fac[] = {
		0.0f, 
		0.4f,     // 40 percent of the way from aqua to blue
		0.8f,     // 80 percent of the way from aqua to blue
		1.0f};

		REAL pos[] = {
			0.0f, 
			0.3f,   // 30 percent of the way from the boundary to the center
			0.7f,   // 70 percent of the way from the boundary to the center
			1.0f};

			pthGrBrush.SetBlend(fac, pos, 4);

			// Fill the ellipse with the path gradient brush.
			//graphics.FillEllipse(&pthGrBrush, 0, 0, 200, 100);
			graphics.FillPath(&pthGrBrush, &path);

			// Obtain information about the path gradient brush.
			INT blendCount = pthGrBrush.GetBlendCount();
			REAL* factors = new REAL[blendCount];
			REAL* positions = new REAL[blendCount];

			pthGrBrush.GetBlend(factors, positions, blendCount);
			delete [] factors;
			delete [] positions; 
}


void converDemo::paintEvent( QPaintEvent *event)
{
	
	QPainter painter(this);
	HDC hdc = painter.paintEngine()->getDC();
// 	drawOnGDIplus(hdc);
// 	painter.paintEngine()->releaseDC(hdc);
// 
// 	painter.translate(420, 20);
// 	drawOnQT(painter);
	Example_GetBlend(hdc);


}

void converDemo::drawOnGDIplus(const HDC &hdc)
{
	Q_ASSERT(hdc);
	Graphics graphics(hdc);
	Matrix matrix;
 	QTransform transform;
	transform.translate(0, 20);
	converQTransform2GpMatrix(transform, &matrix);
	graphics.SetTransform(&matrix);

	drawOnNativeGdi(graphics);
	transform.reset();
	transform.translate(210, 20);
	converQTransform2GpMatrix(transform, &matrix);
	graphics.SetTransform(&matrix);
	drawOnTranslateQT(graphics);

}

QBrush newLineGradientBrush()
{
	QLinearGradient gradientBrush(QPointF(0, 0), QPointF(500, 500));
	gradientBrush.setColorAt(0, QColor(Qt::red));
	gradientBrush.setColorAt(0.5, QColor(Qt::green));
	gradientBrush.setColorAt(1, QColor(Qt::blue));
	QBrush brush(gradientBrush);
	QTransform transform;
	transform.translate(100, 0);
	transform.shear(0.15, 0.3);
	transform.scale(0.4, 0.2);
	transform.rotate(0.5);
	brush.setTransform(transform);
	return brush;
}

//原生GDI+ 绘制
void converDemo::drawOnNativeGdi(Graphics &graphics)
{

	Color color1, color2;
	LinearGradientBrush brush(PointF(0, 0), PointF(500, 500), color1, color2);
	brush.SetWrapMode(WrapModeTileFlipXY);
	Color colors[] = {
		Color(255, 255, 0, 0), Color(255, 0, 255, 0), Color(255, 0, 0, 255)
	};
	REAL reals[] = {
		0, 0.5, 1
	};
	brush.SetInterpolationColors(colors, reals, 3);
	QTransform transform;
	transform.translate(100, 0);
	transform.shear(0.15, 0.3);
	transform.scale(0.4, 0.2);
	transform.rotate(0.5);
	Matrix matrix;
	converQTransform2GpMatrix(transform, &matrix);
	brush.MultiplyTransform(&matrix, MatrixOrderAppend);
	RectF rect(0.0f, 0.0f, 200.0f, 400.0f);
	graphics.FillRectangle(&brush, rect);
}


//QT绘制
void converDemo::drawOnQT(QPainter &painter)
{
	QPointF p1(10, 10), p2(10, 200);
	painter.setBrush(newLineGradientBrush());
	painter.setPen(QColor(100, 210, 207.5, 100));
	QRect rect(0.0f, 0.0f, 200.0f, 400.0f);
//	painter.drawLine(p1, p2);
	painter.drawRect(rect);
}
//QT转成GDI+绘制
void converDemo::drawOnTranslateQT(Graphics &graphics)
{
	Color color;
	QColor qcolor(100, 210, 207.5, 100);
	convertQColor2GpColor(qcolor, &color);

	RectF rect(convertQRectF2GpRectF(QRectF(0.0f, 0.0f, 200.0f, 400.0f)));
	Brush* brush = convertQBrush2GpBrush(newLineGradientBrush());
	graphics.FillRectangle(brush, rect);
	delete brush;
}
