from pathlib import Path
from typing import TYPE_CHECKING, TypeVar

from collections import defaultdict

from tqdm import tqdm

import rangers.modbuilder
from rangers.modbuilder import ModBuilder
from rangers.dat import DAT

if TYPE_CHECKING:
    T = TypeVar('T')

    def tqdm(x: T) -> T:
        ...


def recursive_defaultdict() -> defaultdict:
    return defaultdict(recursive_defaultdict)


class Builder(rangers.modbuilder.ModBuilder):
    name = 'DenButtonsPack'

    path = Path(f'Mods/Den/{name}/')

    def build(self, src: Path, dst: Path) -> None:
        (dst / self.path).mkdir(parents=True, exist_ok=True)
        (dst / self.path / 'CFG').mkdir(parents=True, exist_ok=True)
        (dst / self.path / 'DATA').mkdir(parents=True, exist_ok=True)

        cachedata = recursive_defaultdict()

        for img_file in tqdm(list((src / 'buttons').glob('**/*.png'))):
            rel_gi_file = self.path / 'DATA' / img_file.relative_to(src).with_suffix('.gi')
            gi_file = dst / rel_gi_file
            self.png_to_gi(img_file, gi_file, fmt=2)

            cachedata['Bm']['Buttons']['18'][rel_gi_file.parts[-2].capitalize()][
                rel_gi_file.parts[-3].capitalize()
            ][rel_gi_file.stem] = str(rel_gi_file)

        cachedata_dat = DAT.from_json_value(cachedata)
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
                'SmallDescription': 'Добавляет графику различных кнопок, которые могут использоваться в других модах',
                'FullDescription': f'Добавляет графику различных кнопок, которые могут использоваться в других модах.\n'
                '{rangers.modbuilder.legal_rus}\n'
                '{rangers.modbuilder.stateless_rus}\n'
                '{rangers.modbuilder.contentless_rus}\n',
                'SmallDescriptionEng': 'TODO: English small description',
                'FullDescriptionEng': f'TODO: English full description.\n'
                '{rangers.modbuilder.legal_eng}\n'
                '{rangers.modbuilder.stateless_eng}\n'
                '{rangers.modbuilder.contentless_eng}\n',
            },
        )


if __name__ == '__main__':
    Builder()._build()
