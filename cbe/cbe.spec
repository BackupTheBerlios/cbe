# cbe.spec: used for building rpms
# -*- rpm-spec -*-

%define name cbe
%define version 0.0.1
%define release 1

Name: %{name}
Version: %{version}
Release: %{release}
Summary: A Change Blindness Experiment/Environment 
Copyright: GPL
Group: Applications/Multimedia
Source: %{name}-%{version}.tar.gz
Buildroot: %{_tmppath}/%{name}-%{version}-%{release}-root
URL: http://cbe.berlios.de/
Packager: Andreas Bauer <baueran@users.berlios.de>

%description
CBE is an environment for setting up Change Blindness experiments.

%prep
%setup

%build
./configure
make

%install
make install prefix=$RPM_BUILD_ROOT%{_prefix} sysconfdir=$RPM_BUILD_ROOT%{_sysconfdir}

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-, root, root)
%doc AUTHORS COPYING ChangeLog INSTALL NEWS README TODO
%{_bindir}/cbe

%changelog
* Thu Jan 25 2001 Andreas Bauer <baueran@users.berlios.de>

- Changed my own e-mail address to BerliOS

- Added INSTALL file to document root

- Changed Group to Applications/Multimedia

* Sat Jan 20 2001 Andreas Bauer <baueran@users.berlios.de>

- Initial creation of the package
