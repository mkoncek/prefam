%global git_ref 3d5adb3c367049743dbad8c2cf251318171664f3
%global git_short_ref %(echo %{git_ref} | cut -b -7)

Name:           prefam
Version:        0^202607091026.%{git_short_ref}
Release:        %autorelease
Summary:        Preloaded file access monitor
License:        Apache-2.0
URL:            https://github.com/mkoncek/prefam

Source0:        https://github.com/mkoncek/prefam/archive/%{git_ref}.tar.gz#/%{name}-%{version}.tar.gz

BuildRequires:  gcc
BuildRequires:  make
BuildRequires:  asciidoctor

%description
Prefam is a library that interposes C library functions of families open, fopen,
opendir, readlink, exec, posix_spawn and records which file paths have been
accessed by these functions.

It works by preloading a shared library via LD_PRELOAD, which means it only
intercepts dynamically linked C library calls and not raw system calls made via
syscall or inline assembly.

%prep
%autosetup -p1 -C

%build
%{make_build} test-compile manpages

%install
install -m 755 -p -D -t %{buildroot}%{_bindir} src/prefam
install -m 755 -p -D -t %{buildroot}%{_libdir} target/lib/libprefam.so.1
install -m 644 -p -D -t %{buildroot}%{_mandir} target/manpages/*

%check
%{make_build} test

%files
%license LICENSE NOTICE
%doc README.adoc
%{_bindir}/prefam
%{_libdir}/libprefam.so.1
%{_mandir}/*

%changelog
%autochangelog
