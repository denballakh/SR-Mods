from pathlib import Path

import rangers.modbuilder


class Builder(rangers.modbuilder.ModBuilder):
    name = 'DenSettingsControl'

    path = Path(f'Mods/Den/{name}/')

    def build(self, src: Path, dst: Path) -> None:
        (dst / self.path).mkdir(parents=True, exist_ok=True)
        (dst / self.path / 'CFG').mkdir(parents=True, exist_ok=True)
        (dst / self.path / 'CFG' / 'Rus').mkdir(parents=True, exist_ok=True)
        (dst / self.path / 'CFG' / 'Eng').mkdir(parents=True, exist_ok=True)
        (dst / self.path / 'DATA').mkdir(parents=True, exist_ok=True)

        self.txt_to_dat(
            src / 'Lang.txt',
            dst / self.path / 'CFG' / 'Rus' / 'Lang.dat',
            fmt='HDMain',
        )
        self.txt_to_dat(
            src / 'Lang.txt',
            dst / self.path / 'CFG' / 'Eng' / 'Lang.dat',
            fmt='HDMain',
        )
        self.txt_to_dat(
            src / 'Main.txt',
            dst / self.path / 'CFG' / 'Main.dat',
            fmt='HDMain',
        )
        self.txt_to_dat(
            src / 'CacheData.txt',
            dst / self.path / 'CFG' / 'CacheData.dat',
            fmt='HDCache',
        )

        self.png_to_gi(
            src / 'BG_AA.png',
            dst / self.path / 'DATA' / 'BG_AA.gi',
        )
        self.png_to_gi(
            src / 'BG_Diff.png',
            dst / self.path / 'DATA' / 'BG_Diff.gi',
        )

        self.write_modinfo(
            dst / self.path / 'ModuleInfo.txt',
            {
                'Name': self.name,
                'Section': 'Den',
                'Author': 'denball',
                'Conflict': 'SettingsControl',
                'Dependence': 'DenButtonsPack',
                'Priority': 0,
                'Languages': 'Rus',  # ,Eng
                'SmallDescription': 'Добавляет панель, с помощью которой можно менять настройки партии',
                'FullDescription': 'Добавляет панель на экран космоса, с помощью которой можно менять сложность партии и тонкие настройки прямо во время игры.\n'
                'В космосе в левом верхнем углу появятся две кнопки: одна для изменения ТН, другая для изменения сложности.\n'
                f'{rangers.modbuilder.stateless_rus}',
                'SmallDescriptionEng': 'Adds a panel for editing settings of game',
                'FullDescriptionEng': 'Adds a panel in space, with which you can change game settings and advanced adjustments right while playing.\n'
                'You\'ll find two new buttons in the left upper corner: one for changing advanced adjustments, another - for changing difficulty.\n'
                f'{rangers.modbuilder.stateless_eng}\n'
                f'{rangers.modbuilder.warn_eng % "Mod isnt translated to english"}\n',
            },
        )


if __name__ == '__main__':
    Builder()._build()
