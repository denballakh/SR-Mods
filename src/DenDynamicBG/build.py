from pathlib import Path

import rangers.modbuilder


class Builder(rangers.modbuilder.ModBuilder):
    name = 'DenDynamicBG'

    path = Path(f'Mods/Den/{name}/')

    def build(self, src: Path, dst: Path) -> None:
        (dst / self.path).mkdir(parents=True, exist_ok=True)
        (dst / self.path / 'CFG').mkdir(parents=True, exist_ok=True)
        (dst / self.path / 'CFG' / 'Rus').mkdir(parents=True, exist_ok=True)
        (dst / self.path / 'CFG' / 'Eng').mkdir(parents=True, exist_ok=True)

        self.txt_to_dat(
            src / 'Lang.txt',
            dst / self.path / 'CFG' / 'Rus' / 'Lang.dat',
            fmt='HDMain',
            sign=True,
        )
        self.txt_to_dat(
            src / 'Lang.txt',
            dst / self.path / 'CFG' / 'Eng' / 'Lang.dat',
            fmt='HDMain',
            sign=True,
        )
        self.txt_to_dat(
            src / 'Main.txt',
            dst / self.path / 'CFG' / 'Main.dat',
            fmt='HDMain',
            sign=True,
        )
        self.txt_to_dat(
            src / 'CacheData.txt',
            dst / self.path / 'CFG' / 'CacheData.dat',
            fmt='HDCache',
            sign=True,
        )

        self.write_modinfo(
            dst / self.path / 'ModuleInfo.txt',
            {
                'Name': self.name,
                'Section': 'Den',
                'Author': 'denball',
                'Conflict': '',
                'Dependence': '',
                'Priority': 0,
                'Languages': 'Rus,Eng',
                'SmallDescription': 'Разные фоны на планетах и станциях',
                'FullDescription': 'Разные населенные планеты и станции будут иметь разные фоны.'
                '\n'
                'Подключение этих модов добавит фонов в общий набор фонов:\n'
                ' - DenBG (куча фонов планет и станций)\n'
                ' - KlaxonsCoalitionBG (фоны планет из КР2)\n'
                ' - KlaxonsStationsBG (фоны станций из КР2)\n'
                ' - FairanBG (фоны планет)\n'
                ' - PolarCoalitionBG (фоны планет)\n'
                ' - PolarStationsBG (фоны станций)\n'
                ' - PolarPirateBG (фоны пиратских планет)\n'
                '\n'
                'Чтобы отключить мод во время партии, нужно:\n'
                ' - ввести этот код в консоль девкита или чита SUDO:\n'
                '       <color=0,0,255>ShipDeleteCustomShipInfo(Player(),\'Player_DynamicBG\');</color>\n'
                ' - сохраниться\n'
                ' - отключить мод\n'
                ' - загрузить сейв и играть дальше, все фоны сбросятся на обычные\n'
                '\n'
                '{rangers.modbuilder.legal_rus}\n'
                '{rangers.modbuilder.contentless_rus}\n',
                #
                'SmallDescriptionEng': 'Different backgrounds on planets and stations',
                'FullDescriptionEng': 'Different planets and stations will have different backgrounds'
                '\n'
                'Enabling of this mods will add backgrounds in total set of backgrounds:\n'
                ' - DenBG (a lot of backgrounds for planets and stations)\n'
                ' - KlaxonsCoalitionBG (backgrounds of planets from SR2)\n'
                ' - KlaxonsStationsBG (backgrounds of stations from SR2)\n'
                ' - FairanBG (backgrounds of planets)\n'
                ' - PolarCoalitionBG (backgrounds of planets)\n'
                ' - PolarStationsBG (backgrounds of stations)\n'
                ' - PolarPirateBG (backgrounds of pirate planets)\n'
                '\n'
                'For disabling this mod during the game you need:\n'
                ' - enter this code in DevKit or SUDO-cheat console:\n'
                '       <color=0,0,255>ShipDeleteCustomShipInfo(Player(),\'Player_DynamicBG\');</color>\n'
                ' - make a save\n'
                ' - disable mod\n'
                ' - load save and play. All backgrounds will change on default ones.\n'
                '\n'
                '{rangers.modbuilder.legal_eng}\n'
                '{rangers.modbuilder.contentless_eng}\n',
            },
        )


if __name__ == '__main__':
    Builder()._build()
