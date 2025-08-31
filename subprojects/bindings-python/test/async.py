#!/usr/bin/env python3

import asyncio

import pms_utils  # type: ignore


async def get_async_metadata(
    ebuild: pms_utils.repo.Ebuild,
) -> pms_utils.ebuild.Metadata:
    return await asyncio.ensure_future(ebuild.async_metadata())


async def main():
    repo = pms_utils.repo.Repository("/var/db/repos/gentoo")
    for category in repo:
        for package in category:
            for ebuild in package:
                sync_metadata = ebuild.metadata
                async_metadata = await get_async_metadata(ebuild)
                assert sync_metadata == async_metadata


if __name__ == "__main__":
    asyncio.run(main())
