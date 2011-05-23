#download and patch options
SRCNAME=binutils
VERSION=2.17
DOWNLOAD_URL="http://ftp.gnu.org/gnu/binutils/$(SRCNAME)-$(VERSION).tar.bz2"
PATCHDIR=patch

#confiure and compile options
HOSTMACHINE=$(shell uname -m)
PREFIX=/usr
INFODIR=$(PREFIX)/share/info
LIBDIR=$(PREFIX)/lib
MANDIR=$(PREFIX)/share/man
CONFIGURE_OPTS=--prefix=$(PREFIX) --disable-nls --infodir=$(INFODIR) --libdir=$(LIBDIR) --mandir=$(MANDIR) --disable-werror --target=avr
PACKAGE_READY_FILE=pkg/usr/lib/

#package options
PKG_NAME=avr-binutils-tinyos
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

patch: unpack $(SRCNAME)-$(VERSION)/patched
$(SRCNAME)-$(VERSION)/patched:
	cd binutils-2.17&&patch -p0 -i ../$(PATCHDIR)/*.patch
	touch $(SRCNAME)-$(VERSION)/patched #just for the makefile

configure: patch $(SRCNAME)-$(VERSION)/build/Makefile
$(SRCNAME)-$(VERSION)/build/Makefile:
	install -d $(SRCNAME)-$(VERSION)/build
	cd $(SRCNAME)-$(VERSION)/build&&../configure $(CONFIGURE_OPTS)

compile: configure
	cd $(SRCNAME)-$(VERSION)/build&&make

package: compile
	install -d $(PKG_DIR)
	cd $(SRCNAME)-$(VERSION)/build&&make DESTDIR=$(PWD)/$(PKG_DIR) tooldir=/usr install
	#remove confliciting and/or unnecesary files
	rm -f $(PKG_DIR)/usr/lib/libiberty.a \
		$(PKG_DIR)/usr/share/info/dir \
		$(PKG_DIR)/usr/bin/addr2line \
		$(PKG_DIR)/usr/bin/ar \
		$(PKG_DIR)/usr/bin/as \
		$(PKG_DIR)/usr/bin/c++filt \
		$(PKG_DIR)/usr/bin/gprof \
		$(PKG_DIR)/usr/bin/ld \
		$(PKG_DIR)/usr/bin/nm \
		$(PKG_DIR)/usr/bin/objcopy \
		$(PKG_DIR)/usr/bin/objdump \
		$(PKG_DIR)/usr/bin/ranlib \
		$(PKG_DIR)/usr/bin/readelf \
		$(PKG_DIR)/usr/bin/size \
		$(PKG_DIR)/usr/bin/strings \
		$(PKG_DIR)/usr/bin/strip
	#rename info files to avr-*
	mv $(PKG_DIR)/usr/share/info/in.info $(PKG_DIR)/usr/share/info/avr-in.info||echo
	mv $(PKG_DIR)/usr/share/info/as.info $(PKG_DIR)/usr/share/info/avr-as.info||echo
	mv $(PKG_DIR)/usr/share/info/bfd.info $(PKG_DIR)/usr/share/info/avr-bfd.info||echo
	mv $(PKG_DIR)/usr/share/info/binutils.info $(PKG_DIR)/usr/share/info/avr-binutils.info||echo
	mv $(PKG_DIR)/usr/share/info/configure.info $(PKG_DIR)/usr/share/info/avr-configure.info||echo
	mv $(PKG_DIR)/usr/share/info/ld.info $(PKG_DIR)/usr/share/info/avr-ld.info||echo
	mv $(PKG_DIR)/usr/share/info/standards.info $(PKG_DIR)/usr/share/info/avr-standards.info||echo

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


