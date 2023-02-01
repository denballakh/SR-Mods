from pathlib import Path
from typing import Any, TypeVar

from collections import defaultdict
import shutil
import random

from tqdm import tqdm

import rangers.modbuilder
from rangers.dat import DAT


def recursive_defaultdict() -> defaultdict[Any, Any]:
    return defaultdict(recursive_defaultdict)


class Builder(rangers.modbuilder.ModBuilder):
    name = 'DenBG'

    path = Path(f'Mods/Den/{name}/')

    def build(self, src: Path, dst: Path) -> None:
        tmp = src / '.tmp'
        pics = src / 'bg'
        pkg_name = f'{self.name}.pkg'

        cachedata = recursive_defaultdict()
        main = recursive_defaultdict()

        self.rename_bgs()
        self.delete_empty()

        cnt = 0
        for png_file_planet in tqdm(list(pics.glob('planets/**/*.png'))):
            race = png_file_planet.parts[-2]

            png_file_gov = pics / 'gov' / png_file_planet.relative_to(pics / 'planets')
            if not png_file_gov.exists():
                png_file_gov = random.choice(list(png_file_gov.parent.glob('*.png')))
                assert png_file_gov.exists()

            rel_gi_file_planet = (
                Path() / 'DATA' / 'BG' / png_file_planet.relative_to(pics).with_suffix('.gi')
            )
            rel_gi_file_gov = (
                Path() / 'DATA' / 'BG' / png_file_gov.relative_to(pics).with_suffix('.gi')
            )

            gi_file_planet = tmp / 'pkg' / rel_gi_file_planet
            gi_file_gov = tmp / 'pkg' / rel_gi_file_gov

            gi_file_planet.parent.mkdir(parents=True, exist_ok=True)
            gi_file_gov.parent.mkdir(parents=True, exist_ok=True)

            cachedata['Bm']['City']['DynamicBG'][f'{self.name}_{cnt:03}'] = str(rel_gi_file_planet)
            cachedata['Bm']['City']['DynamicBG_gov'][f'{self.name}_{cnt:03}'] = str(rel_gi_file_gov)
            main['DynamicBG'][f'{self.name}_planet_{cnt:03}'] = {
                'requirement': f'Den{self.name}',
                'type': 'planet' if 'pirate' not in race else 'planet_pirate',
                'race': race.replace('_pirate', '').capitalize(),
                'paths': {
                    'city': f'Bm.City.DynamicBG.{self.name}_{cnt:03}',
                    'gov': f'Bm.City.DynamicBG_gov.{self.name}_{cnt:03}',
                },
            }

            self.png_to_gi(png_file_planet, gi_file_planet, fmt=0, opt=32)
            self.png_to_gi(png_file_gov, gi_file_gov, fmt=0, opt=32)

            cnt += 1

        cnt = 0
        for png_file_ruin in tqdm(list(pics.glob('stations/**/*.png'))):
            ruin_type = png_file_ruin.parts[-2]

            rel_gi_file_ruin = (
                Path() / 'DATA' / 'BG' / png_file_ruin.relative_to(pics).with_suffix('.gi')
            )

            gi_file_ruin = tmp / 'pkg' / rel_gi_file_ruin

            gi_file_ruin.parent.mkdir(parents=True, exist_ok=True)

            cachedata['Bm']['City']['DynamicBG_ruin'][f'{self.name}_{cnt:03}'] = str(
                rel_gi_file_ruin
            )
            main['DynamicBG'][f'{self.name}_ruin_{cnt:03}'] = {
                'requirement': f'Den{self.name}',
                'type': 'ruin',
                'ruin': ruin_type.upper(),
                'path': f'Bm.City.DynamicBG_ruin.{self.name}_{cnt:03}',
            }

            self.png_to_gi(png_file_ruin, gi_file_ruin, fmt=0, opt=32)
            cnt += 1

        self.write_install(dst / self.path / 'INSTALL.TXT', [self.path / pkg_name])

        main_dat = DAT.from_dict(main)
        main_dat.to_dat(dst / self.path / 'CFG' / 'Main.dat', fmt='HDMain', sign=True)
        cachedata_dat = DAT.from_dict(cachedata)
        cachedata_dat.to_dat(dst / self.path / 'CFG' / 'CacheData.dat', fmt='HDCache', sign=True)

        self.pack_folder(tmp / 'pkg', tmp / pkg_name, 9)
        self.copy_file(tmp / pkg_name, dst / self.path / pkg_name)

        self.write_modinfo(
            dst / self.path / 'ModuleInfo.txt',
            {
                'Name': self.name,
                'Section': 'Den',
                'Author': 'denball, Marshalska Rokossovsky, Arti',
                'Conflict': '',
                'Dependence': 'DenDynamicBG',
                'Priority': 0,
                'Languages': 'Rus,Eng',
                'SmallDescription': 'Пак с фонам для мода на динамические фоны',
                'FullDescription': 'Огромный пак всевозможных фонов для планет и станций.\n'
                f'{rangers.modbuilder.stateless_rus}\n'
                f'{rangers.modbuilder.legal_rus}\n',
                #
                'SmallDescriptionEng': 'Pack with backgrounds for mod with dynamic BGs',
                'FullDescriptionEng': 'A large pack with lots of backgrounds for planets and stations.\n'
                f'{rangers.modbuilder.stateless_eng}\n'
                f'{rangers.modbuilder.legal_eng}\n',
            },
        )

    def rename_bgs(self) -> None:
        cnt = 0
        pics = Path() / 'bg'
        for folder in pics.glob('**/'):
            cnt = 0
            for file in folder.glob('*.png'):
                file.rename(file.with_suffix('.png.tmp'))

            for file in folder.glob('*.png.tmp'):
                file.rename(file.with_name(f'{cnt:03}.png'))
                cnt += 1

    def delete_empty(self) -> None:
        for file in Path('.tmp/').glob('**/*.png'):
            print(file)
            if file.stat().st_size == 0:
                print(f'Deleted empty file: {file}')
                file.unlink()


if __name__ == '__main__':
    Builder()._build()
