DEPENDPATH += . \
    ../Shared/irrKlang
INCLUDEPATH += ../Shared/irrKlang/include
win32:LIBS += ../Shared/irrKlang/lib/libirrKlang.a
HEADERS += $$PWD/include/irrKlang.h \
    $$PWD/include/ik_vec3d.h \
    $$PWD/include/ik_SAudioStreamFormat.h \
    $$PWD/include/ik_IVirtualRefCounted.h \
    $$PWD/include/ik_ISoundStopEventReceiver.h \
    $$PWD/include/ik_ISoundSource.h \
    $$PWD/include/ik_ISoundEngine.h \
    $$PWD/include/ik_ISoundEffectControl.h \
    $$PWD/include/ik_ISoundDeviceList.h \
    $$PWD/include/ik_ISound.h \
    $$PWD/include/ik_irrKlangTypes.h \
    $$PWD/include/ik_IRefCounted.h \
    $$PWD/include/ik_IFileReader.h \
    $$PWD/include/ik_IFileFactory.h \
    $$PWD/include/ik_IAudioStreamLoader.h \
    $$PWD/include/ik_IAudioStream.h \
    $$PWD/include/ik_IAudioRecorder.h \
    $$PWD/include/ik_EStreamModes.h \
    $$PWD/include/ik_ESoundOutputDrivers.h \
    $$PWD/include/ik_ESoundEngineOptions.h

DISTFILES += \
    $$PWD/lib/libirrKlang.a
