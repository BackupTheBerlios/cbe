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
Group: Applications
Source: %{name}-%{version}.tar.gz
Buildroot: %{_tmppath}/%{name}-%{version}-%{release}-root
URL: http://cbe.berlios.de/
Packager: Andreas Bauer <baueran@users.sourceforge.net>

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
%doc AUTHORS COPYING ChangeLog NEWS README TODO
%{_bindir}/cbe

%changelog
* Sat Jan 20 2001 Andreas Bauer <baueran@users.sourceforge.net>

- Initial creation of the package
