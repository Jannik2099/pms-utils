# Copyright 2024 Gentoo Authors
# Distributed under the terms of the GNU General Public License v2

EAPI=8

PYTHON_COMPAT=( python3_{12..13} pypy3 )

inherit meson python-r1

if [[ "${PV}" == "9999" ]]; then
	inherit git-r3
	EGIT_REPO_URI="https://github.com/Jannik2099/pms-utils.git"
else
	SRC_URI="https://github.com/Jannik2099/pms-utils/archive/refs/tags/${PV}.tar.gz -> ${P}.tar.gz"
	KEYWORDS="~amd64"
fi

DESCRIPTION="A helper library to implement the Gentoo Package Manager Specification"
HOMEPAGE="https://github.com/Jannik2099/pms-utils"

LICENSE="GPL-2+"
SLOT="0"

IUSE="+python"
REQUIRED_USE="python? ( ${PYTHON_REQUIRED_USE} )"

DEPEND="
python? (
	${PYTHON_DEPS}
	$(python_gen_cond_dep 'dev-python/typing-extensions[${PYTHON_USEDEP}]' pypy3)
)
>=dev-libs/boost-1.87
"
RDEPEND="${DEPEND}"
BDEPEND=""

bindings_python_configure() {
	unset emesonargs
	local TEST="false"
	if has test ${FEATURES}; then
		TEST="true"
	fi
	local emesonargs=(
		"-Dtest=${TEST}"
	)
	local EMESON_SOURCE="${S}/subprojects/bindings-python"
	python_foreach_impl meson_src_configure --pkg-config-path="${WORKDIR}/${P}-build/meson-uninstalled"
}

bindings_python_compile() {
	local EMESON_SOURCE="${S}/subprojects/bindings-python"
	python_foreach_impl meson_src_compile
}

bindings_python_install() {
	local EMESON_SOURCE="${S}/subprojects/bindings-python"
	python_foreach_impl meson_src_install
}

bindings_python_test() {
	local EMESON_SOURCE="${S}/subprojects/bindings-python"
	python_foreach_impl meson_src_test
}

if [[ ${PV} == 9999 ]]; then
src_unpack() {
	git-r3_src_unpack
	cd "${S}/subprojects/bindings-python" || die
	meson subprojects download || die
}
fi

src_configure() {
	local TEST="false"
	if has test ${FEATURES}; then
		TEST="true"
	fi
	local emesonargs=(
		'-Dbindings=[]'
		"-Dtest=${TEST}"
	)
	meson_src_configure
	use python && bindings_python_configure
}

src_compile() {
	meson_src_compile
	use python && LD_LIBRARY_PATH="${WORKDIR}/${P}-build/lib;${LD_LIBRARY_PATH}" bindings_python_compile
}

src_install() {
	meson_src_install
	use python && bindings_python_install
}

src_test() {
	meson_src_test
	use python && LD_LIBRARY_PATH="${WORKDIR}/${P}-build/lib;${LD_LIBRARY_PATH}" bindings_python_test
}
