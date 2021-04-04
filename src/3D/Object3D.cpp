#include "Object3D.h"

const QImage *OBJECT3D::getTexture(const QString &file)
{
    if (loadedTextures.contains(file)) {
        return &loadedTextures[file];
    }
    QImage img;
    if (img.load(":/ressourcepacks/default/textures/" + file + ".png")) {
        if (img.format() != QImage::Format_RGBA64) {
            img = img.convertToFormat(QImage::Format_RGBA64);
        }
        loadedTextures.insert(file, img);
        return &loadedTextures[file];
    }
    qWarning() << "[OBJECT3D::getTexture] can't load this texture:" << file;
    /*if(img.load(":/ressourcepacks/default/textures/blocks/block_nul.png"))
        return img;*/
    if (missingTexture.isNull()) {
        missingTexture = QImage(2, 2, QImage::Format_RGBA64);
        missingTexture.fill(QColor(0, 0, 0, 100));
        missingTexture.setPixelColor(0, 0, QColor(255, 0, 255, 100));
        missingTexture.setPixelColor(1, 1, QColor(255, 0, 255, 100));
    }
    loadedTextures.insert(file, missingTexture); // ne pas rappeller plusieurs fois

    return &missingTexture;
}
