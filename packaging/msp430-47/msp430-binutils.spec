Summary: Experimental (4.7) MSP430 binutils
Name: msp430-binutils-47
Version: %{version}
Release: %{release}
License: GNU GPL
Packager: Razvan Musaloiu-E. <razvan@musaloiu.com>
Group: Development/Tools

%description

%install
rsync -a %{prefix} %{buildroot}

%files
/opt/msp430-47
