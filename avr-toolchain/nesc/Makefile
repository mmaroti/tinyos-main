#download and patch options
SRCNAME=nesc
VERSION=1.3.1
DOWNLOAD_URL="http://sourceforge.net/projects/nescc/files/nescc/v$(VERSION)/$(SRCNAME)-$(VERSION).tar.gz"
PATCHDIR=patch

#confiure and compile options
PREFIX=/usr
PREFIX=/usr
HOST=$(shell uname -m)
CONFIGURE_OPTS=--host=$(HOST) --prefix=$(PREFIX)

#package options
PKG_NAME=$(SRCNAME)
###START OF PACKAGE INDEPENDENT PART###
PKG_VERSION=$(VERSION)
PKG_RELEASE=$(shell date +%Y%m%d)
PKG_HOST=$(shell echo $(HOST)|sed 's/.*64/amd64/'|sed 's/.*86/i386/')
PKG_DIR=$(PKG_NAME)_$(PKG_VERSION)-$(PKG_RELEASE)_$(PKG_HOST)
ABS_PKG_DIR=$(shell echo $(PWD)/$(PKG_DIR)|sed 's/\//\\\//g')

#check package builders: these variables are the absolute path to the packager, or empty if the packager is not present
RPMBUILD=$(shell whereis rpmbuild|sed 's/.*: //'|sed 's/ .*//'|sed 's/.*:.*//g')
DPKG=$(shell whereis dpkg-deb|sed 's/.*: //'|sed 's/ .*//'|sed 's/.*:.*//g')

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

help:
	@echo "default target: all"
	@echo "all: creates rpm (if rpmbuild is available) and deb (if dpkg-deb is available) packages"
	@echo "get: downloads the source from $(DOWNLOAD_URL)"
	@echo "unpack: uncopresses the source"
	@echo "patch: applying all patches from directory $(PATCHDIR)"
	@echo "configure: configure package with options: $(CONFIGURE_OPTS)"
	@echo "compile: compiles package"
	@echo "pacakge: install the package to $(PKG_DIR)"
	@echo "deb: creates deb package from $(PKG_DIR) if dpkg-deb is available"
	@echo "rpm: creates rpm package from $(PKG_DIR) if rpmbuild is available"

dummyrpm:
	@echo "Can't found rpmbuild. Rpm package will not be created"

dummydeb:
	@echo "Can't found dpkg. Deb package will not be created"

rpm: $(RPMTARGET)
deb: $(DEBTARGET)

get: $(SRCNAME)-$(VERSION).tar.gz
$(SRCNAME)-$(VERSION).tar.gz:
	wget $(DOWNLOAD_URL)	

unpack: get $(SRCNAME)-$(VERSION)/configure
$(SRCNAME)-$(VERSION)/configure:
	tar -zxf $(SRCNAME)-$(VERSION).tar.gz

patch: $(PATCHTARGET)
dummypatch: unpack
realpatch: unpack make_patchdone
make_patchdone:
	cd $(SRCNAME)-$(VERSION)&&cat ../$(PATCHDIR)/*.patch|patch -p0 && cd .. && touch make_patchdone #just for the makefile

configure: patch $(SRCNAME)-$(VERSION)/Makefile
$(SRCNAME)-$(VERSION)/Makefile:
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
		fakeroot $(DPKG) --build $(PKG_DIR)&& \
		rm -rf $(PKG_DIR)/DEBIAN

realrpm: package make_pkg_rpmdone
make_pkg_rpmdone:
	sed 's/PKG_VERSION/$(PKG_VERSION)/g' rpm.spec | \
		sed 's/PKG_RELEASE/$(PKG_RELEASE)/g'> realrpm.spec && \
		$(RPMBUILD) --target=$(HOST) --buildroot=$(ABS_PKG_DIR) -bb realrpm.spec&&rm realrpm.spec&&touch make_pkg_rpmdone&&rm -f make_pkg_packagedone

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
	cd $(SRCNAME)-$(VERSION)&&make DESTDIR=$(PWD)/$(PKG_DIR) install
	touch make_pkg_packagedone #just for the makefile
