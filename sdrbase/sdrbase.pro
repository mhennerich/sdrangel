#--------------------------------------------------------
#
# Pro file for Android and Windows builds with Qt Creator
#
#--------------------------------------------------------

QT += core gui multimedia opengl
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = lib
TARGET = sdrbase
INCLUDEPATH += $$PWD

DEFINES += USE_KISSFFT=1
win32 {
    DEFINES += __WINDOWS__=1
    DEFINES += DSD_USE_SERIALDV=1
}
DEFINES += USE_SSE2=1
QMAKE_CXXFLAGS += -msse2
DEFINES += USE_SSE4_1=1
QMAKE_CXXFLAGS += -msse4.1

CONFIG(Release):build_subdir = release
CONFIG(Debug):build_subdir = debug

CONFIG(ANDROID):INCLUDEPATH += /opt/softs/boost_1_60_0

CONFIG(MINGW32):INCLUDEPATH += "D:\boost_1_58_0"
CONFIG(MINGW64):INCLUDEPATH += "D:\boost_1_58_0"

CONFIG(MINGW32):INCLUDEPATH += "D:\softs\serialDV"
CONFIG(MINGW64):INCLUDEPATH += "D:\softs\serialDV"

CONFIG(macx):INCLUDEPATH += "../../../boost_1_64_0"

win32 {
    HEADERS += \
        dsp/dvserialengine.h \
        dsp/dvserialworker.h
    SOURCES += \
        dsp/dvserialengine.cpp \
        dsp/dvserialworker.cpp
}

SOURCES += mainwindow.cpp\
        audio/audiodeviceinfo.cpp\
        audio/audiofifo.cpp\
        audio/audiooutput.cpp\
        audio/audioinput.cpp\
        device/devicesourceapi.cpp\
        device/devicesinkapi.cpp\
        dsp/afsquelch.cpp\
        dsp/agc.cpp\
        dsp/downchannelizer.cpp\
        dsp/upchannelizer.cpp\
        dsp/channelmarker.cpp\
        dsp/ctcssdetector.cpp\
        dsp/cwkeyer.cpp\
        dsp/dspcommands.cpp\
        dsp/dspengine.cpp\
        dsp/dspdevicesourceengine.cpp\
        dsp/dspdevicesinkengine.cpp\
        dsp/fftengine.cpp\
        dsp/kissengine.cpp\
        dsp/fftfilt.cxx\
        dsp/fftwindow.cpp\
        dsp/filterrc.cpp\
        dsp/filtermbe.cpp\
        dsp/filerecord.cpp\
        dsp/interpolator.cpp\
        dsp/hbfiltertraits.cpp\
        dsp/lowpass.cpp\
        dsp/nco.cpp\
        dsp/ncof.cpp\
        dsp/pidcontroller.cpp\
        dsp/phaselock.cpp\
        dsp/recursivefilters.cpp\
        dsp/samplesinkfifo.cpp\
        dsp/samplesourcefifo.cpp\
        dsp/samplesinkfifodoublebuffered.cpp\
        dsp/basebandsamplesink.cpp\
        dsp/basebandsamplesource.cpp\
        dsp/nullsink.cpp\
        dsp/spectrumscopecombovis.cpp\
        dsp/spectrumscopengcombovis.cpp\
        dsp/scopevis.cpp\
        dsp/scopevisng.cpp\
        dsp/spectrumvis.cpp\
        dsp/threadedbasebandsamplesink.cpp\
        dsp/threadedbasebandsamplesource.cpp\
        dsp/wfir.cpp\
        gui/aboutdialog.cpp\
        gui/addpresetdialog.cpp\
        gui/basicchannelsettingswidget.cpp\
        gui/basicchannelsettingsdialog.cpp\
        gui/buttonswitch.cpp\
        gui/channelwindow.cpp\
        gui/clickablelabel.cpp\
        gui/colormapper.cpp\
        gui/cwkeyergui.cpp\
        gui/glscope.cpp\
        gui/glscopegui.cpp\
        gui/glscopeng.cpp\
        gui/glscopenggui.cpp\
        gui/glshadersimple.cpp\
        gui/glshadertextured.cpp\
        gui/glspectrum.cpp\
        gui/glspectrumgui.cpp\
        gui/indicator.cpp\
        gui/levelmeter.cpp\
        gui/pluginsdialog.cpp\
        gui/audiodialog.cpp\
        gui/presetitem.cpp\
        gui/rollupwidget.cpp\
        gui/samplingdevicecontrol.cpp\
        gui/mypositiondialog.cpp\
        gui/scale.cpp\
        gui/scaleengine.cpp\
        gui/valuedial.cpp\
        gui/valuedialz.cpp\
        dsp/devicesamplesource.cpp\
        dsp/devicesamplesink.cpp\
        plugin/pluginapi.cpp\
        plugin/plugininterface.cpp\
        plugin/pluginmanager.cpp\
        settings/preferences.cpp\
        settings/preset.cpp\
        settings/mainsettings.cpp\
        util/CRC64.cpp\
        util/db.cpp\
        util/message.cpp\
        util/messagequeue.cpp\
        util/prettyprint.cpp\
        util/syncmessenger.cpp\
        util/samplesourceserializer.cpp\
        util/simpleserializer.cpp

