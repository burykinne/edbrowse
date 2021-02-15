Name: edbrowse
Version: 3.7.7
Release: alt1

Summary: ed-alike webbrowser written in C
License: GPL and MPL
Group: Networking/WWW

Url: http://edbrowse.org/
Source0: https://github.com/CMB/edbrowse/archive/v%version.tar.gz#/%name-%version.tar.gz
Patch0: %name-%version-alt-pcre-and-tidy-warnings.patch

BuildRequires: cmake
BuildRequires: gcc-c++
BuildRequires: libcurl-devel
BuildRequires: libreadline-devel
BuildRequires: libpcre-devel
BuildRequires: libtidy-devel
BuildRequires: libduktape-devel

%description
edbrowse is a reimplementation of /bin/ed, with some basic
differences (it uses Perl regular expressions) with the ability to
visit webpages and ftp sites. edbrowse performs basic transformations
on the html source to produce a readable representation. edbrowse
supports Forms, Frames, Netscape-style cookies, HTTPS
connections and JavaScript.

%prep
%setup
%patch0 -p1

%build
cmake .
%make_build

%install
install -Dm755 src/%name %buildroot/%_bindir/%name

%files
%doc README COPYING CHANGES
%_bindir/%name

%changelog
* Fri Feb 05 2021 Nikolay Burykin <bne@altlinux.org> 3.7.7-alt1
- Initial build for ALT

* Mon Oct 15 2018 Wei-Lun Chao <bluebat@member.fsf.org> 3.7.4
- Rebuild for Fedora
