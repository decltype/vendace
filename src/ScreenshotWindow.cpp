#include <QtWidgets>
#include <QtGui>
#include <QRect>
#include <QSize>

#include <iostream>

#include "ScreenshotWindow.hpp"
#include "ResultWindow.hpp"

ScreenshotWindow::ScreenshotWindow() : mCropper(NULL) {
    // Get the entire screen size
    int w = 0, h = 0;
    for (int i = 0; i < QGuiApplication::screens().size(); i++) {
        QScreen *screen = QGuiApplication::screens().at(i);
        w += screen->geometry().width();
        h = std::max(h, screen->geometry().height());
    }

    // Snap
    // FIXME: Is this safe with multiple monitors?
    mScreenshot = QGuiApplication::primaryScreen()->grabWindow(0, 0, 0, w, h);

    // Full screen and no title bar
    setWindowFlags(Qt::FramelessWindowHint | Qt::CustomizeWindowHint);

    createUi();
    move(-99999999, -99999999); // FIXME hack
}

void ScreenshotWindow::createUi() {
    mScreenshotLabel = new QLabel;
    mScreenshotLabel->setPixmap(mScreenshot);
    setCentralWidget(mScreenshotLabel);

    mHelpLabel = new QLabel(mScreenshotLabel);
    mHelpLabel->setText(tr("Click, hold and drag to crop.\nClick inside the drawn rectangle to finish."));
    mHelpLabel->setStyleSheet("QLabel { background: #000; color: #fff; font-size: 18px; padding: 10px; }");
    mHelpLabel->move(10, 10);
}

void ScreenshotWindow::keyReleaseEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
        close();
    }
}

void ScreenshotWindow::selectCrop() {
    ResultWindow *result = new ResultWindow(mScreenshot.copy(mCropper->geometry()));
    result->show();
    close();
}

void ScreenshotWindow::mousePressEvent(QMouseEvent *event) {
    QPoint pos = event->pos();
    if (mCropper != NULL && mCropper->geometry().contains(pos)) {
        // Click inside current rectangle
        selectCrop();
    } else {
        // Hide current cropper
        if (mCropper != NULL) {
            mCropper->hide();
            delete mCropper;
        }

        // Create new cropper
        mCropOrigin = pos;
        mCropper = new QRubberBand(QRubberBand::Rectangle, mScreenshotLabel);
        mCropper->setGeometry(QRect(mCropOrigin, QSize()));
        mCropper->show();
    }
}

void ScreenshotWindow::mouseMoveEvent(QMouseEvent *event) {
    mCropper->setGeometry(QRect(mCropOrigin, event->pos()).normalized());
}