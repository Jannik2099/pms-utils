FROM docker.io/gentoo/stage3:latest
# all one single step because Docker is a medieval piece of crap that still can't squash (at least the action can't)
RUN <<EOF
    cd /etc/portage
    echo "FEATURES=\"-network-sandbox -pid-sandbox -ipc-sandbox -mount-sandbox parallel-fetch parallel-install -merge-sync binpkg-request-signature getbinpkg\"" >> make.conf
    mkdir -p profile/use.stable.mask
    echo "-python_targets_python3_12" >> profile/use.stable.mask/pms-utils
    echo "-python_targets_pypy3" >> profile/use.stable.mask/pms-utils
    echo "-python_single_target_python3_12" >> profile/use.stable.mask/pms-utils
    echo "-python_single_target_pypy3" >> profile/use.stable.mask/pms-utils
    mkdir -p package.mask
    # to prefer -bin
    echo "dev-python/pypy3_*-exe" >> package.mask/pypy3

    mkdir -p package.env
    mkdir -p env

    echo "dev-python/pybind11 pb11env" > package.env/pb11
    echo "PYTHON_TARGETS=\"python3_11 python3_12 pypy3\"" >> env/pb11env
    emerge-webrsync -q
    getuto
    emerge --autounmask --autounmask-write --autounmask-continue -q -j8 dev-vcs/git dev-util/pkgdev sys-devel/clang sys-devel/lld dev-libs/boost dev-python/pybind11 dev-build/meson dev-python/pip dev-util/cppcheck
    env-update
    . /etc/profile
    rm -rf /var/cache/distfiles
    rm -rf /var/cache/binpkgs
EOF
