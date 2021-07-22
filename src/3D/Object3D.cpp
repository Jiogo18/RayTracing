#include "Object3D.h"

const QImage *OBJECT3D::getTexture(const QString &file)
{
    if (loadedTextures.contains(file)) {
        return &loadedTextures[file];
    }
    QImage img;

    if(file.startsWith("hologramme_")) {
        QString file_tag = file.mid(11);// après hologramme_
        bool ok = true;
        int alpha = file_tag.toInt(&ok);
        if(!ok) {
            qDebug() << "Texture invalide" << file << file_tag;
            qFatal("Texture invalide : ");
        }
        img = QImage(1, 1, QImage::Format_RGBA64);

        img.fill(QColor(255 - alpha, 0, alpha, alpha / 10));

    }
    else if (img.load(":/ressourcepacks/default/textures/" + file + ".png")) {
        if (img.format() != QImage::Format_RGBA64) {
            img = img.convertToFormat(QImage::Format_RGBA64);
        }
    }
    else {
        qWarning() << "[OBJECT3D::getTexture] can't load this texture:" << file;
        /*if(img.load(":/ressourcepacks/default/textures/blocks/block_nul.png"))
            return img;*/
        if (missingTexture.isNull()) {
            missingTexture = QImage(2, 2, QImage::Format_RGBA64);
            missingTexture.fill(QColor(0, 0, 0, 100));
            missingTexture.setPixelColor(0, 0, QColor(255, 0, 255, 100));
            missingTexture.setPixelColor(1, 1, QColor(255, 0, 255, 100));
        }
        img = missingTexture; // enregistrer file pour ne pas rappeller plusieurs fois
    }

    loadedTextures.insert(file, img);
    return &loadedTextures[file];
}

/*****************************************************************************
  Object
 *****************************************************************************/

Object *Object::operator=(const Object &obj)
{
    setPos(obj);
    maxGeometry = obj.maxGeometry;
    return this;
}
