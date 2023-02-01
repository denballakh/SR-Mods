from __future__ import annotations
from typing import TYPE_CHECKING, Final, TypeVar

from pathlib import Path
import traceback
import importlib.machinery
import importlib.util
import shutil
import zipfile

from tqdm import tqdm

from rangers.modbuilder import ModBuilder

if TYPE_CHECKING:
    T = TypeVar('T')

    def tqdm(x: T) -> T:
        ...


def load_builder(file: Path) -> ModBuilder:
    loader = importlib.machinery.SourceFileLoader(file.stem, str(file))
    spec = importlib.util.spec_from_loader(file.stem, loader)
    assert spec, (spec, file)
    module = importlib.util.module_from_spec(spec)
    loader.exec_module(module)
    builder_cls: type[ModBuilder] = module.Builder
    builder = builder_cls()
    return builder


def pack_to_zip(folder: Path, dst: Path) -> None:
    print(f'Packing folder {folder} into {dst}...')

    if not dst.exists():
        dst.parent.mkdir(parents=True, exist_ok=True)
        with zipfile.ZipFile(dst, 'w', allowZip64=False) as zf:
            for file in tqdm(list(folder.glob('**/*'))):
                zf.write(file, file.relative_to(folder))


groups: Final[dict[str, list[str]]] = {
    'DenMods': [
        'DenSettingsControl',
        'DenButtonsPack',
        # 'DenBlockMinimap',
        'DenDynamicBG',
        'DenDatVersion',
        'DenGalaxyMapKlingColors',
    ],
    'DenBG': [
        'DenBG',
        'DenNightBG',
    ],
    'DenPython': [
        'DenPython',
    ],
    'UtilityFunctionsPack': [
        'UtilityFunctionsPack',
    ],
}


def main(
    p_src: Path = Path('./src/'),
    p_build: Path = Path('./build/'),
    p_dist: Path = Path('./dist/'),
) -> None:
    builders: dict[str, ModBuilder] = {}

    for p_builder in list(p_src.glob('**/build.py')):
        try:
            builder = load_builder(p_builder)
            builder._src = p_builder.parent
            builders[builder.name] = builder

        except Exception:
            traceback.print_exc()

    builder_groups: dict[str, list[ModBuilder]] = {}
    for group_name, builder_names in groups.items():
        res = list[ModBuilder]()
        for builder_name in builder_names:
            res.append(builders.pop(builder_name))
        builder_groups[group_name] = res

    if builders:
        print('Unused builders:')
        print(builders)
        print()

    for group_name, group_builders in builder_groups.items():
        p_build_gr = p_build / group_name

        for builder in group_builders:
            print(f'Building {builder.name}...')
            builder.build(builder._src, p_build_gr)

        pack_to_zip(p_build_gr, p_dist / f'{group_name}.zip')

        print()


if __name__ == '__main__':
    main()
