#!/usr/bin/env python3

import pms_utils

useflags = set()
repo = pms_utils.repo.Repository("/var/db/repos/gentoo")
for category in repo:
    for package in category:
        for ebuild in package:
            for iuse_elem in ebuild.metadata.IUSE:
                useflags.add(iuse_elem.useflag)

len(useflags)
[str(x) for x in useflags]
