#pragma once

#include "../Shape.h"
#include "../WindowManager.h"
#include <climits>
#include <numeric>
#include <optional>

class ShapeDrawer {
    friend class ImagePickerDrawer;

protected:
    unsigned int selectedShapeLineWidth = 2;

    QPixmap &pixmap;

    virtual Shape drawNextShape(ShapeProperties shapeProperties, Shape shape) = 0;

    virtual ShapeProperties calcShapeProps() = 0;

    virtual XPoint getNextShapePosition(ShapeProperties shapeProperties, Dimensions windowDimensions, std::optional<XPoint> lastShapePosition) = 0;

    virtual void drawSelectedShapeIndicator(ShapeProperties shapeProperties, Shape shape) = 0;

    virtual void clearSelectedShapeIndicator(ShapeProperties shapeProperties, Shape shape) = 0;

    virtual void clearShape(ShapeProperties shapeProperties, Shape shape) = 0;

public:
    ShapeDrawer(QPixmap &pixmap) : pixmap(pixmap) {
    }
};
