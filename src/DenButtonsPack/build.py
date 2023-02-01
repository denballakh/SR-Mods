from pathlib import Path
from typing import Any, TypeVar

from collections import defaultdict

from tqdm import tqdm

import rangers.modbuilder
from rangers.modbuilder import ModBuilder
from rangers.dat import DAT


def recursive_defaultdict() -> defaultdict[Any, Any]:
    return defaultdict(recursive_defaultdict)


class Builder(rangers.modbuilder.ModBuilder):
    name = 'DenButtonsPack'

    path = Path(f'Mods/Den/{name}/')

    def build(self, src: Path, dst: Path) -> None:
        tmp = src / '.tmp'

        cachedata = recursive_defaultdict()

        for img_file in tqdm(list((src / 'buttons').glob('**/*.png'))):
            rel_gi_file = self.path / 'DATA' / img_file.relative_to(src).with_suffix('.gi')
            tmp_gi_file = tmp / img_file.relative_to(src).with_suffix('.gi')
            gi_file = dst / rel_gi_file
            self.png_to_gi(img_file, tmp_gi_file, fmt=2)
            self.copy_file(tmp_gi_file, gi_file)

            cachedata['Bm']['Buttons']['18'][rel_gi_file.parts[-2].capitalize()][
                rel_gi_file.parts[-3].capitalize()
            ][rel_gi_file.stem] = str(rel_gi_file)

        cachedata_dat = DAT.from_dict(cachedata)
        cachedata_dat.to_dat(dst / self.path / 'CFG' / 'CacheData.dat', fmt='HDCache', sign=True)

        self.write_modinfo(
            dst / self.path / 'ModuleInfo.txt',
            {
                'Name': self.name,
                'Section': 'Den',
                'Author': 'denball',
                'Conflict': 'ButtonsPack',
                'Dependence': '',
                'Priority': 0,
                'Languages': 'Rus,Eng',
                'SmallDescription': f'Добавляет графику различных кнопок, которые могут использоваться в других модах',
                'FullDescription': f'Добавляет графику различных кнопок, которые могут использоваться в других модах.\n'
                f'{rangers.modbuilder.legal_rus}\n'
                f'{rangers.modbuilder.stateless_rus}\n'
                f'{rangers.modbuilder.contentless_rus}\n',
                #
                'SmallDescriptionEng': f'Adds button graphics that can be used in other mods',
                'FullDescriptionEng': f'Adds button graphics that can be used in other mods.\n'
                f'{rangers.modbuilder.legal_eng}\n'
                f'{rangers.modbuilder.stateless_eng}\n'
                f'{rangers.modbuilder.contentless_eng}\n',
            },
        )


if __name__ == '__main__':
    Builder()._build()
