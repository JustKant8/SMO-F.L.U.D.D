# Starlight — Fludd Odyssey

This repo contains a modification to Super Mario Odyssey that adds Fludd (the jetpack from Super Mario Sunshine) into the game.

## Download

When released there will be a download link in this github or an alternate download on GameBanana. Note: This only works for SMO version.1.0.0

## Controls

- Hover - hold (R) while in the air
- Rocket - hold (R) to charge and press(A) when ready
- Turbo - hold (R) to charge while on ground/water when ready

## Prerequisites

- [devkitPro](https://devkitpro.org/) 
- Python 3
- The [Keystone-Engine](https://www.keystone-engine.org/) Python Module

## Building

Build has only been tested on WSL2 running Ubuntu 20.04.1.

Just run:
```
DEVKITPRO={path_to_devkitpro} make
```

On Ubuntu (and other Debian-based systems), devkitPro will be installed to `/opt/devkitpro` by default:

```
DEVKITPRO=/opt/devkitpro/ make
```

## Installing (Atmosphère)

After a successful build, simply transfer the `atmosphere` folder located inside `starlight_patch_100` to the root of your switch's SD card.

---

# Credits
- [OdysseyReversed](https://github.com/shibbo/OdysseyReversed) original decomp repo
- [open-ead](https://github.com/open-ead/sead) sead Headers
- [Bryce](https://github.com/brycewithfiveunderscores/Starlight-SMO-Example/) Original Starlight fork for SMO
- [CraftyBoss] Base project that this mod is made off of

# Starlight (Original README)
An enviroment for linking to Splatoon 2 executable and implementing hooks.

# Starlight Contributors
- [3096](https://github.com/3096)
- [khang06](https://github.com/khang06)
- [OatmealDome](https://github.com/OatmealDome)
- [Random0666](https://github.com/random0666)
- [shadowninja108](https://github.com/shadowninja108)
- [shibbo](https://github.com/shibbo) - Repo based on their work on OdysseyReversed
- [Thog](https://github.com/Thog) - Expertise in how rtld is implemented

# Starlight Credits
- devkitA64
- libnx - switch build rules
