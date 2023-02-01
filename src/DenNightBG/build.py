from pathlib import Path
from typing import Any, TypeVar

from collections import defaultdict
import shutil

from tqdm import tqdm

import rangers.modbuilder
from rangers.dat import DAT


def recursive_defaultdict() -> defaultdict[Any, Any]:
    return defaultdict(recursive_defaultdict)


class Builder(rangers.modbuilder.ModBuilder):
    name = 'DenNightBG'

    path = Path(f'Mods/Den/{name}/')

    def build(self, src: Path, dst: Path) -> None:
        tmp = src / '.tmp'
        pics = src / 'bg'
        pkg_name = f'{self.name}.pkg'
        resource_path = Path() / 'DATA' / 'NightBG'

        for png_file in tqdm(list(pics.glob('*.png'))):
            rel_gi_file = resource_path / png_file.relative_to(pics).with_suffix('.gi')
            gi_file = tmp / 'pkg' / rel_gi_file
            self.png_to_gi(png_file, gi_file, fmt=0, opt=32)

        self.pack_folder(tmp / 'pkg', tmp / pkg_name, 9)
        self.copy_file(tmp / pkg_name, dst / self.path / pkg_name)

        self.write_install(dst / self.path / 'INSTALL.TXT', [self.path / pkg_name])

        cachedata = recursive_defaultdict()
        main = recursive_defaultdict()

        cachedata['Bm']['City']['DynamicBG_gov'][f'{self.name}_gaal_0'] = str(
            resource_path / 'gaal_gov_0.gi'
        )

        cachedata['Bm']['City']['DynamicBG'][f'{self.name}_maloc_0'] = str(
            resource_path / 'maloc_0.gi'
        )
        cachedata['Bm']['City']['DynamicBG'][f'{self.name}_peleng_0'] = str(
            resource_path / 'peleng_0.gi'
        )
        cachedata['Bm']['City']['DynamicBG'][f'{self.name}_peleng_1'] = str(
            resource_path / 'peleng_1.gi'
        )
        cachedata['Bm']['City']['DynamicBG'][f'{self.name}_people_0'] = str(
            resource_path / 'people_0.gi'
        )
        cachedata['Bm']['City']['DynamicBG'][f'{self.name}_fei_0'] = str(resource_path / 'fei_0.gi')
        cachedata['Bm']['City']['DynamicBG'][f'{self.name}_fei_1'] = str(resource_path / 'fei_1.gi')
        cachedata['Bm']['City']['DynamicBG'][f'{self.name}_gaal_0'] = str(
            resource_path / 'gaal_0.gi'
        )
        cachedata['Bm']['City']['DynamicBG'][f'{self.name}_gaal_1'] = str(
            resource_path / 'gaal_1.gi'
        )

        main['DynamicBG'][f'{self.name}_planet_maloc_0'] = {
            'requirement': f'Den{self.name}',
            'type': 'planet',
            'race': 'Maloc',
            'paths': {
                'city': f'Bm.City.DynamicBG.{self.name}_maloc_0',
                'gov': f'Bm.City.DynamicBG_gov.Maloc_vanilla',
            },
        }

        main['DynamicBG'][f'{self.name}_planet_peleng_0'] = {
            'requirement': f'Den{self.name}',
            'type': 'planet',
            'race': 'Peleng',
            'paths': {
                'city': f'Bm.City.DynamicBG.{self.name}_peleng_0',
                'gov': f'Bm.City.DynamicBG_gov.Peleng_vanilla',
            },
        }

        main['DynamicBG'][f'{self.name}_planet_peleng_1'] = {
            'requirement': f'Den{self.name}',
            'type': 'planet_pirate',
            'race': 'Peleng',
            'paths': {
                'city': f'Bm.City.DynamicBG.{self.name}_peleng_1',
                'gov': f'Bm.City.DynamicBG_gov.Peleng_vanilla',
            },
        }

        main['DynamicBG'][f'{self.name}_planet_people_0'] = {
            'requirement': f'Den{self.name}',
            'type': 'planet_pirate',
            'race': 'People',
            'paths': {
                'city': f'Bm.City.DynamicBG.{self.name}_people_0',
                'gov': f'Bm.City.DynamicBG_gov.People_vanilla',
            },
        }

        main['DynamicBG'][f'{self.name}_planet_fei_0'] = {
            'requirement': f'Den{self.name}',
            'type': 'planet_pirate',
            'race': 'Fei',
            'paths': {
                'city': f'Bm.City.DynamicBG.{self.name}_fei_0',
                'gov': f'Bm.City.DynamicBG_gov.Fei_vanilla',
            },
        }

        main['DynamicBG'][f'{self.name}_planet_fei_1'] = {
            'requirement': f'Den{self.name}',
            'type': 'planet',
            'race': 'Fei',
            'paths': {
                'city': f'Bm.City.DynamicBG.{self.name}_fei_1',
                'gov': f'Bm.City.DynamicBG_gov.Fei_vanilla',
            },
        }

        main['DynamicBG'][f'{self.name}_planet_gaal_0'] = {
            'requirement': f'Den{self.name}',
            'type': 'planet_pirate',
            'race': 'Gaal',
            'paths': {
                'city': f'Bm.City.DynamicBG.{self.name}_gaal_0',
                'gov': f'Bm.City.DynamicBG_gov.{self.name}_gaal_0',
            },
        }

        main['DynamicBG'][f'{self.name}_planet_gaal_1'] = {
            'requirement': f'Den{self.name}',
            'type': 'planet',
            'race': 'Gaal',
            'paths': {
                'city': f'Bm.City.DynamicBG.{self.name}_gaal_1',
                'gov': f'Bm.City.DynamicBG_gov.{self.name}_gaal_0',
            },
        }

        main_dat = DAT.from_dict(main)
        main_dat.to_dat(dst / self.path / 'CFG' / 'Main.dat', fmt='HDMain', sign=True)
        cachedata_dat = DAT.from_dict(cachedata)
        cachedata_dat.to_dat(dst / self.path / 'CFG' / 'CacheData.dat', fmt='HDCache', sign=True)

        self.write_modinfo(
            dst / self.path / 'ModuleInfo.txt',
            {
                'Name': self.name,
                'Section': 'Den',
                'Author': 'denball',
                'Conflict': '',
                'Dependence': 'DenDynamicBG',
                'Priority': 0,
                'Languages': 'Rus,Eng',
                'SmallDescription': 'Добавляет 8 ночных фонов планет',
                'FullDescription': 'Добавляет 8 ночных фонов планет.\n'
                f'{rangers.modbuilder.stateless_rus}\n'
                f'{rangers.modbuilder.legal_rus}\n',
                #
                'SmallDescriptionEng': 'Add 8 night planet backgrounds',
                'FullDescriptionEng': 'Add 8 night planet backgrounds.\n'
                f'{rangers.modbuilder.stateless_eng}\n'
                f'{rangers.modbuilder.legal_eng}\n',
            },
        )


if __name__ == '__main__':
    Builder()._build()
