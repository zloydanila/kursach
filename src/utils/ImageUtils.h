#ifndef IMAGEUTILS_H
#define IMAGEUTILS_H

#include <QPixmap>
#include <QImage>
#include <QString>

class ImageUtils
{
public:
    static QPixmap loadImage(const QString& path, int width = 0, int height = 0);
    static QPixmap createCircularImage(const QPixmap& source);
    static QPixmap createRoundedImage(const QPixmap& source, int radius);
    static QPixmap applyBlur(const QPixmap& source, int radius);
    static QPixmap createThumbnail(const QPixmap& source, int size);
    
    static bool saveImage(const QPixmap& pixmap, const QString& path, const QString& format = "PNG");
    static QPixmap resizeImage(const QPixmap& source, int width, int height, Qt::AspectRatioMode mode = Qt::KeepAspectRatio);
    
    static QPixmap createGradientBackground(int width, int height, const QColor& color1, const QColor& color2);
    static QPixmap overlayImages(const QPixmap& background, const QPixmap& foreground, int x, int y);
    
    static QString extractDominantColor(const QPixmap& pixmap);
    static QImage convertToGrayscale(const QImage& image);
};

#endif
