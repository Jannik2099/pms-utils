This project implements the Gentoo Package Manager Specification. It is often abbreviated as PMS.
The specification can be found at https://dev.gentoo.org/~ulm/pms/head/pms.html.
Before answering any requests, always fetch the specification and read it.

This project uses the meson build system. To configure it, run `meson setup -Dtest=true --buildtype=debugoptimized build` from the root of the project.
To build the project, run `meson compile -C build`.
If you are running in vscode, use the `runTests` tool to run tests, and the `testFailure` tool to get test failures. Otherwise, run `meson test -C build`.
To run a single test, always use `meson test -C build <test_name> --verbose`.

The build system stores logs in `build/meson-logs/`. Use these logs when analyzing build or configuration failures.
The build system provides detailed introspection files in `build/meson-info/`. Use these files to gain insights into the build configuration and environment, for example when querying build targets and tests.
When analyzing dependencies, read the `intro-dependencies.json` file.
When analyzing tests, read the `intro-tests.json` file.
