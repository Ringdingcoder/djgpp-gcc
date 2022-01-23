FROM fedora:35
RUN dnf install -y rpmdevtools
RUN useradd -m build
USER build
