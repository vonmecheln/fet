lessThan(QT_MAJOR_VERSION, 5) {
	error(Qt version $$QT_VERSION is not supported. The minimum supported Qt version is 5.15.0.)
}

equals(QT_MAJOR_VERSION, 5) {
	lessThan(QT_MINOR_VERSION, 15){
		error(Qt version $$QT_VERSION is not supported. The minimum supported Qt version is 5.15.0.)
	}
}

TEMPLATE = subdirs
SUBDIRS = src/src.pro src/src-cl.pro

unix {
	translations.path = /usr/share/fet/translations
	translations.files = translations/*.qm

	examples.path = /usr/share/fet
	examples.files = examples/

	mime.path = /usr/share/mime/packages
	mime.files = fet.xml

	desktop.path = /usr/share/applications
	desktop.files = fet.desktop

	manual.path = /usr/share/man/man1
	manual.files = man/*

	licenses.path = /usr/share/doc/fet/licenses
	licenses.files = licenses/*

	documentation.path = /usr/share/doc/fet
	documentation.files = doc/* AUTHORS COPYING ChangeLog README REFERENCES THANKS TODO TRANSLATORS

	icon_png.path = /usr/share/icons/hicolor/128x128/apps
	icon_png.files = icons/fet.png

	icon_png_256x256.path = /usr/share/icons/hicolor/256x256/apps
	icon_png_256x256.files = icons/256x256/fet.png

	icon_png_512x512.path = /usr/share/icons/hicolor/512x512/apps
	icon_png_512x512.files = icons/512x512/fet.png

	icon_svg.path = /usr/share/icons/hicolor/scalable/apps
	icon_svg.files = icons/fet.svg

	# The "licenses" entry needs to appear before the "documentation" entry, so that "make uninstall" can remove the "/usr/share/doc/fet" directory.
	INSTALLS += translations examples mime desktop manual licenses documentation icon_png icon_png_256x256 icon_png_512x512 icon_svg
}
