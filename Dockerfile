FROM fedora:35
RUN dnf install -y rpmdevtools git
RUN useradd -m build
USER build
RUN git config --global --add safe.directory /bld
