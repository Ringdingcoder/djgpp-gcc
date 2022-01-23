FROM fedora:35
RUN dnf install -y mock rpmdevtools
RUN useradd -m build
USER build
