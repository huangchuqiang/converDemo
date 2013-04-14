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


VOID Example_GetImage(HDC hdc)
{
	Graphics graphics(hdc);

	// Create a texture brush, and use it to fill an ellipse.
	Image image(L"D:\\HELLO.jpg");
	TextureBrush textureBrush(&image);
	graphics.FillEllipse(&textureBrush, 0, 0, 200, 100);

	// Get the brush's image, and draw that image.
	Image* pImage = textureBrush.GetImage();
	graphics.DrawImage(pImage, 10, 150);
}


void converDemo::paintEvent( QPaintEvent *event)
{
	
	QPainter painter(this);
	HDC hdc = painter.paintEngine()->getDC();
 	drawOnGDIplus(hdc);
 	painter.paintEngine()->releaseDC(hdc);
 
 	painter.translate(420, 20);
 	drawOnQT(painter);
	Example_GetImage(hdc);


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
	QLinearGradient gradientBrush(QPointF(0, 0), QPointF(200, 400));
	gradientBrush.setColorAt(0, QColor(Qt::red));
	gradientBrush.setColorAt(0.2, QColor(Qt::white));
	gradientBrush.setColorAt(0.7, QColor(Qt::black));
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

QBrush newPathGradientBrush()
{
	QPainterPath path;
	path.addEllipse(QPointF(10, 10), 100, 100);
	path.addRect(100, 100, 200, 250);
	QPathGradient pathGradient(path);
	pathGradient.setColorAt(0, QColor(255, 0, 0));
	pathGradient.setColorAt(0.5, QColor(0, 255, 0));
	pathGradient.setColorAt(0.8,QColor(Qt::black));
	pathGradient.setColorAt(1, QColor(0, 0, 255));
	QBrush brush(pathGradient);
	QTransform transform;
	transform.translate(100, 0);
	transform.shear(0.15, 0.10);
	transform.scale(10, 5.2);
	transform.rotate(0.5);
	brush.setTransform(transform);
	return brush;
}

//原生GDI+ 绘制
void converDemo::drawOnNativeGdi(Graphics &graphics)
{

	GraphicsPath path;
	path.AddEllipse(10, 10, 100, 100);
	path.AddRectangle(RectF(100, 100, 200, 250));
	PathGradientBrush* brush = new PathGradientBrush(&path);
	Color colors[] = {
		Color(0, 0, 255), Color(0, 0, 0), Color(0, 255, 0), Color(255, 0, 0)
	};
	REAL reals[] = {
		0, 0.5, 0.8, 1
	};
	brush->SetInterpolationColors(colors, reals, 4);
	Matrix matrix;
	matrix.Translate(100, 0);
	matrix.Shear(0.15, 0.3);
	matrix.Scale(0.4, 0.2);
	matrix.Rotate(0.5);
//	brush->MultiplyTransform(&matrix, MatrixOrderAppend);
	RectF rect(convertQRectF2GpRectF(QRectF(0.0f, 0.0f, 200.0f, 400.0f)));
	graphics.FillRectangle(brush, rect);
	delete brush;

}


//QT绘制
void converDemo::drawOnQT(QPainter &painter)
{
// 	QPointF p1(10, 10), p2(10, 200);
// 	painter.setBrush(newPathGradientBrush());
// 	painter.setPen(QColor(100, 210, 207.5, 100));
// 	QRect rect(0.0f, 0.0f, 200.0f, 400.0f);
// //	painter.drawLine(p1, p2);
// 	painter.drawRect(rect);
	QImage qimage("D:\\HELLO.jpg");
	QVector<QRgb> vecColorTable(qimage.colorTable());
	painter.drawImage(QPointF(0, 0), qimage);
}
//QT转成GDI+绘制
void converDemo::drawOnTranslateQT(Graphics &graphics)
{
	Color color;
	QColor qcolor(100, 210, 207.5, 100);
	convertQColor2GpColor(qcolor, &color);

	RectF rect(convertQRectF2GpRectF(QRectF(0.0f, 0.0f, 200.0f, 400.0f)));
	Brush* brush = convertQBrush2GpBrush(newPathGradientBrush());
	graphics.FillRectangle(brush, rect);
	delete brush;
}
