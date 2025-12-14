#include "ImageUtils.h"
#include <QPainter>
#include <QPainterPath>
#include <QGraphicsBlurEffect>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

QPixmap ImageUtils::loadImage(const QString& path, int width, int height)
{
    QPixmap pixmap(path);
    
    if (pixmap.isNull()) {
        return QPixmap();
    }
    
    if (width > 0 && height > 0) {
        return pixmap.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    } else if (width > 0) {
        return pixmap.scaledToWidth(width, Qt::SmoothTransformation);
    } else if (height > 0) {
        return pixmap.scaledToHeight(height, Qt::SmoothTransformation);
    }
    
    return pixmap;
}

QPixmap ImageUtils::createCircularImage(const QPixmap& source)
{
    int size = qMin(source.width(), source.height());
    QPixmap result(size, size);
    result.fill(Qt::transparent);
    
    QPainter painter(&result);
    painter.setRenderHint(QPainter::Antialiasing);
    
    QPainterPath path;
    path.addEllipse(0, 0, size, size);
    painter.setClipPath(path);
    
    QPixmap scaled = source.scaled(size, size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    painter.drawPixmap(0, 0, scaled);
    
    return result;
}

QPixmap ImageUtils::createRoundedImage(const QPixmap& source, int radius)
{
    QPixmap result(source.size());
    result.fill(Qt::transparent);
    
    QPainter painter(&result);
    painter.setRenderHint(QPainter::Antialiasing);
    
    QPainterPath path;
    path.addRoundedRect(0, 0, source.width(), source.height(), radius, radius);
    painter.setClipPath(path);
    
    painter.drawPixmap(0, 0, source);
    
    return result;
}

QPixmap ImageUtils::applyBlur(const QPixmap& source, int radius)
{
    QGraphicsScene scene;
    QGraphicsPixmapItem item;
    item.setPixmap(source);
    
    QGraphicsBlurEffect* blur = new QGraphicsBlurEffect;
    blur->setBlurRadius(radius);
    item.setGraphicsEffect(blur);
    
    scene.addItem(&item);
    
    QPixmap result(source.size());
    result.fill(Qt::transparent);
    
    QPainter painter(&result);
    scene.render(&painter);
    
    return result;
}

QPixmap ImageUtils::createThumbnail(const QPixmap& source, int size)
{
    return source.scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

bool ImageUtils::saveImage(const QPixmap& pixmap, const QString& path, const QString& format)
{
    return pixmap.save(path, format.toUtf8().constData());
}

QPixmap ImageUtils::resizeImage(const QPixmap& source, int width, int height, Qt::AspectRatioMode mode)
{
    return source.scaled(width, height, mode, Qt::SmoothTransformation);
}

QPixmap ImageUtils::createGradientBackground(int width, int height, const QColor& color1, const QColor& color2)
{
    QPixmap pixmap(width, height);
    
    QPainter painter(&pixmap);
    QLinearGradient gradient(0, 0, 0, height);
    gradient.setColorAt(0, color1);
    gradient.setColorAt(1, color2);
    
    painter.fillRect(0, 0, width, height, gradient);
    
    return pixmap;
}

QPixmap ImageUtils::overlayImages(const QPixmap& background, const QPixmap& foreground, int x, int y)
{
    QPixmap result = background.copy();
    
    QPainter painter(&result);
    painter.drawPixmap(x, y, foreground);
    
    return result;
}

QString ImageUtils::extractDominantColor(const QPixmap& pixmap)
{
    QImage image = pixmap.toImage().scaled(50, 50, Qt::IgnoreAspectRatio, Qt::FastTransformation);
    
    int r = 0, g = 0, b = 0;
    int pixelCount = 0;
    
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            QRgb pixel = image.pixel(x, y);
            r += qRed(pixel);
            g += qGreen(pixel);
            b += qBlue(pixel);
            pixelCount++;
        }
    }
    
    r /= pixelCount;
    g /= pixelCount;
    b /= pixelCount;
    
    return QString("#%1%2%3")
        .arg(r, 2, 16, QChar('0'))
        .arg(g, 2, 16, QChar('0'))
        .arg(b, 2, 16, QChar('0'));
}

QImage ImageUtils::convertToGrayscale(const QImage& image)
{
    QImage result = image.convertToFormat(QImage::Format_Grayscale8);
    return result;
}
