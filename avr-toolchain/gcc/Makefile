#download and patch options
SRCNAME=gcc
VERSION=4.1.2
DOWNLOAD_URL="ftp://ftp.gnu.org/gnu/$(SRCNAME)/$(SRCNAME)-$(VERSION)/$(SRCNAME)-$(VERSION).tar.bz2"
PATCHDIR=patch

#confiure and compile options
PREFIX=/usr
#TODO some of these needed for ubuntu, but I don't know which
AS=/usr/bin/avr-as
LD=/usr/bin/avr-ld
CONFIGURE_OPTS=$(CONFIGURE_UBI) --prefix=$(PREFIX) --disable-libssp --disable-nls --enable-languages=c --infodir=$(PREFIX)/share/info --libdir=$(PREFIX)/lib --libexecdir=$(PREFIX)/lib --mandir=$(PREFIX)/share/man --target=avr --with-ld=$(LD) --with-as=$(AS)

#package options
PKG_NAME=avr-gcc-tinyos
###START OF PACKAGE INDEPENDENT PART###
PKG_VERSION=$(VERSION)
PKG_RELEASE=$(shell date +%Y%m%d)
PKG_HOST=$(shell uname -m|sed 's/.*64/amd64/'|sed 's/.*86/i386/')
PKG_DIR=$(PKG_NAME)_$(PKG_VERSION)-$(PKG_RELEASE)_$(PKG_HOST)
ABS_PKG_DIR=$(shell echo $(PWD)/$(PKG_DIR)|sed 's/\//\\\//g')


#check package builders: these variables are the absolute path to the packager, or empty if the packager is not present
RPMBUILD=$(shell whereis rpmbuild|sed 's/.*: //'|sed 's/ .*//'|sed 's/.*:.*//g')
DPKG=$(shell whereis dpkg|sed 's/.*: //'|sed 's/ .*//'|sed 's/.*:.*//g')

ifeq ($(RPMBUILD),)
 RPMTARGET=dummyrpm
else
 RPMTARGET=realrpm
endif

ifeq ($(DPKG),)
 DEBTARGET=dummydeb
else
 DEBTARGET=realdeb
endif

all: $(DEBTARGET) $(RPMTARGET)

dummyrpm:
	@echo "Can't found rpmbuild. Rpm package will not be created"

dummydeb:
	@echo "Can't found dpkg. Deb package will not be created"

rpm: $(RPMTARGET)
deb: $(DEBTARGET)

get: $(SRCNAME)-$(VERSION).tar.bz2
$(SRCNAME)-$(VERSION).tar.bz2:
	wget $(DOWNLOAD_URL)	

unpack: get $(SRCNAME)-$(VERSION)/configure
$(SRCNAME)-$(VERSION)/configure:
	tar -jxf $(SRCNAME)-$(VERSION).tar.bz2

#TODO: if there's no patch, it crashes
patch: unpack make_patchdone
make_patchdone:
	cd $(SRCNAME)-$(VERSION)&&patch -p0 -i ../$(PATCHDIR)/*.patch && cd .. && touch make_patchdone #just for the makefile

configure: patch $(SRCNAME)-$(VERSION)/build/Makefile
$(SRCNAME)-$(VERSION)/build/Makefile:
	install -d $(SRCNAME)-$(VERSION)/build
	cd $(SRCNAME)-$(VERSION)/build&&../configure $(CONFIGURE_OPTS)

compile: configure make_compiledone
make_compiledone:
	cd $(SRCNAME)-$(VERSION)/build&&make&&cd ../..&&touch make_compiledone #just for the makefile

realdeb: package $(PKG_DIR).deb
$(PKG_DIR).deb:
	install -d $(PKG_DIR)/DEBIAN
	cp debcontrol $(PKG_DIR)/DEBIAN/control
	sed 's/PKG_VERSION/$(PKG_VERSION)-$(PKG_RELEASE)/g' debcontrol | \
		sed 's/PKG_ARCHITECTURE/$(PKG_HOST)/g' > $(PKG_DIR)/DEBIAN/control&& \
		$(DPKG) --build $(PKG_DIR)&& \
		rm -rf $(PKG_DIR)/DEBIAN

realrpm: package make_pkg_rpmdone
make_pkg_rpmdone:
	sed 's/PKG_VERSION/$(PKG_VERSION)/g' rpm.spec | \
		sed 's/PKG_RELEASE/$(PKG_RELEASE)/g'> realrpm.spec && \
		$(RPMBUILD) --buildroot=$(ABS_PKG_DIR) -bb realrpm.spec&&rm realrpm.spec&&touch make_pkg_rpmdone
cleanpackage:
	rm -rf $(PKG_DIR)
	rm -f *.rpm *.deb
	rm -f realrpm.spec
	rm -f make_pkg_*

cleanbuild: cleanpackage
	rm -rf $(SRCNAME)-$(VERSION)
	rm -f make_*

clean: cleanbuild
	rm -f $(SRCNAME)-$(VERSION).tar.bz2
	
#this should be the only package specific target
package: compile make_pkg_packagedone
make_pkg_packagedone:
	install -d $(PKG_DIR)
###END OF PACKAGE INDEPENDENT PART###
	cd $(SRCNAME)-$(VERSION)/build&&make -j1 DESTDIR=$(PWD)/$(PKG_DIR) install
	#remove confliciting and/or unnecesary files
	rm -rf $(PKG_DIR)/usr/lib/libiberty.a \
		$(PKG_DIR)/usr/share/man/man7 \
		$(PKG_DIR)/usr/share/info	
	touch make_pkg_packagedone #just for the makefile


