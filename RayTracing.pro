QT += widgets opengl openglwidgets #winextras

SOURCES += \
    DebugTime.cpp \
    GUI.cpp \
    Object3D.cpp \
    3DObject/3DGeometry.cpp \
    RayTracing.cpp \
    Timestamp.cpp \
    Transfo.cpp \
    fenetre.cpp \
    main.cpp \
    map3D.cpp

HEADERS += \
    DebugTime.h \
    GUI.h \
    Object3D.h \
    3DObject/3DGeometry.h \
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
