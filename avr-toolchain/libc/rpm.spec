Summary: C library for the AVR platform
Name: avr-libc
Version: PKG_VERSION
Release: PKG_RELEASE
Packager: Janos Sallai, Vanderbilt University
License: GNU GPL-compatible
Group: Development/Tools
URL: http://www.nongnu.org/avr-libc/3/

%description
C library for the AVR platform.

%install
rm -rf $RPM_BUILD_ROOT
cp -a PKG_DIRECTORY $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
/usr


