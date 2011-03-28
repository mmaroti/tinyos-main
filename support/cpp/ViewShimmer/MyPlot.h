#ifndef MYPLOT_H
#define MYPLOT_H

#include <qwt_plot.h>

class QwtPlot;

class MyPlot : public QwtPlot
{
public:
  MyPlot( QWidget *parent=0, char *name=0 );
};




#endif // MYPLOT_H
