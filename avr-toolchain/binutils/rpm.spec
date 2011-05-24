Summary: GNU binutils for the AVR platform
Name: avr-binutils
Version: PKG_VERSION
Release: PKG_RELEASE
Packager: Janos Sallai, Vanderbilt University <janos.sallai@vanderbilt.edu>
URL: http://ftp.gnu.org/gnu/binutils/binutils-2.17.tar.bz2

License: GNU GPL

%description
The GNU Binutils are a collection of binary tools. The main tools are 
ld and as for the AVR platform. 

%install
rm -rf $RPM_BUILD_ROOT
cp -a PKG_DIRECTORY $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
/usr
