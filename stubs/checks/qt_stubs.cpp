#include <GeoItem/AllItems>
#include <SPGeoItem/Polygon>
#include <GeoGUI/GeoCoordView>
#include <FileSystemUtils/CSVFile>
#include <QApplication>
#include <QBuffer>
#include <memory>
#include <string>

template <class Operation>
bool isUnavailable(Operation operation, const char *name) {
    try { operation(); }
    catch (const std::logic_error &error) {
        const std::string message(error.what());
        return message.find("Demo stub:") != std::string::npos && message.find(name) != std::string::npos;
    }
    return false;
}
int main(int argc, char **argv) {
    QApplication application(argc, argv);
    Ics::SP::QIcon icon;
    Ics::SP::QLabel label;
    Ics::SP::QPolygon polygon;
    label.setFollowObject(&icon);
    label.texts.push_front("demo");
    label.id = "Start";
    icon.geoPos = Geodesy::Math::PZ_903d(Degree(55), Degree(37), 0);
    icon.image = QPixmap(1, 1).scaled({64, 64}, Qt::KeepAspectRatio);
    polygon.points.append(icon.geoPos);
    polygon.borderPen.setColor(Qt::black);
    polygon.backgroundColor = Qt::green;
    polygon.setStyle(":/zoneStyle.ini");
    polygon.trueOrthodromes = true;
    polygon.needShowPoints = true;
    polygon.isEditable = true;
    polygon.isVisible = false;
    polygon.zValue = 100;
    std::unique_ptr<Ics::SP::QPolygon> clone(polygon.clone());
    if (clone->points.size() != 1) return 1;
    QBuffer buffer;
    buffer.open(QIODevice::ReadWrite);
    QDataStream stream(&buffer);
    if (!isUnavailable([&] { polygon.updateGeo(); }, "updateGeo") ||
        !isUnavailable([&] { label.updateView(); }, "updateView") ||
        !isUnavailable([&] { Ics::SP::addGeoItem(&polygon); }, "addGeoItem") ||
        !isUnavailable([&] { Ics::SP::changeCenterAnimated(icon.geoPos); }, "changeCenterAnimated") ||
        !isUnavailable([&] { polygon.getCenter(); }, "getCenter") ||
        !isUnavailable([&] { stream << polygon; }, "serialization") ||
        !isUnavailable([&] { stream >> polygon; }, "deserialization") ||
        !isUnavailable([&] { GeoGUI::toString(icon.geoPos.to2d()); }, "GeoGUI") ||
        !isUnavailable([] { CSVFile file("unused.csv", CSVFile::SYNC_OFF, ',');
            for (auto &row : file) { (void)row; } }, "CSVFile")) return 2;
}
