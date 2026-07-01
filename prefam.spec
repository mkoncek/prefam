Name:           prefam
Version:        1.0.0
Release:        %autorelease
Summary:        Preloaded file access monitor
License:        Apache-2.0
URL:            https://github.com/mkoncek/prefam

Source0:        https://github.com/mkoncek/prefam/archive/%{git_ref}.tar.gz#/%{name}-%{version}.tar.gz

BuildRequires:  gcc
BuildRequires:  make
BuildRequires:  asciidoctor

%description
Prefam is a library that overrides system calls open*, exec*, posix_spawn* and
records which file paths have been accessed by these system calls.

%prep
%autosetup -p1 -C

%build
%{make_build} test-compile manpage

%install
install -m 755 -p -D -t %{buildroot}%{_libdir} target/lib/libprefam.so.1
install -m 644 -p -D -t %{buildroot}%{_mandir} target/manpages/*

%check
%{make_build} test

%files
%license LICENSE
%doc README.adoc
%{_libdir}/libprefam.so.1
%{_mandir}/*

%changelog
%autochangelog
