APP_NAME = Basket

CONFIG += qt warn_on cascades10

include(config.pri)

LIBS += -lbb -lbbsystem -lbbnetwork -lbbdata -lbbmultimedia -lbbcascadespickers

QT += network core gui
