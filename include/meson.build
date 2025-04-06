inc_lib = include_directories('.')

headers = files(
    'pms-utils/atom/atom.hpp',
    'pms-utils/atom/atom_parser.hpp',
    'pms-utils/depend/depend.hpp',
    'pms-utils/depend/depend_parser.hpp',
    'pms-utils/ebuild/ebuild.hpp',
    'pms-utils/ebuild/ebuild_parser.hpp',
    'pms-utils/misc/macro-begin.hpp',
    'pms-utils/misc/macro-end.hpp',
    'pms-utils/misc/meta.hpp',
    'pms-utils/misc/try_parse.hpp',
    'pms-utils/misc/x3_utils.hpp',
    'pms-utils/profile/profile.hpp',
    'pms-utils/profile/profile_parser.hpp',
    'pms-utils/repo/repo.hpp',
    'pms-utils/repo/repo_parser.hpp',
)

install_headers(
    headers,
    preserve_path: true,
)

pch_headers = []

foreach header : headers
    if not (fs.stem(header) in ['macro-begin', 'macro-end'])
        pch_headers += header
    endif

endforeach

pch_str = ''

foreach header : pch_headers
    header_str = fs.relative_to(header, '.')
    pch_str += f'#include <@header_str@>\n'
endforeach

pch_header = configure_file(
    configuration: {
        'PCH_STR': pch_str,
    },
    input: 'pms-utils.hpp.in',
    output: 'pms-utils.hpp',
)

install_headers(pch_header)
