#pragma once

#include <X11/Xutil.h>
#include <X11/Xlib.h>
#include <X11/extensions/xf86vmode.h>
#include <functional>
#include <memory>

#include "../image/image.h"
#include "Shape.h"
#include "drawer/ShapeDrawer.h"
#include "WindowManager.h"
#include "drawer/ShapeDrawerFactory.h"

enum class ImagePickerMove {
    NONE,
    LEFT,
    RIGHT,
    UP,
    DOWN,

    HOME,
    END,

    LINE
};

class ImagePickerDrawer {
private:
    WindowManager *windowManager;

    ShapeProperties shapeProperties;
    ShapeDrawer* shapeDrawer;

    Shape *selectedShape;
    int page = 0;

    std::vector<Image> *hotkeys;
    std::vector<Shape> shapes;

    std::vector<Image>::iterator getPageImageStart();

    int getImagePage(long index);

    void goToImage(long hotkeyIdx);

    std::function<bool(Image*)> filter;

public:
    ImagePickerDrawer(WindowManager* windowManager, std::vector<Image> *hotkeys);

    void drawFrame(Image* selectedImage);

    bool move(ImagePickerMove move, unsigned int steps = 1);

    void setFilter(std::function<bool(Image *)> filter);

    Image* getSelectedImage();
};

