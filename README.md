![main](img/main.png)

# LeagueSkinChanger
LeagueSkinChanger is internal skin changer for League of Legends.

- Use any available skin for your champion, or any other in game, enemy or ally
- Automatic skins database update
- Change skin for lane minions
- In-game configuration with <a href="https://github.com/ocornut/imgui">ImGui</a>
- Configure your jungle mobs skins like baron, red, blue
- Change skins anytime and unlimited times in single game
- Support for spectator mode
- <a href="https://github.com/nlohmann/json">JSON</a> based configuration saving & loading

# Usage
- Clone the source with: git clone --recursive https://github.com/B3akers/LeagueSkinChanger.git
- Compile using your preferred compiler
- Inject the resulting library into the game or use league_skin_injector
- Press <kbd>Insert</kbd> to bring up the menu.
- Select skin for you, your teammates, enemies, wards, etc.

## Note
If you want to use Visual Studio:
- Use Visual Studio 2019 
- Retarget solution to latest version:
	* Right click solution in Solution Explorer
	* Click "Retarget solution"
	* Select "lastest installed version"
	* Select both projects and hit "ok"
- Solution configuration should be: Debug | x86

# License
<b>This project is licensed under the MIT license</b>

See the "<a href="https://github.com/b3akers/LeagueSkinChanger/blob/master/LICENSE">LICENSE</a>" file for more details.
