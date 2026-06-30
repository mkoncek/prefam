%global git_ref f081bd3fdcebdb2f396ca4545f598cc47f7135cd
%global git_short_ref %(echo %{git_ref} | cut -b -7)

Name:           prefam
Version:        0^202606261236.%{git_short_ref}
Release:        %autorelease
Summary:        Preloaded file access monitor
License:        Apache-2.0
URL:            https://github.com/mkoncek/prefam

Source0:        https://github.com/mkoncek/prefam/archive/%{git_ref}.tar.gz#/%{name}-%{version}.tar.gz

BuildRequires:  gcc
BuildRequires:  make
BuildRequires:  asciidoctor

%description
Prefam is a library that overrides system calls open*, exec*, posix_spawn* and records
which file paths have been accessed by these system calls.

%prep
%autosetup -p1 -C

%build
%{make_build} test-compile manpage

%install
install -m 755 -p -D -t %{buildroot}%{_libdir} target/lib/libprefam.so
install -m 644 -p -D -t %{buildroot}%{_mandir} target/manpages/*

%check
%{make_build} test

%files
%license LICENSE
%doc README.adoc
%{_libdir}/libprefam.so
%{_mandir}/*

%changelog
%autochangelog
