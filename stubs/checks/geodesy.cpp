#include <Geodesy/Math/Ellipsoid>
#include <Geodesy/Math/GnomonicPoint>
#include <Geodesy/Math/PZ_90>
#include <Geodesy/Math/Spheroid>
#include <iostream>
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
int main() {
    using namespace Geodesy::Math;
    const PZ_902d a(Degree(55), Degree(37));
    PZ_90 b(Degree(56), Degree(38));
    b.setB(b.b() + IMath::VRadian(1e-6));
    b.setL(b.l() + IMath::VRadian(1e-6));
    if (PZ_902d().isValid() || !a.isValid() || !b.isValid() || !(a.to3d().to2d() == a) ||
        PZ_902d(Degree(100), Degree(0)).isValid()) return 1;
    using Curve = Ellipsoid::OrthodromicCurveEx<PZ_902d>;
    const Curve curve(a, b);
    const Spheroid::OrthodromicPolygon<PZ_902d> polygon({a, b});
    const Spheroid::OrthodromicPolygon<PZ_902d> other(std::vector<PZ_902d>{b, a});
    const RotatedSpheroid sphere(PZ_90_02GeoPoint(a));
    const GnomonicSpherePoint projected(PZ_90_02GeoPoint(b), sphere);
    const GnomonicGeometry geometry(sphere);
    if (!isUnavailable([&] { curve.length(); }, "::length") ||
        !isUnavailable([&] { Curve::length(a, b); }, "::length") ||
        !isUnavailable([&] { curve.forward(100); }, "::forward") ||
        !isUnavailable([&] { Curve::directCalculate(a, Degree(90), 100); }, "::directCalculate") ||
        !isUnavailable([&] { curve.getPoints(10); }, "::getPoints") ||
        !isUnavailable([&] { polygon.intersects(other); }, "::intersects") ||
        !isUnavailable([&] { projected.toPoint2d(); }, "::toPoint2d") ||
        !isUnavailable([&] { geometry.angle({}, {}, {}); }, "::angle")) return 2;
    std::cout << "Geodesy interfaces compile; unavailable operations report a stub.\n";
}
