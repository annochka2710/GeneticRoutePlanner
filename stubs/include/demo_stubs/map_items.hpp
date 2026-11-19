#pragma once
#include "geodesy.hpp"
#include <QColor>
#include <QDataStream>
#include <QPen>
#include <QPixmap>
#include <QStringList>
#include <QVector>

namespace Ics { namespace SP {
// Geographic SDK items, not Qt's namesake classes.
class GeoItem {
public:
    virtual ~GeoItem() = default;
    bool isVisible = true;
    double zValue = 0;
    QString style;
    void setStyle(const QString &path) { style = path; }
    void updateGeo() { demo_stubs::unavailable("GeoItem::updateGeo"); }
    void updateView() { demo_stubs::unavailable("GeoItem::updateView"); }
};
class QIcon : public GeoItem {
public:
    QPixmap image;
    Geodesy::Math::PZ_903d geoPos;
};
class QLabel : public GeoItem {
public:
    QString id;
    QStringList texts;
    void setFollowObject(QIcon *item) { followObject_ = item; }
private:
    QIcon *followObject_ = nullptr; // Non-owning.
};
class QPolygon : public GeoItem {
public:
    QVector<Geodesy::Math::PZ_903d> points;
    QPen borderPen;
    QColor backgroundColor;
    bool trueOrthodromes = false;
    bool needShowPoints = false;
    bool isEditable = false;
    QPolygon *clone() const { return new QPolygon(*this); }
    Geodesy::Math::PZ_903d getCenter() const {
        demo_stubs::unavailable("SPGeoItem::QPolygon::getCenter");
    }
};
// The original binary format is unknown; do not invent a replacement.
inline QDataStream &operator<<(QDataStream &, const QPolygon &) {
    demo_stubs::unavailable("SPGeoItem::QPolygon serialization");
}
inline QDataStream &operator>>(QDataStream &, QPolygon &) {
    demo_stubs::unavailable("SPGeoItem::QPolygon deserialization");
}
inline void addGeoItem(GeoItem *) { demo_stubs::unavailable("GeoItem::addGeoItem"); }
inline void changeCenterAnimated(const Geodesy::Math::PZ_903d &) {
    demo_stubs::unavailable("GeoItem::changeCenterAnimated");
}
}}
