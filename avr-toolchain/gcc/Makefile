#download and patch options
SRCNAME=gcc
VERSION=4.1.2
#TODO this is a hungarian mirror...
DOWNLOAD_URL="http://robotlab.itk.ppke.hu/$(SRCNAME)/releases/$(SRCNAME)-$(VERSION)/$(SRCNAME)-$(VERSION).tar.bz2"
PATCHDIR=patch

#confiure and compile options
HOSTMACHINE=$(shell uname -m)
PREFIX=/usr
INFODIR=$(PREFIX)/share/info
LIBDIR=$(PREFIX)/lib
LIBEXECDIR=$(LIBDIR)
MANDIR=$(PREFIX)/share/man
CONFIGURE_OPTS=--prefix=$(PREFIX) --disable-libssp --disable-nls --enable-languages=c --infodir=$(INFODIR) --libdir=$(LIBDIR) --libexecdir=$(LIBEXECDIR) --mandir=$(MANDIR) --target=avr

#package options
PKG_NAME=avr-gcc-tinyos
PKG_VERSION=$(VERSION)-$(shell date +%Y%m%d)
PKG_HOST=$(shell uname -m|sed 's/.*64/amd64/'|sed 's/.*86/i386/')
PKG_DIR=$(PKG_NAME)_$(PKG_VERSION)_$(PKG_HOST)


all: rpm deb

get: $(SRCNAME)-$(VERSION).tar.bz2
$(SRCNAME)-$(VERSION).tar.bz2:
	wget $(DOWNLOAD_URL)	

unpack: get $(SRCNAME)-$(VERSION)/configure
$(SRCNAME)-$(VERSION)/configure:
	tar -jxf $(SRCNAME)-$(VERSION).tar.bz2

#TODO: if there's no patch, it crashes
patch: unpack $(SRCNAME)-$(VERSION)/patched
$(SRCNAME)-$(VERSION)/patched:
	cd $(SRCNAME)-$(VERSION)&&patch -p0 -i ../$(PATCHDIR)/*.patch
	touch $(SRCNAME)-$(VERSION)/patched #just for the makefile

configure: patch $(SRCNAME)-$(VERSION)/build/Makefile
$(SRCNAME)-$(VERSION)/build/Makefile:
	install -d $(SRCNAME)-$(VERSION)/build
	cd $(SRCNAME)-$(VERSION)/build&&../configure $(CONFIGURE_OPTS)

compile: configure
	cd $(SRCNAME)-$(VERSION)/build&&make

#this should be the only package specific target
package: compile
	install -d $(PKG_DIR)
	cd $(SRCNAME)-$(VERSION)/build&&make -j1 DESTDIR=$(PWD)/$(PKG_DIR) install
	#remove confliciting and/or unnecesary files
	rm -rf $(PKG_DIR)/usr/lib/libiberty.a \
		$(PKG_DIR)/usr/share/man/man7 \
		$(PKG_DIR)/usr/share/info \


deb: package
	install -d $(PKG_DIR)/DEBIAN
	cp debcontrol $(PKG_DIR)/DEBIAN/control
	sed 's/PKG_VERSION/$(PKG_VERSION)/' $(PKG_DIR)/DEBIAN/control > /tmp/control
	sed 's/PKG_ARCHITECTURE/$(PKG_HOST)/' /tmp/control > $(PKG_DIR)/DEBIAN/control
	dpkg --build $(PKG_DIR)
	rm -rf $(PKG_DIR)/DEBIAN
rpm: package
	echo "TODO"
clean:
	rm -rf $(SRCNAME)-$(VERSION)
	rm -rf pkg $(PKG_NAME)_$(PKG_VERSION)_$(PKG_HOST)
	rm -f *.rpm *.deb
	rm -f $(SRCNAME)-$(VERSION).tar.bz2


