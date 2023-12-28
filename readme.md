# XIVRP-Formatter

This is designed to take your logs from your roleplaying sessions in FFXIV and format them up into
a nice looking HTML page, that's easy to re-read, share with others, an embed pictures into.

Besides formatting it into a simple, readability-focused, HTML page, it also works to make your
sessions easier and more enjoyable to read by converting simple formatting into styled HTML,
combining continued messages, removing unrelated messages, embedding images intelligently, and
picking your session out of the noise of a busy venue and even across multiple sessions and even
different logs of different formats.

Ready to get started? Check out the [instructions](#instructions) below.

Need some help, or want to get more advanced? Check out
the [wiki](https://github.com/zbee/xivrp-formatter/wiki).

Want to contribute? Check out
the [contributing guide](https://github.com/zbee/xivrp-formatter/wiki/contributing).
or the Sponsor section along the right.

## Features

### Core

These considerations and functionalities are baked into the program at its core for the best
experience.

- Great for reading on desktop or mobile.
- Faster than reading the log itself, and easier to read than how you originally wrote it.
- Easy to share, with everything encoded into one file.
- Lightweight, standalone, portable, and easy to use with defaults that don't even need adjusted
  for a great output.
- Saving of settings for optional use next-run, and command-line arguments for specific settings
  or sweeping defaults, intelligent and automatic or manual and fine-grained settings, so you can
  use the program however you like.

### Toggles

All of these are optional and can be turned on or off as you like per-run.

- Remove Out-Of-Character messages from the log.
- Highlight `~emphatic~ /text/ *used*` in the log.
- Combine messages that are continued, with various detections for this.
- Embed images depending on when they were taken during the session.
- Find gaps in sessions that spanned multiple sittings and remove them, for real writing lengths.
- Remove time data from the output for greater immersion.

<details><summary style="cursor:pointer"><h3 style="display:inline">Planned Features (ToDo)</h3></summary>

(Roughly in order of priority)

- Designating cover images for the output, and styling it a variety of ways.
- Profile images that match character names in the provided log.
- Finding sessions based on provided character names.
- Finding images automatically. (e.g. finding your XIV folder and checking for screenshots, or
  finding your gshade/reshade settings and checking where screenshots are saved)
- Finding log files automatically. (e.g. finding your XIVLauncher folder and checking where the
  plugins store their logs)
- Finding roleplay sessions within a larger log.
- Finding a roleplay session continued across multiple logs and combining them into one.
- More detection for continued messages, e.g. simply the same person talking twice in a row.

</details>

## Examples

<img src="example_usage.jpg" style="display:inline" alt="Example usage" /><img src="example_output.jpg" style="display:inline" alt="Example output" />

## Instructions

### Quick-Start

- Check [releases](https://github.com/zbee/xivrp-formatter/releases) for the latest version.
- Download the `XIVRP-Formatter.zip` file and extract it somewhere.
- Place your logs next to the executable.
- Place any images taken during the session next to the executable.
- Run the executable and follow the prompts.
- Open the generated HTML file in your browser.

## Supports

The following log types are supported:

- [ChatScan](https://github.com/serifine/XIV-Chat-Scanner) logs
- PNG Images in the same folder as the chat logs

### Planned Support

The following log types are planned for future support:

- Discord chat copy-paste (or pasted into a file)
- [Chat 2](https://git.anna.lgbt/anna/ChatTwo/src/branch/main/ChatTwo) database
- [GobChat](https://github.com/MarbleBag/Gobchat) logs
- [XIVLogger](https://github.com/cadaeix/XIVLogger) logs

## Building

I don't know, sorry; this is my first c++ project, after building the core of this for a class
project after wanting a tool like this for some time and then continuing on with the project.

> Theoretically, it should just be good to go as-is in c++20 with cmake on Windows 10.

That said: It was built in CLion with CMake, an included MinGW toolchain, on Windows 10. This
information, along with the `.idea/` folder - which includes the run and build configs - should
hopefully be enough produce a workable build.

## License

This software is licensed under GPLv3, and as such can be shared and modified freely.

`XIVRP-Formatter  Copyright (C) 2024  Ethan Henderson <ethan@zbee.codes>`

```
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
```

## Attribution

- [nlohman's JSON library](https://github.com/nlohmann/json)
- [HowardHinnant's date library](https://github.com/HowardHinnant/date)
- [Tomáš Andrle's templating library](https://www.catnapgames.com/2013/04/09/nltemplate-html-template-library-for-c/)
- [heifner's base64 library](https://github.com/heifner/base64)
- [tailwindlabs' tailwindcss](https://github.com/tailwindlabs/tailwindcss)
