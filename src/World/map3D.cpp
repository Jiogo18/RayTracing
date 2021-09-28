#include "map3D.h"

map3D::map3D()
{
    world = new World();
    client = new Entity(Pos3D::fromDegree(-0.5L, 1.5L, 1.5L, -30, -20), ENTITY::Type::ghost);
    world->addEntity(client);
    //client->moveTo(Pos3D::fromDegree(1.8L, 2, 1.9L, -140, -30));//belle vue
    //client->moveTo(Pos3D::fromDegree(1.5L, 1.1L, 0.9L, -140, -30));//vue green_glass

    //client->moveTo(Pos3D::fromDegree(0.5L, 1.5, 2.5L, -90, -30));//vue miroir verre blanc
    //client->moveTo(Pos3D::fromDegree(1.2L, 0.5, 2.5L, -180, -30));//vue miroir verre vert
    //client->moveTo(Pos3D::fromDegree(0.5L, 4, 1.2L, -90, -10));//vue miroir glowstone
    client->moveTo(Pos3D::fromDegree(0.5, 3, 0.9L, -70, -10)); //belle vue mirroir
    //client->moveTo(Pos3D::fromDegree(0.5, 1.5, 2.5, 0, -90));//verre au dessus

    //client->moveTo(Pos3D::fromDegree(-16, 16, 16, 0, 0)); // vue map Pollux

    world->addSolid(new Cube(Pos3D(0, 0, 0, 0, 0), SOLID::Material::oak_log));
    world->addSolid(new Cube(Pos3D(0, 1, 0, 0, 0), SOLID::Material::birch_log));
    world->addSolid(new Cube(Pos3D(0, 0, 1, 0, 0), SOLID::Material::stone));
    world->addSolid(new Cube(Pos3D(0, 1, 1, 0, 0), SOLID::Material::glass));
    world->addSolid(new Cube(Pos3D(1, 0, 0, 0, 0), SOLID::Material::green_glass));
    world->addSolid(new HalfCube(Pos3D(0, 2, -0.25, 0, 0), SOLID::Material::glowstone));
    world->addSolid(new Cube(Pos3D(0, 0, 2, 0, 0), SOLID::Material::mirror));

    //mur de miroir entouré de stone
    fillCube(Point3D(2, 0, 0), Point3D(2, 2, 3), SOLID::Material::stone);
    fillCube(Point3D(2, 1, 0), Point3D(2, 1, 3), SOLID::Material::mirror);

    world->addSolid(new Cube(Pos3D(0, 3, 0, 0, 0), SOLID::Material::mirror));
    world->addSolid(new Cube(Pos3D(1, 3, 0, 0, 0), SOLID::Material::watter));

    world->addSolid(new HalfCube(Pos3D(-1, -1, -1, 0, 0), SOLID::Material::glowstone));
}

map3D::~map3D()
{
    if (world != nullptr) delete world;
    //client est delete par le world donc c'est bon
}

int map3D::fillCube(const Point3D &posMin, const Point3D &posMax, SOLID::Material blockMaterial)
{
    HRect3D rect(posMin, posMax);

    int blockPlaced = 0;
    for (int x = rect.getPointMin().x(); x <= rect.getPointMax().x(); x++) {
        for (int y = rect.getPointMin().y(); y <= rect.getPointMax().y(); y++) {
            for (int z = rect.getPointMin().z(); z <= rect.getPointMax().z(); z++) {
                Cube *c = new Cube(Pos3D(x, y, z, 0, 0), blockMaterial);
                if (world->addSolid(c))
                    blockPlaced++;
                else
                    delete c;
            }
        }
    }
    return blockPlaced;
}

bool map3D::load(std::string mapFileName)
{
    // remove everything
    const std::vector<Chunk *> chunks = world->getChunks();
    for (const Chunk *c : chunks) {
        const std::vector<Solid *> solids = *c->getSolids();
        for (const Solid *s : solids) {
            world->removeSolid(s->getPoint());
        }
    }

    // load the file
    QFile mapFile(mapFileName);
    if (!mapFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        std::cout << "map3D::load can't open \"" << mapFileName << "\" " << mapFile.errorString() << std::endl;
        return false;
    }

    //QRegularExpression regex("X:(\\d+) Y:(\\d+) Z:(\\d+) Value:(\\d+)");
    const std::regex regex("(\\d+):(\\d+):(\\d+):(\\d+)");
    int blockAdded = 0;
#if true // chargement avec le fichier
    while (!mapFile.atEnd()) {
        QByteArray line = mapFile.readLine();

        std::regex_match(line, regex);
        std::smatch pieces_match;

        if (!std::regex_match(line, pieces_match, regex)) {
            std::cout << "map3D::load line doesn't match at " << mapFile.pos() << std::endl;
            return false;
        }

        doubli x = std::stod(pieces_match[1].str());
        doubli y = std::stod(pieces_match[2].str());
        doubli z = std::stod(pieces_match[3].str());
        int alpha = std::stoi(pieces_match[4].str()); // le min c'est 85
        //if(alpha < 120) continue; // LIMITE

        std::vector<SOLID::Variation> variations;
        if (alpha & 1) variations.push_back(SOLID::Variation::BIN1);
        if (alpha & 2) variations.push_back(SOLID::Variation::BIN2);
        if (alpha & 4) variations.push_back(SOLID::Variation::BIN3);
        if (alpha & 8) variations.push_back(SOLID::Variation::BIN4);
        if (alpha & 16) variations.push_back(SOLID::Variation::BIN5);
        if (alpha & 32) variations.push_back(SOLID::Variation::BIN6);
        if (alpha & 64) variations.push_back(SOLID::Variation::BIN7);
        if (alpha & 128) variations.push_back(SOLID::Variation::BIN8);

        if (world->addSolid(new Block(Pos3D(x, y, z, 0, 0), Size3D(1, 1, 1), SOLID::Material::hologramme, variations))) {
            blockAdded++;
        }
    }
#else // chargement avec un cube de 256*256*256
    for (int x = 0; x < 256; x++) {
        for (int y = 0; y < 256; y++) {
            for (int z = 0; z < 256; z++) {

                int alpha = (x + y + z) / 3;

                std::vector<SOLID::Variation> variations;
                if (alpha & 1) variations.push_back(SOLID::Variation::BIN1);
                if (alpha & 2) variations.push_back(SOLID::Variation::BIN2);
                if (alpha & 4) variations.push_back(SOLID::Variation::BIN3);
                if (alpha & 8) variations.push_back(SOLID::Variation::BIN4);
                if (alpha & 16) variations.push_back(SOLID::Variation::BIN5);
                if (alpha & 32) variations.push_back(SOLID::Variation::BIN6);
                if (alpha & 64) variations.push_back(SOLID::Variation::BIN7);
                if (alpha & 128) variations.push_back(SOLID::Variation::BIN8);

                if (world->addSolid(new Block(Pos3D(x, y, z, 0, 0), Size3D(1, 1, 1), SOLID::Material::hologramme, variations))) {
                    blockAdded++;
                }
            }
        }
        std::cout << "Blocs placés : " << blockAdded << " / 16777216" << std::endl;
    }
#endif

    std::cout << "map3D::load Le monde contient " << blockAdded << " blocs" << std::endl;

    mapFile.close();
    return true;
}
