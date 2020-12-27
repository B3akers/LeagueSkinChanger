# LeagueSkinChanger
<p align="center">
  <img height=400 src="/img/main.png" >
  <br>
  <i>Mmm yes, chicken</i>
</p>

`LeagueSkinChanger` is internal skin changer for League of Legends.

- Use any available skin for your champion, or any other in game, enemy or ally.
- Automatic skins database update.
- Change skin for lane minions.
- In-game configuration with <a href="https://github.com/ocornut/imgui">ImGui</a>.
- Configure your jungle mobs skins like baron, red, blue.
- Change skins anytime and unlimited times in single game.
- Support for spectator mode.
- <a href="https://github.com/nlohmann/json">JSON</a> based configuration saving & loading.

# Building
1. Clone the source with `git clone --recursive https://github.com/B3akers/LeagueSkinChanger.git`
2. Build in Visual Studio 2017/19 with configuration "Any | x86" (build will fail if set to "x64")

# Usage
1. Use `league_skin_injector` or inject the resulting DLL into the game yourself.
   - *Administrator* privilege may be needed if failed to inject.
   - League client can crash if injected before going into arena.
      - A workaround is to not inject until you are in the arena (you will need to be fast to not disrupt the game).
2. Press <kbd>Insert</kbd> to bring up the menu.
3. Select skin for you, your teammates, enemies, wards, etc.

# License
<b>This project is licensed under the MIT license</b>

See the <a href="https://github.com/b3akers/LeagueSkinChanger/blob/master/LICENSE">LICENSE</a> file for more details.