HEADERS  += mainwindow.h\
        audio/audiodeviceinfo.h\
        audio/audiofifo.h\
        audio/audiooutput.h\
        audio/audioinput.h\
        device/devicesourceapi.h\
        device/devicesinkapi.h\
        dsp/afsquelch.h\
        dsp/downchannelizer.h\
        dsp/upchannelizer.h\
        dsp/channelmarker.h\
        dsp/cwkeyer.h\
        dsp/complex.h\
        dsp/decimators.h\
        dsp/interpolators.h\
        dsp/dspcommands.h\
        dsp/dspengine.h\
        dsp/dspdevicesourceengine.h\
        dsp/dspdevicesinkengine.h\
        dsp/dsptypes.h\
        dsp/fftengine.h\
        dsp/fftfilt.h\
        dsp/fftwengine.h\
        dsp/fftwindow.h\
        dsp/filterrc.h\
        dsp/filtermbe.h\
        dsp/filerecord.h\
        dsp/gfft.h\
        dsp/hbfiltertraits.h\
        dsp/interpolator.h\
        dsp/inthalfbandfilter.h\
        dsp/inthalfbandfilterdb.h\
        dsp/inthalfbandfiltereo1.h\
        dsp/inthalfbandfiltereo1i.h\
        dsp/inthalfbandfilterst.h\
        dsp/inthalfbandfiltersti.h\
        dsp/kissfft.h\
        dsp/kissengine.h\
        dsp/lowpass.h\
        dsp/misc.h\
        dsp/movingaverage.h\
        dsp/nco.h\
        dsp/ncof.h\
        dsp/phasediscri.h\
        dsp/phaselock.h\
        dsp/pidcontroller.h\
        dsp/recursivefilters.h\
        dsp/samplesinkfifo.h\
        dsp/samplesourcefifo.h\
        dsp/samplesinkfifodoublebuffered.h\
        dsp/samplesinkfifodecimator.h\
        dsp/basebandsamplesink.h\
        dsp/basebandsamplesource.h\
        dsp/nullsink.h\
        dsp/spectrumscopecombovis.h\
        dsp/spectrumscopengcombovis.h\        
        dsp/scopevis.h\
        dsp/scopevisng.h\
        dsp/spectrumvis.h\
        dsp/threadedbasebandsamplesink.h\
        dsp/threadedbasebandsamplesource.h\
        dsp/wfir.h\
        gui/aboutdialog.h\
        gui/addpresetdialog.h\
        gui/audiodialog.h\
        gui/basicchannelsettingswidget.h\
        gui/basicchannelsettingsdialog.h\
        gui/buttonswitch.h\
        gui/channelwindow.h\
        gui/clickablelabel.h\
        gui/colormapper.h\
        gui/cwkeyergui.h\
        gui/glscope.h\
        gui/glscopegui.h\
        gui/glscopeng.h\
        gui/glscopenggui.h\
        gui/glshadersimple.h\
        gui/glshadertextured.h\
        gui/glspectrum.h\
        gui/glspectrumgui.h\
        gui/indicator.h\
        gui/levelmeter.h\
        gui/physicalunit.h\
        gui/pluginsdialog.h\
        gui/presetitem.h\
        gui/rollupwidget.h\
        gui/samplingdevicecontrol.h\
        gui/mypositiondialog.h\
        gui/scale.h\
        gui/scaleengine.h\
        gui/valuedial.h\
        gui/valuedialz.h\
        dsp/devicesamplesource.h\
        dsp/devicesamplesink.h\
        plugin/pluginapi.h\
        plugin/plugininstanceui.h\
        plugin/plugininterface.h\
        plugin/pluginmanager.h\
        settings/preferences.h\
        settings/preset.h\
        settings/mainsettings.h\
        util/CRC64.h\
        util/db.h\
        util/export.h\
        util/message.h\
        util/messagequeue.h\
        util/prettyprint.h\
        util/syncmessenger.h\
        util/samplesourceserializer.h\
        util/simpleserializer.h

FORMS    += mainwindow.ui\
        gui/scopewindow.ui\
        gui/addpresetdialog.ui\
        gui/basicchannelsettingswidget.ui\
        gui/basicchannelsettingsdialog.ui\
        gui/cwkeyergui.ui\
        gui/audiodialog.ui\
        gui/glscopegui.ui\
        gui/glscopenggui.ui\
        gui/aboutdialog.ui\
        gui/pluginsdialog.ui\
        gui/samplingdevicecontrol.ui\
        gui/myposdialog.ui\
        gui/glspectrumgui.ui\
        mainwindow.ui

RESOURCES = resources/res.qrc

!macx:LIBS += -L../serialdv/$${build_subdir} -lserialdv

CONFIG(ANDROID):CONFIG += mobility
CONFIG(ANDROID):MOBILITY =
