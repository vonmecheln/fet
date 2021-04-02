TEMPLATE = subdirs
SUBDIRS += interface
CONFIG += release \
          warn_on \
          qt \
          thread 
MOC_DIR = ../tmp
OBJECTS_DIR = ../tmp
UI_DIR = ../tmp
