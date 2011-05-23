#download and patch options
SRCNAME=avr-libc
VERSION=1.6.7
DOWNLOAD_URL="http://download.savannah.gnu.org/releases/$(SRCNAME)/$(SRCNAME)-$(VERSION).tar.bz2"
PATCHDIR=patch

#confiure and compile options
HOSTMACHINE=$(shell uname -m)
PREFIX=/usr
CONFIGURE_OPTS=--prefix=$(PREFIX) --host=avr

#package options
PKG_NAME=avr-libc-tinyos
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
	cd $(SRCNAME)-$(VERSION)/build&&make DESTDIR=$(PWD)/$(PKG_DIR) install

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


