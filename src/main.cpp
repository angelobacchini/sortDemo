
#include <QApplication>
#include <QMainWindow>
#include <QStyleFactory>

#include "global.h"
#include "sortWidget.h"

int main(int argc, char *argv[])
{
  QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));
  QApplication qtApp(argc, argv);
  qtApp.setStyle(QStyleFactory::create("Fusion"));

  QMainWindow window;
  window.setStyleSheet(BACKGROUND_COLOR);

  sortWidget widget(NUM_ELEMENTS, &window);
  widget.setFixedHeight(100 + RANGE);

  window.setCentralWidget(&widget);
  window.show();
  qtApp.exec();

  return 0;
}
