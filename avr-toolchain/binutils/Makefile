#download and patch options
SRCNAME=binutils
VERSION=2.17
DOWNLOAD_URL="http://ftp.gnu.org/gnu/$(SRCNAME)/$(SRCNAME)-$(VERSION).tar.bz2"
PATCHDIR=patch

#confiure and compile options
PREFIX=/usr
CONFIGURE_OPTS=--prefix=$(PREFIX) --disable-nls --infodir=$(PREFIX)/share/info --libdir=$(PREFIX)/lib --mandir=$(PREFIX)/share/man --disable-werror --target=avr

#package options
PKG_NAME=avr-binutils-tinyos
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

#check if there's any patch
PATCHNUM=$(shell ls -1 $(PATCHDIR)/*.patch|wc -l)
ifeq ($(PATCHNUM),0)
 PATCHTARGET=dummypatch
else
 PATCHTARGET=realpatch
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

patch: $(PATCHTARGET)
dummypatch: unpack
realpatch: unpack make_patchdone
make_patchdone:
	cd $(SRCNAME)-$(VERSION)&&patch -p0 -i ../$(PATCHDIR)/*.patch && cd .. && touch make_patchdone #just for the makefile

configure: patch $(SRCNAME)-$(VERSION)/Makefile
$(SRCNAME)-$(VERSION)/Makefile:
	install -d $(SRCNAME)-$(VERSION)
	cd $(SRCNAME)-$(VERSION)&&./configure $(CONFIGURE_OPTS)

compile: configure make_compiledone
make_compiledone:
	cd $(SRCNAME)-$(VERSION)&&make&&cd ..&&touch make_compiledone #just for the makefile

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
	cd $(SRCNAME)-$(VERSION)&&make DESTDIR=$(ABS_PKG_DIR) tooldir=/usr install
	#remove confliciting and/or unnecesary files
	rm -f $(PKG_DIR)/usr/lib/libiberty.a \
		$(PKG_DIR)/usr/share/info/dir \
		$(PKG_DIR)/usr/bin/addr2line* \
		$(PKG_DIR)/usr/bin/ar* \
		$(PKG_DIR)/usr/bin/as* \
		$(PKG_DIR)/usr/bin/c++filt* \
		$(PKG_DIR)/usr/bin/gprof* \
		$(PKG_DIR)/usr/bin/ld* \
		$(PKG_DIR)/usr/bin/nm* \
		$(PKG_DIR)/usr/bin/objcopy* \
		$(PKG_DIR)/usr/bin/objdump* \
		$(PKG_DIR)/usr/bin/ranlib* \
		$(PKG_DIR)/usr/bin/readelf* \
		$(PKG_DIR)/usr/bin/size* \
		$(PKG_DIR)/usr/bin/strings* \
		$(PKG_DIR)/usr/bin/strip*
	#rename info files to avr-*
	-mv $(PKG_DIR)/usr/share/info/in.info $(PKG_DIR)/usr/share/info/avr-in.info
	-mv $(PKG_DIR)/usr/share/info/as.info $(PKG_DIR)/usr/share/info/avr-as.info
	-mv $(PKG_DIR)/usr/share/info/bfd.info $(PKG_DIR)/usr/share/info/avr-bfd.info
	-mv $(PKG_DIR)/usr/share/info/binutils.info $(PKG_DIR)/usr/share/info/avr-binutils.info
	-mv $(PKG_DIR)/usr/share/info/configure.info $(PKG_DIR)/usr/share/info/avr-configure.info
	-mv $(PKG_DIR)/usr/share/info/ld.info $(PKG_DIR)/usr/share/info/avr-ld.info
	-mv $(PKG_DIR)/usr/share/info/standards.info $(PKG_DIR)/usr/share/info/avr-standards.info
	touch make_pkg_packagedone #just for the makefile