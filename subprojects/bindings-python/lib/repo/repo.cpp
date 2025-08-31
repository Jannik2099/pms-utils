#include "repo.hpp"

#include "../common.hpp"
#include "pms-utils/atom/atom.hpp"
#include "pms-utils/ebuild/ebuild.hpp"
#include "pms-utils/misc/meta.hpp"
#include "pms-utils/repo/repo.hpp"

#include <boost/asio/awaitable.hpp>
#include <filesystem>
#include <nanobind/nanobind.h>
#include <string_view>

namespace nb = nanobind;

using namespace pms_utils::repo;

namespace pms_utils::bindings::python::repo {

void _register(nb::module_ &_module) {
    nb::module_ repo = _module.def_submodule("repo");

    bind_awaitable<meta::crt<boost::asio::awaitable<ebuild::Metadata>>>(repo);
    repo.def("parse_metadata", parse_metadata);
    repo.def("async_parse_metadata", async_parse_metadata,
             nb::sig{"def async_parse_metadata(path: str | os.PathLike) -> "
                     "collections.abc.Awaitable[pms_utils.ebuild.Metadata]"});

    auto py_Ebuild =
        create_bindings<Ebuild>(repo)
            .def_prop_ro("metadata", &Ebuild::metadata)
            .def(
                "async_metadata",
                [] [[clang::coro_wrapper]] (const Ebuild &ebuild) { return ebuild.async_metadata(); },
                nb::sig{"def async_metadata(self) -> collections.abc.Awaitable[pms_utils.ebuild.Metadata]"});

    auto py_Package =
        create_bindings<Package>(repo)
            .def("__getitem__",
                 [](const Package &package, const atom::Version &version) { return package[version]; })
            .def("__getitem__",
                 [](const Package &package, std::string_view version) { return package[version]; });

    auto py_Category =
        create_bindings<Category>(repo)
            .def("__getitem__",
                 [](const Category &category, const atom::Name &name) { return category[name]; })
            .def("__getitem__",
                 [](const Category &category, std::string_view package) { return category[package]; });

    auto py_Repository =
        create_bindings<Repository>(repo)
            .def(nb::init<std::filesystem::path>())
            .def("__getitem__", [](const Repository &repository,
                                   const atom::Category &category) { return repository[category]; })
            .def("__getitem__", [](const Repository &repository, std::string_view category) {
                return repository[category];
            });
}

} // namespace pms_utils::bindings::python::repo
