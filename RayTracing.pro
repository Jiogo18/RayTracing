QT += widgets opengl openglwidgets #winextras

SOURCES += \
    src/3D/Entity.cpp \
    src/3D/Geometry3D.cpp \
    src/3D/Object3D.cpp \
    src/3D/Point3D.cpp \
    src/3D/Pos3D.cpp \
    src/3D/Solid.cpp \
    src/DebugTime.cpp \
    src/GUI/GUI.cpp \
    src/GUI/fenetre.cpp \
    src/RayTracing/ColorLight.cpp \
    src/RayTracing/Ray.cpp \
    src/RayTracing/RayTracing.cpp \
    src/RayTracing/Ressources.cpp \
    src/RayTracing/Transfo.cpp \
    src/RayTracing/Worker.cpp \
    src/World/Timestamp.cpp \
    src/World/World3D.cpp \
    src/World/map3D.cpp \
    src/main.cpp

HEADERS += \
    src/3D/Entity.h \
    src/3D/Geometry3D.h \
    src/3D/Object3D.h \
    src/3D/Point3D.h \
    src/3D/Pos3D.h \
    src/3D/Solid.h \
    src/3D/maths3D.h \
    src/DebugTime.h \
    src/GUI/GUI.h \
    src/GUI/fenetre.h \
    src/RayTracing/ColorLight.h \
    src/RayTracing/Ray.h \
    src/RayTracing/RayTracing.h \
    src/RayTracing/Ressources.h \
    src/RayTracing/Transfo.h \
    src/RayTracing/Worker.h \
    src/World/Timestamp.h \
    src/World/World3D.h \
    src/World/map3D.h


RC_ICONS = icone.ico

RESOURCES += \
    ressourpack.qrc

DISTFILES += \
    bugs.md \
    README.md \
    equations.txt
