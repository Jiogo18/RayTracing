QT += widgets opengl openglwidgets #winextras

SOURCES += \
    3DObject/3DGeometry.cpp \
    3DObject/Object3D.cpp \
    3DObject/World3D.cpp \
    DebugTime.cpp \
    GUI.cpp \
    RayTracing.cpp \
    Timestamp.cpp \
    Transfo.cpp \
    fenetre.cpp \
    main.cpp \
    map3D.cpp

HEADERS += \
    3DObject/3DGeometry.h \
    3DObject/Object3D.h \
    3DObject/World3D.h \
    DebugTime.h \
    GUI.h \
    RayTracing.h \
    Timestamp.h \
    Transfo.h \
    fenetre.h \
    map3D.h

RC_ICONS = icone.ico

RESOURCES += \
    ressourpack.qrc

DISTFILES += \
    bugs.md \
    README.md
