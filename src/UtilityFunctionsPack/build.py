from __future__ import annotations
import os
import traceback
import shutil
import os.path
import warnings
from pathlib import Path

import rangers
import rangers.modbuilder


class CompilationError(Exception):
    ...


def exec_cmd(*args: object) -> int:
    return os.system(' '.join(map(str, args)))


class Builder(rangers.modbuilder.ModBuilder):
    name = 'UtilityFunctionsPack'

    path = Path(f'Mods/Tweaks/{name}/')

    def build(self, src: Path, dst: Path) -> None:
        tmp = src / '.tmp'

        self.txt_to_dat(
            src / 'Main.txt',
            dst / self.path / 'CFG' / 'Main.dat',
            fmt='HDMain',  # dont sign!
        )

        self.write_modinfo(
            dst / self.path / 'ModuleInfo.txt',
            {
                'Name': self.name,
                'Section': 'Tweaks',
                'Author': 'Klaxons, denball',
                'Conflict': '',
                'Dependence': '',
                'Priority': 6,
                'Languages': 'Rus,Eng',
                'SmallDescription': 'Добавляет набор полезных функций для моддинга',
                'FullDescription': 'Данный мод добавляет набор полезных функций для моддинга, но сам по себе дополнением не является.',
                #
                'SmallDescriptionEng': 'SmallDescriptionEng',
                'FullDescriptionEng': 'FullDescriptionEng',
            },
        )

        dll_src = src / 'main.cpp'
        dll_out = tmp / 'UtilityFunctions.dll'

        if not dll_out.exists() or dll_out.stat().st_mtime < dll_src.stat().st_mtime:
            dll_out.parent.mkdir(parents=True, exist_ok=True)
            res = exec_cmd(
                'g++32',
                '-m32',
                '-O3',
                '-static-libgcc',
                '-static-libstdc++',
                '-shared',
                '-o',
                dll_out,
                dll_src,
            )
            if res:
                raise CompilationError

        self.copy_file(dll_out, dst / self.path / 'DATA' / 'UtilityFunctions.dll')


if __name__ == '__main__':
    Builder()._build()
