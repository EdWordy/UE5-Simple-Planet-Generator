# UE5-Simple-Planet-Generator
A simple planet generation plugin for unreal 5

---
### FEATURES
- Uses simplex noise and procedural mesh components to make a simple planet with varied terrain
- Biomes are via vertex colours
- Basic orbit camera: arrow keys to rotate, mouse wheel to zoom
- Adjust the generation parameters for different results (remember to adjust biomes too, which are manually heightmapped)

---
### USAGE
- Place a `BP_PlanetActor` in your map (it may hardlock for 30 sec to 2 min while it loads -- turn off auto update in the generation parameters to allevite this for every little update)
- Place a `BP_Star_Basic` in the map and adjust to a comfortable distance, tweak `intensity` as neeeded
- Place the player start at the planet origin
- Press play!

---
###  NOTES
- The basic star is very basic.
- The planet generation technique is very simple -- could be refined ofc.
- MIT license
