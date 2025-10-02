#ifndef APP_CONST_HPP
#define APP_CONST_HPP
#include <qcolor.h>

constexpr float CELL_SIZE = 30.0f;
constexpr float COUNT_CELLS = 100;
constexpr int MAX_ZOOM = 12;


const QColor kPrimaryColor[3] = {
    QColor("#3F51B5"),
    QColor("#6573C3"),
    QColor("#3f51B5")
};

const QColor kSecondaryColor[3] = {
    QColor("#A64300"),
    QColor("#FF8D40"),
    QColor("#5ED0BD")
};

const QColor kBackgroundColor = QColor("#f0f0f0");
const QColor kGridColor = QColor("#dadaea");

#endif //APP_CONST_HPP
