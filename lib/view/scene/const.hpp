#ifndef APP_CONST_HPP
#define APP_CONST_HPP
#include <qcolor.h>

constexpr float CELL_SIZE = 30.0f;
constexpr float COUNT_CELLS = 100;
constexpr int MAX_ZOOM = 12;


const QColor kPrimaryColor[3] = {
    QColor("#F50057"),
    QColor("#F73378"),
    QColor("#AB003C")
};

const QColor kSecondaryColor[3] = {
    QColor("#303030"),
    QColor("#424242"),
    QColor("#424242")
};

const QColor kPathColor[2] = {
    QColor("#F73378"),
    QColor("#AB003C"),
};

const QColor kBackgroundColor = QColor("#f0f0f0");
const QColor kGridColor = QColor("#dadaea");

const QColor kTextLight = QColor("#ffffff");
const QColor kBgDark = QColor("#000000");

#endif //APP_CONST_HPP
