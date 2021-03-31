QT += widgets opengl openglwidgets #winextras

SOURCES += \
    src/3D/Geometry3D.cpp \
    src/3D/Object3D.cpp \
    src/3D/Point3D.cpp \
    src/3D/Pos3D.cpp \
    src/DebugTime.cpp \
    src/GUI/GUI.cpp \
    src/GUI/fenetre.cpp \
    src/RayTracing/RayTracing.cpp \
    src/RayTracing/Transfo.cpp \
    src/World/Timestamp.cpp \
    src/World/World3D.cpp \
    src/World/map3D.cpp \
    src/main.cpp

HEADERS += \
    src/3D/Geometry3D.h \
    src/3D/Object3D.h \
    src/3D/Point3D.h \
    src/3D/Pos3D.h \
    src/3D/maths3D.h \
    src/DebugTime.h \
    src/GUI/GUI.h \
    src/GUI/fenetre.h \
    src/RayTracing/RayTracing.h \
    src/RayTracing/Transfo.h \
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
