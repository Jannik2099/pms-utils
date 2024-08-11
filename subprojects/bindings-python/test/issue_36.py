#!/usr/bin/env python3

import pms_utils

repo = pms_utils.repo.Repository("/var/db/repos/gentoo")
category = repo["www-client"]

arr = [x for x in category]
name_set = {str(x.name) for x in arr}
assert len(name_set) > 1
