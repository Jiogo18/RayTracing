#include "Object3D.h"

const Image *OBJECT3D::getTexture(const std::string &file)
{
    if (loadedTextures.count(file)) {
        return &loadedTextures[file];
    }
    Image img;

    if (file.find("hologramme_") == 0) {
        std::string file_tag = file.substr(11); // après hologramme_
        std::size_t ok;
        int alpha = std::stoi(file_tag, &ok);
        if (ok != file_tag.length()) {
            std::cout << "Texture invalide " << file << " " << file_tag << std::endl;
            throw "Texture invalide";
        }
        img = Image(1, 1, Image::Format_RGBA64);

        img.fill(QColor(255 - alpha, 0, alpha, alpha / 10));

    } else if (img.load(":/ressourcepacks/default/textures/" + file + ".png")) {
        if (img.format() != Image::Format_RGBA64) {
            img = img.convertToFormat(Image::Format_RGBA64);
        }
    } else {
        std::cout << "[OBJECT3D::getTexture] can't load this texture: " << file << std::endl;
        /*if(img.load(":/ressourcepacks/default/textures/blocks/block_nul.png"))
            return img;*/
        if (missingTexture.isNull()) {
            missingTexture = Image(2, 2, Image::Format_RGBA64);
            missingTexture.fill(QColor(0, 0, 0, 100));
            missingTexture.setPixelColor(0, 0, QColor(255, 0, 255, 100));
            missingTexture.setPixelColor(1, 1, QColor(255, 0, 255, 100));
        }
        img = missingTexture; // enregistrer file pour ne pas rappeller plusieurs fois
    }

    loadedTextures.insert(std::pair<std::string, Image>(file, img));
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
