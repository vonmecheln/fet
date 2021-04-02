TEMPLATE = subdirs
SUBDIRS = src/src.pro src/src-cl.pro

unix {
translations.path = /usr/share/fet/translations
translations.files = translations/*.qm

examples.path = /usr/share/fet
examples.files = examples/

desktop.path = /usr/share/applications
desktop.files = fet.desktop

manual.path = /usr/share/man/man1
manual.files = man/*

licenses.path = /usr/share/doc/fet/licenses
licenses.files = licenses/*

documentation.path = /usr/share/doc/fet
documentation.files = doc/* AUTHORS COPYING ChangeLog README REFERENCES THANKS TODO TRANSLATORS

icon_svg.path = /usr/share/icons/hicolor/scalable/apps
icon_svg.files = icons/fet.svg

icon_png.path = /usr/share/pixmaps
icon_png.files = icons/fet.png

# The "licenses" entry needs to appear before the "documentation" entry, so that "make uninstall" can remove the "/usr/share/doc/fet" directory.
INSTALLS += translations examples desktop manual licenses documentation icon_svg icon_png
}
