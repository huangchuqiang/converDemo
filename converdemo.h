#ifndef CONVERDEMO_H
#define CONVERDEMO_H

#include <windows.h>
#include <gdiplus.h>
#include <GdiPlus.h>
using namespace Gdiplus;

#include <QtGui/QMainWindow>
#include "ui_converdemo.h"

#include <QPen>
#include <QBrush>
#include <QPainter>

#define KPT_BEGIN_NAMESPACE namespace kpt {
#define KPT_END_NAMESPACE }
#define KPT_USE_NAMESPACE using namespace ::kpt;
#define  OUT

class converDemo : public QMainWindow
{
	Q_OBJECT

public:
	converDemo(QWidget *parent = 0, Qt::WFlags flags = 0);
	~converDemo();

protected:
	void paintEvent( QPaintEvent *event); 


private:
	Ui::converDemoClass ui;

	void drawOnQT(QPainter &painter);
	void drawOnGDIplus(const HDC &hdc);
	void drawOnNativeGdi(Graphics &graphics);
	void drawOnTranslateQT(Graphics &graphics);
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;

};

#endif // CONVERDEMO_H
