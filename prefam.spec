%global git_ref 6c382ec93d94847fc5dd132809638831a5c2b9d7
%global git_short_ref %(echo %{git_ref} | cut -b -7)

Name:           prefam
Version:        0^202606252341.%{git_short_ref}
Release:        %autorelease
Summary:        Preloaded file access monitor
License:        Apache-2.0
URL:            https://github.com/mkoncek/prefam

Source0:        https://github.com/mkoncek/prefam/archive/%{git_ref}.tar.gz#/%{name}-%{version}.tar.gz

BuildRequires:  gcc
BuildRequires:  make
BuildRequires:  asciidoctor

%description
%{summary}.

%prep
%autosetup -p1 -C

%build
%{make_build} compile manpage

%install
install -m 755 -p -D -t %{buildroot}%{_libdir} target/libprefam.so
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
